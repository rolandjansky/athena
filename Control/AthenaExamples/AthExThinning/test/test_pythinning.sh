#!/bin/bash
# Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration.
#
# test_pythinning
#

# Return the correct code:
set -e

# Run the test:
mkdir -p test_pythinning
cd test_pythinning
chappy.py AthExThinning/test_pythinning.py
