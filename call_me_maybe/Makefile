VENV_PATH = /sgoinfre/brportos/call_me_maybe

all: run

venv:
	@if [ ! -d "$(VENV_PATH)" ]; then uv venv $(VENV_PATH); fi
	@ln -sfn $(VENV_PATH) .venv

install: venv
	@uv pip install --python .venv/bin/python -r pyproject.toml