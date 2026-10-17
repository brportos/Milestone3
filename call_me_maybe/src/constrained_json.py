import json
import numpy as np
import string
import re
from typing import Any


def get_valid_continuations(output_text: str, valid_function_names: list[str]) -> list[str]:
    constraint_prefix = '{"name":"'
    if len(output_text) < len(constraint_prefix):
        return [constraint_prefix[len(output_text):]]

    after_constraint_prefix = output_text[len(constraint_prefix):]
    if '"' not in after_constraint_prefix:
        return [name[len(after_constraint_prefix):] + '"' for name in valid_function_names if name.startswith(after_constraint_prefix)]

    func_name = after_constraint_prefix.split('"')[0]
    target = constraint_prefix + func_name + '","parameters":{'
    if len(output_text) < len(target):
        return [target[len(output_text):]]

    return list(string.printable)

def generate_constrained_json(prompt_text: str, cache: Any) -> str:
    cleaned_schemas = []
    for func in cache.raw_functions:
        cleaned_schemas.append({"name": func["name"], "descriptions": func.get("description", ""), "parameters": func.get("parameters", {})})

    schemas_json = json.dumps(cleaned_schemas, separators=(',', ':'))
    prompt = (
        f"System: Output a tool call in JSON matching these schemas: {schemas_json}\n"
        f"User: {prompt_text}\n"
        "Tool Call: "
    )

    token_ids = cache.model.encode(prompt).tolist()[0]
    vacab_size = len(cache.model.get_logits_from_input_ids(token_ids))
    
    output_text = ""
    constraint_prefix = '{"name":"'
    output_text = constraint_prefix

    token_ids.extend(cache.model.encode(constraint_prefix).tolist()[0])
    has_bridge_injected = False

    max_output_tokens = 150

    while (not output_text.replace(" ", "").replace("\n", "").endswith('}}') and len(token_ids) < len(prompt) + max_output_tokens):
        if constraint_prefix in output_text and '", "parameters":{' not in output_text:
            after_constraint_prefix = output_text.split(constraint_prefix)[1]
            possible_names = [n for n in cache.allowed_fn if n.startswith(after_constraint_prefix)]

            if len(possible_names) == 1 and possible_names[0] != after_constraint_prefix:
                remainder = possible_names[0][len(after_constraint_prefix):] + '"'
                output_text += remainder
                token_ids.extend(cache.model.encode(remainder).tolist()[0])
                continue

        if (output_text.endswith('"') and not has_bridge_injected and constraint_prefix in output_text and len(output_text) > len(constraint_prefix)):
            bridge = ',"parameters":{'
            output_text += bridge
            token_ids.extend(cache.model.encode(bridge).tolist()[0])
            has_bridge_injected = True

            func_name = output_text.split('"name":"')[1].split('"')[0]
            if cache.func_params.get(func_name, 99) == 0:
                output_text += "}}"
                break
            
            active_schema = next((f for f in cache.raw_functions if f["name"] == func_name), None)

            if active_schema:
                tiny_schema = json.dumps([{"name": active_schema["name"], "description": active_schema.get("description", ""), "parameters": active_schema.get("parameters", {})}], separators=(',', ':'))
                tiny_prompt = (
                    f"System: Output a tool call in JSON matching these schemas: {tiny_schema}\n"
                    f"User: {prompt_text}\n"
                    f"Tool Call: {output_text}"
                )

                token_ids = cache.model.encode(tiny_prompt).tolist()[0]
            else:
                token_ids.extend(cache.model.encode(bridge).tolist()[0])
            
            continue
        
        rules = get_valid_continuations(output_text, cache.allowed_fn)
        logits = np.array(cache.model.get_logits_from_input_ids(token_ids))
        mask = np.zeros(vocab_size, dtype=bool)

        if len(rules) > 10:
            matching = re.search(r'"name"\s*:\s*"([^"]+)', output_text)
            func_name = matching.group(1) if matching else ""

            params_str = output_text.split('"parameters"')[1] if '"parameters"' in output_text else ""

            if params_str:
                in_string = False
                last_structural_colon = -1
                last_structural_comma = -1
                last_structural_brace = -1

                for i, char in enumerate(params_str):
                    if char == '"':
                        if i == 0 or params_str[i-1] != '\\':
                            in_string = not in_string
                    elif not in_string:
                        if char == ':':
                            last_structural_colon = i
                        elif char == ',':
                            last_structural_comma = i
                        elif char == '}':
                            last_structural_brace = i
                
                is_inside_value = (last_structural_colon > last_structural_comma and last_structural_colon > last_structural_brace)

                active_key = ""
                if is_inside_value:
                    keys_found = re.findall(r'"([^"]+)"\s*:', params_str)
                    if keys_found:
                        active_key = keys_found[-1]
                
                expected_type = cache.param_types.get(func_name, {}).get(active_key, "Any")

                param_count = len(re.findall(r'"([^"]+)"\s*:', params_str))
                target_count = cache.func_params.get(func_name, 99)

                if is_inside_value and expected_type == "number":
                    mask = cache.p4_numbers_only.copy()

                    if param_count == target_count:
                        for i, s in cache.clean_dict_items:
                            if ',' in s:
                                mask[i] = False
                
                elif is_inside_value and in_string:
                    mask = cache.p4_mask.copy()

                    if param_count == target_count:
                        for i, s in cache.clean_dict_items:
                            if ' ' in s:
                                mask[i] = False
                        
                        if re.search(r'"regex"\s*:\s*"$', params_str):
                            for i, s in cache.clean_dict_items:
                                if not any(s.startswith(c) for c in ['[', '\\']):
                                    mask[i] = False
                
                elif param_count == target_count:
                    clean_str = output_text.strip()
                    if clean_str.endswith('"'):
                        output_text = clean_str + "}}"
                        print(f"\rGenerating: {output_text}", end="", flush=True)
                        break
                    elif clean_str.endswith('}'):
                        output_text = clean_str + "}"
                        print(f"\rGenerating: {output_text}", end="", flush=True)
                        break
                    elif clean_str.endswith(','):
                        output_text = clean_str[:-1] + "}}"
                        print(f"\rGenerating: {output_text}", end="", flush=True)
                        break
                    else:
                        mask = cache.p4_no_comma.copy()
                
                else:
                    mask = cache.p4_mask.copy()
                    is_expecting_key = (params_str.strip().endswith('{') or params_str.strip().endswith(','))
                    if is_expecting_key:
                        for i, s in cache.clean_dict_items:
                            cleaned = s.strip()
                            if not (cleaned.startswith('"') or not cleaned):
                                mask[i] = False
        
        else:
            for i, s in cache.mini_dict:
                if any(rule.startswith(s) for rule in rules):
                    mask[i] = True
        
        logits[~mask] = -np.inf

        best_id = int(np.argmax(logits))
        output_text += cache.vocab_dict.get(best_id, "")
        token_ids.append(best_id)

        if (output_text.endswith('"') and not has_bridge_injected and constraint_prefix in output_text):
            bridge = ',"parameters":{'
            output_text += bridge
            token_ids.extend(cache.model.encode(bridge).tolist()[0])
            has_bridge_injected = True
            continue
        else:
            print(f"\rGenerating: {output_text}", end="", flush=True)
    
    print()
    return output_text
