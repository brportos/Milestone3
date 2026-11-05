import json
import numpy as np
import string
import re
from typing import Any


def get_valid_continuations(output_text: str, valid_function_names: list[str]) -> list[str]:
    ...

def generate_constrained_json(prompt_text: str, cache: Any) -> str:
    ...