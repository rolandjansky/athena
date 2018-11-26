#!/bin/sh
# art-type: build
# art-include: master/Athena

if [ -z ${THREADS+x} ]; then export THREADS=2; fi
athena.py --threads=$THREADS AthViews/ViewScheduling.py
unset THREADS
