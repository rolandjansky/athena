#!/usr/bin/env sh
echo $PYPATH
#flake8 `find $PYPATH -name '*.py'`
flake8 --ignore=E,W `find $PYPATH -name '*.py'`
