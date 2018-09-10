#!/bin/bash
# Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration.
#
# Script for test_cfgItemList.
#

# Return the correct code:
set -e

# Run the test:
python -c'import ControlTest.test_cfgItemList'
