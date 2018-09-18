#!/bin/bash
# Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration.
#
# Script for test_keyStore.
#

# Return the correct code:
set -e

# Run the test:
python -c'import ControlTest.test_keyStore'
