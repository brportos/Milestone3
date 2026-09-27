export UV_CACHE_DIR ?= /sgoinfre/brportos/.cache/uv
export HF_HOME ?= /sgoinfre/brportos/.cache/huggingface
VENV_PATH ?= /sgoinfre/brportos/mmm_venv

venv:
	@rm -rf .venv
	@if [ ! -d "$(VENV_PATH)" ]; then uv venv $(VENV_PATH); fi
	@ln -sfn $(VENV_PATH) .venv

install: venv
	uv pip install -r pyproject.toml

run:
	uv run python -m src

debug:
	uv run python -m pdb -m src

clean:
	find . -type d -name "__pycache__" -exec rm -rf {} +
	find . -type d -name ".mypy_cache" -exec rm -rf {} +
	rm -rf .venv $(VENV_PATH)

lint:
	uv run flake8 src/ --exclude=.venv
	uv run mypy src/ --warn-return-any --warn-unused-ignores --ignore-missing-imports --disallow-untyped-defs --check-untyped-defs

lint-strict:
	uv run flake8 src/ --exclude=.env
	uv run mypy src/ --strict