#!/bin/sh

# This is not an ART test, but a unit test

# This should be tested in AthViews rather that in TrigUpgradeTest

if [ -z ${THREADS+x} ]; then export THREADS=2; fi
athena.py --threads=$THREADS AthViews/ViewScheduling.py
unset THREADS
