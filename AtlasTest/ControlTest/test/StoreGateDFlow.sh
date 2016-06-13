#!/bin/bash
#
# $Id: StoreGateDFlow.sh 729263 2016-03-11 17:34:05Z krasznaa $
#
# Script running the StoreGateDFlow intergration test.
#

# Transmit errors:
set -e

# Run the job:
athena.py AthExStoreGateExample/dflow_jobo.py
