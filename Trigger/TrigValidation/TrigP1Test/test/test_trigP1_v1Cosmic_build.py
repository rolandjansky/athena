#!/usr/bin/env python
# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

# art-description: Trigger athenaHLT test of the Cosmic_run3_v1 menu on physics_Main stream from a cosmic run, then running BS decoding follows the athenaHLT process
# art-type: build
# art-include: master/Athena
# art-include: 22.0/Athena

from TrigValTools.TrigValSteering import Test, ExecStep, CheckSteps

##################################################
# Helper functions to build the test steps
##################################################
from TrigP1Test.TrigP1TestSteps import filterBS, decodeBS

##################################################
# Test definition
##################################################

ex = ExecStep.ExecStep()
ex.type = 'athenaHLT'
ex.job_options = 'TriggerJobOpts/runHLT_standalone.py'
ex.input = 'data_cos'
ex.max_events = 200
precommand = ''.join([
  "setMenu='Cosmic_run3_v1';",
  "doCosmics=True;",
  "doL1Sim=True;",
  "rewriteLVL1=True;",
  "setDetDescr='ATLAS-R3S-2021-01-00-02';",
  "condOverride={'/MDT/Onl/T0BLOB':'MDTT0-RUN3-Onl-UPD1-01-BLOB'};"  # TODO: use R3 HLT cond tag when available
])
ex.args = f'-c "{precommand}"'
ex.args += ' -o output'
ex.args += ' --dump-config-reload'

# Extract and decode physics_Main
filterMain = filterBS("Main")
decodeMain = decodeBS("Main")

test = Test.Test()
test.art_type = 'build'
test.exec_steps = [ex, filterMain, decodeMain]
test.check_steps = CheckSteps.default_check_steps(test)

import sys
sys.exit(test.run())
