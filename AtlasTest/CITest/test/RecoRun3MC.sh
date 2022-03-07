#!/usr/bin/env bash
# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

# Run-3 MC but with AODFULL so it can be used as input to the HLT DQ monitoring.
# Once ATLASDQ-879 is fixed, this script can be deleted and the test defintion
# (without the --preExec) moved to Athena.cmake.

RunWorkflowTests_Run3.py --CI -r -w MCReco -e '--maxEvents 25 --preExec="all:from AthenaConfiguration.AllConfigFlags import ConfigFlags; ConfigFlags.Trigger.AODEDMSet=\"AODFULL\""' --no-output-checks
