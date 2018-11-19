#!/bin/bash
# Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration.
#
# BaseLine test.
#

# Run the test:
mkdir -p BaseLine
cd BaseLine
chappy.py PerfMonTests/atn_test_perfMonSvc_baseline_bench.py

