#!/bin/bash
#
# Script running the AthTPCnvSvc_test.py test jobOptions for CTest.
#

# Return the correct code:
set -e

# Run the test:
athena.py AthenaServices/AthTPCnvSvc_test.py
