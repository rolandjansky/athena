#!/bin/bash
#
# Script running the CondAlgsOpts.py test jobOptions for CTest.
#

# Return the correct code:
set -e

# Run the test:
athena.py --threads=3 AthExHive/CondAlgsOpts.py | grep AlgD1 | grep test | awk '{print $6,$7,$8,$9}' | sort -n
