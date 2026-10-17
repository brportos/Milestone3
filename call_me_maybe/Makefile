SGOINFRE  = /sgoinfre/$(USER)
VENV_PATH = $(SGOINFRE)/call_me_maybe
PYTHON    = .venv/bin/python3

export UV_CACHE_DIR = $(SGOINFRE)/.cache/uv
export HF_HOME      = $(SGOINFRE)/.cache/huggingface

.PHONY: all venv install run debug clean lint lint-strict

all: run

venv:
	@if [ ! -d "$(VENV_PATH)" ]; then uv venv $(VENV_PATH); fi
	@ln -sfn $(VENV_PATH) .venv

install: venv
	@uv pip install --python $(PYTHON) -r pyproject.toml

run: install
	@$(PYTHON) -m src

debug: install
	@$(PYTHON) -m pdb -m src

clean:
	@find . -type d \( -name "__pycache__" -o -name ".mypy_cache" \) \
		-prune -exec rm -rf {} +
	@rm -rf .venv $(VENV_PATH)

lint: install
	@$(PYTHON) -m flake8 . --exclude=.venv
	@$(PYTHON) -m mypy . --warn-return-any \
		--warn-unused-ignores \
		--ignore-missing-imports \
		--disallow-untyped-defs \
		--check-untyped-defs

lint-strict: install
	@$(PYTHON) -m flake8 . --exclude=.venv
	@$(PYTHON) -m mypy . --strict