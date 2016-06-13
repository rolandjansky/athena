#!/bin/bash
#
# $Id: FortranAlgorithm.sh 729263 2016-03-11 17:34:05Z krasznaa $
#
# Script running the FortranAlgorithm intergration test.
#

# Transmit errors:
set -e

# Get the necessary inputs:
preFortAlgEx.sh > /dev/null

# Run the job:
athena.py AthExFortranAlgorithm/FortranAlgorithmOptions.py
