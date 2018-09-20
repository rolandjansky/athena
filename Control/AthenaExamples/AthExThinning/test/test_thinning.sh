#!/bin/bash
# Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration.
#
# test_thinning
#

# Return the correct code:
set -e

# Run the test:
mkdir -p test_thinning
cd test_thinning
chappy.py AthExThinning/test_thinning.py
