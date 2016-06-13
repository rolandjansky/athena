#!/bin/bash
#
# $Id: StoreGateReentrant.sh 729263 2016-03-11 17:34:05Z krasznaa $
#
# Script running the StoreGateReentrant intergration test.
#

# Transmit errors:
set -e

# Run the job:
athena.py AthExStoreGateExample/StoreGateExample_Reentrant_jobOptions.py
