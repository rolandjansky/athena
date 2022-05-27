#!/usr/bin/env bash
# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

# Pilot beam collisions need a special export and preExec
export TNS_ADMIN=/cvmfs/atlas.cern.ch/repo/sw/database/DBRelease/current/oracle-admin

RunWorkflowTests_Run3.py -a q449 --CI -r -w DataReco -e '--maxEvents 500 --preExec "from AthenaConfiguration.AllConfigFlags import ConfigFlags; ConfigFlags.Trigger.triggerConfig = \"DB\"; DQMonFlags.useTrigger=False; DQMonFlags.doHLTMon=False;"' --threads 8 --no-output-checks
