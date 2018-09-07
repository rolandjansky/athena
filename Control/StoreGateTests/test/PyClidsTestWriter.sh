#!/bin/bash
# Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration.
#
# Script running the test_recordStlClids_jobOptions.py test jobOptions for CTest.
#

# Return the correct code:
set -e

# Run the test:
athena.py StoreGateTests/test_recordStlClids_jobOptions.py
