import argparse
from pydantic import Field, BaseModel
from typing import Any, Dict, List
import json


class MaskCache:
    model: Any = Field(...)
    vocab_dict: Dict[int, str] = Field(...)

    allowed_fn: List[str] = Field(...)
    raw_functions:List[Dict[str, Any]] = Field(...)
    func_params: Dict[str, int] = Field(...)
    param_types: Dict[str, Dict[str, Any]] = Field(...)


class Parameter(BaseModel):
    type: str = Field(...)


class ReturnType(BaseModel):
    type: str = Field(...)


class FunctionDefinition(BaseModel):
    name: str = Field(...)
    description: str = Field(..., min_length=5)
    parameters: Dict[str, Parameter] = Field(...)
    returns: ReturnType


class Prompt(BaseModel):
    prompt: str = Field(..., min_length=3)

def parse_arguments() -> argparse.Namespace:
    parser = argparse.ArgumentParser(
        prog="callMeMaybe",
        description="Call Me Maybe: LLM Function Calling Tool",
    )

    parser.add_argument(
        '--functions_definition',
        type=str,
        default="data/input/functions_definition.json",
        help="Path to JSON file containing the functions definitions."
    )

    parser.add_argument(
        '--input',
        type=str,
        default="data/input/function_calling_tests.json",
        help="Path to the file containing the prompts."
    )

    parser.add_argument(
        '--output',
        type=str,
        default="data/output/function_calling_results.json",
        help="Path to the JSON output file."
    )

    parser.add_argument(
        '--model',
        type=str,
        default="Qwen/Qwen3-0.6B",
        help="HuggingFace Model ID"
    )
    return parser.parse_args()

def load_function_def(path: str):
    try:
        with open(path, 'r', encoding="utf-8") as f:
            data = json.load(f)
        return [FunctionDefinition(**item) for item in data]
    except FileNotFoundError:
        raise RuntimeError(f"File not found: {path}")
    except json.JSONDecodeError:
        raise RuntimeError(f"Invalide json file in ({path})")
    except Exception as e:
        raise RuntimeError(f"Error: {e}")

def build_system_prompt(functions):
    lines = [
        "Strict system rules: use only a matching from the list",
        "If no functions matches the user's intent (even if type match), set name=None",
        "Never use an unrelated function for different task",
        "--",
        "Available functions"
    ]
    for fn in functions:
        params = ", ".join(
            f"{name}: {info.type}"
            for name, info in fn.parameters.items()
        )
        lines.append(f" -{fn.name}({params}: {fn.description})")
    lines.append('\noutput only valid json: {"name": "<fn>", "args": {<args>}}')
    return "\n".join(lines)


def load_prompt(path: str):
    try:
        with open(path, "r", encoding="utf-8") as f:
            data = json.load(f)
        return [Prompt(**item) for item in data]
    except FileNotFoundError:
        raise RuntimeError(f"File not found : {path}")
    except json.JSONDecodeError:
        raise RuntimeError(f"Invalid json file: {path}")
    except Exception as e:
        raise RuntimeError(f"Error: {e}")
def main():
    args = parse_arguments()
    functions = load_function_def(args.functions_definition)
    if not functions:
        raise RuntimeError(f"Functions definition can't be empty")
    prompt = load_prompt(args.input)
    if not prompt:
        raise RuntimeError(f"Input can't be empty")
    system = build_system_prompt(functions)
    print(system)

if __name__ == "__main__":
    try:
        main()
    except RuntimeError as e:
        print(f"Error: {e}")
   
