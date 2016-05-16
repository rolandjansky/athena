#!/bin/bash
#
# Script running the TestStopRun.py test jobOptions for CTest.
#

# Return the correct code:
set -e

# Run the test:
athena.py AthenaServices/TestStopRun.py
