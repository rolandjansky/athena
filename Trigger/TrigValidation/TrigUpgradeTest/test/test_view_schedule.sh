#!/bin/sh
# art-type: build
# art-ci: master
if [ -z ${THREADS+x} ]; then export THREADS=2; fi
athena.py --threads=$THREADS AthViews/ViewScheduling.py
unset THREADS
