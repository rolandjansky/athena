#!/bin/bash
#
# $Id: rootcore2cmake.sh 711151 2015-11-27 12:00:55Z krasznaa $
#
# This shell script can be used to transform an AnalysisBase source area
# to be compiled by CMake.
#

# Fail on errors:
set -e

# Get the path of this code:
THIS=$(dirname $0)
BASEDIR=$(cd ${THIS};pwd)

# Extend the python path:
PYTHONPATH=$BASEDIR:$PYTHONPATH

# Now run the main python code:
python $BASEDIR/main.py $*
