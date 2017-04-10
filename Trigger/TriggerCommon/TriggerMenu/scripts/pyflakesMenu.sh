#!/usr/bin/env sh
echo $PYPATH
pyflakes `find $PYPATH -name '*.py'`

exit 0
