# simple makefile to simplify repetitive build env management tasks under posix

PYTHON ?= python
NOSETESTS ?= nosetests

all: clean build test

clean-pyc:
	find . -name "*.pyc" -exec rm {} \;

clean: clean-pyc
	(cd cmt && make clean)

build:
	(cd cmt && make)

test:
	$(NOSETESTS) -v -s

trailing-spaces:
	find . -name "*.py" | xargs perl -pi -e 's/[ \t]*$$//'

check-rst:
	cat README.rst | rst2html.py > __output.html
	firefox __output.html
	rm -f __output.html
