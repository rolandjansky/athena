#!/bin/bash
# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration.
#
# Script for test_handleTest
#

# Return the correct code:
set -e

# Run the test:
athena.py AthExStoreGateExample/HandleTest_jobOptions.py
