#!/bin/bash
#
# $Id: test_interactive_athena.sh 729263 2016-03-11 17:34:05Z krasznaa $
#
# Script running the test_interactive_athena intergration test.
#

# Transmit errors:
set -e

# Run the job:
athena.py ControlTest/test_interactive_athena.py
