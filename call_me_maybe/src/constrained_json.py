import json
import numpy as np
import string
import re
from typing import Any


def get_valid_continuations(current_text: str, valid_function_names: list[str]) -> list[str]:
    prefix = '{"name":"'
    if len(current_text) < len(prefix):
        return [prefix[len(current_text):]]

    after_prefix = current_text[len(prefix):]
    if '"' not in after_prefix:
        return [name[len(after_prefix):] + '"' for name in valid_function_names if name.startswith(after_prefix)]

    func_name = after_prefix.split('"')[0]
    target = prefix + func_name + '","parameters":{'
    if len(current_text) < len(target):
        return [target[len(current_text):]]
    return list(string.printable)