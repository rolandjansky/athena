#!/usr/bin/env python
# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# art-description: MC muon slice tests with phase-1 L1Muon simulation enabled
# art-type: build
# art-include: master/Athena
# Skipping art-output which has no effect for build tests.
# If you create a grid version, check art-output in existing grid tests.

from TrigValTools.TrigValSteering import Test, ExecStep, CheckSteps

ex = ExecStep.ExecStep()
ex.type = 'athena'
ex.job_options = 'TriggerJobOpts/runHLT_standalone.py'
ex.input = 'ttbar_phase1'
ex.threads = 1
precommand = ''.join([
  "setMenu='LS2_v1';",
  "enableL1Phase1=True;",
  "enableL1CaloLegacy=True;",  # TODO: fix LvlSimulationConfig to be able to disable this
  "doEmptyMenu=True;",
  "doMuonSlice=True;",
  "doWriteBS=False;",
  "doWriteRDOTrigger=True;",
  "forceEnableAllChains=True;",
])
ex.args = '-c "{:s}"'.format(precommand)

test = Test.Test()
test.art_type = 'build'
test.exec_steps = [ex]
test.check_steps = CheckSteps.default_check_steps(test)

import sys
sys.exit(test.run())
