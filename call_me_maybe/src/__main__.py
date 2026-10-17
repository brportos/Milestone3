import argparse
import json
import sys
from pydantic import BaseModel, ValidationError, ConfigDict
from pydantic.dataclasses import dataclass
import pathlib
from llm_sdk import Small_LLM_Model
from src.constrained_json import generate_constrained_json
from datetime import datetime
import string
rom rich import print_json
import numpy as np
from typing import Any, Dict, List, Tuple


@dataclass(config=ConfigDict(arbitrary_types_allowed=True))
class MaskCache:
    model: Any
    vocab_dict: Dict[int, str]

    allowed_fn: List[str]
    raw_functions:List[Dict[str, Any]]
    func_params: Dict[str, int]
    param_types: Dict[str, Dict[str, Any]]

    p4_mask: np.ndarray[Any, Any]
    p4_numbers_only: np.ndarray[Any, Any]
    p4_no_comma: np.ndarray[Any, Any]
    mini_dict: List[Tuple[int, int]]
    clean_dict_items: List[Tuple[int, str]]

class FunctionDef(BaseModel):
    name: str
    description: str
    parameters: Dict[str, Any]
    returns: Dict[str, Any]

class FunctionCallResult(BaseModel):
    prompt: str
    name: str
    parameters: Dict[str, Any]

def parse_arguments() -> argparse.Namespace:
    parser = argparse.ArgumentParser(
        prog="callMeMaybe",
        description="Call Me Maybe: LLM Function Calling Tool",
        usage="uv run python3 -m src [--functions definition "
        "<function_definition_file>] [--input <input_file>] "
        "[--output <output_file>]"
    )

    parser.add_argument(
        '--functions_definition',
        metavar='',
        type=str,
        default="data/input/functions_definition.json",
        help="Path to JSON file containing the functions definitions."
    )

    parser.add_argument(
        '--input',
        metavar='',
        type=str,
        default="data/input/function_calling_tests.json",
        help="Path to the file containing the prompts."
    )

    parser.add_argument(
        '--output',
        metavar='',
        type=str,
        default="data/output/function_calling_results.json",
        help="Path to the JSON output file."
    )

    parser.add_argument(
        '--model',
        type=str,
        default="Qwen/Qwen3-0.6B"
        help="HuggingFace Model ID"
    )
    
    return parser.parse_args()

def load_json_file(filename: str) -> Any:
    if not pathlib.Path(filename).is_file():
        raise SystemExit(f"Error: {filename}")
    
    try:
        with open(filename, 'r') as file:
            json_data = json.load(file)
    except json.JSONDecodeError:
        print(file=sys.stderr)
        sys.exit(1)
    except PermissionError:
        print(f"{filename}", file=sys.stderr)
        sys.exit(1)
    except Exception as e:
        print(f"Error: {filename}", file=sys.stderr)
        sys.exit(1)
    
    return json_data

def main() -> None:
    print("Hello from call-me-maybe!")


if __name__ == "__main__":
    main()
