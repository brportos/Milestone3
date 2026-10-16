VENV_PATH = /sgoinfre/brportos/call_me_maybe

all: run

venv:
	@if [ ! -d "$(VENV_PATH)" ]; then uv venv $(VENV_PATH); fi
	@ln -sfn $(VENV_PATH) .venv

install: venv
	@uv pip install --python .venv/bin/python3 -r pyproject.toml

run:
	@uv run python3 -m src

debug:
	@uv run python3 -m pdb -m src

clean:
	@find . -type d -name "__pycache__" -exec rm -rf {} +
	@find . -type d -name ".mypy_cache" -exec rm -rf {} +
	@rm -rf .venv $(VENV_PATH)

lint:
	@uv run flake8 . --exclude=.venv
	@uv run mypy . --warn-return-any \
			--warn-unused-ignores \
			--ignore-missing-imports \
			--disallow-untyped-defs \
			--check-untyped-defs

lint-strict:
	@uv run flake8 . --exclude=.venv
	@uv run mypy . --strict

