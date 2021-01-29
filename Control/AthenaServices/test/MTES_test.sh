#!/bin/bash
#
# Script running the MTES_test.py test jobOptions
#

# Return the correct code:
set -e

# Run the test:
athena.py --mtes --threads=2 AthenaServices/MTES_test.py
