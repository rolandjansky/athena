#!/bin/bash
#
# Script running the AthExHiveOpts.py test jobOptions for CTest.
#

# Return the correct code:
set -e

# Run the test:
athena.py --threads=4 AthExHive/AthExHiveOpts.py | grep HiveAlgF | grep test | awk '{print $6,$7,$8,$9,$10,$11,$12,$13}' | sort -n
