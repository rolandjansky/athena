#!/bin/bash
# Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration.
#
# test_slimming
#

# Return the correct code:
set -e

# Run the test:
mkdir -p test_slimming
cd test_slimming
chappy.py AthExThinning/test_slimming.py
