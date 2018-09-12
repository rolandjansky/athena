#!/bin/bash
# Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration.
#
# Test script.
#

# Return the correct code:
set -e

# Run the test:
chappy.py StoreGateTests/atn_test_sgProducerConsumer_jobOptions.py
