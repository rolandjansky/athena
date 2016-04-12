#!/bin/bash
#
# $Id: CaloTowerStore_test.sh 730049 2016-03-15 14:19:51Z krasznaa $
#
# Script running the CaloTowerStore_test.py integration test with CTest.
#

# Transmit errors:
set -e

# Run the job:
athena.py CaloUtils/CaloTowerStore_test.py
