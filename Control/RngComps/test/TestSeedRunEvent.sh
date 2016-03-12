#!/bin/bash
#
# $Id: TestSeedRunEvent.sh 729159 2016-03-11 12:58:15Z krasznaa $
#
# Script running the TestSeedRunEvent.py jobOption with CTest in the
# CMake build.
#

# Return errors:
set -e

# Run the test:
athena.py RngComps/TestSeedRunEvent.py
