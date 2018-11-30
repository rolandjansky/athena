#!/bin/bash
# Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration.
#
# NoopAlg_WriteEvtInfo test.
#

# Run the test:
mkdir -p NoopAlg_WriteEvtInfo
cd NoopAlg_WriteEvtInfo
chappy.py PerfMonTests/atn_test_perfMonSvc_noopalg_write_evtinfo.py
