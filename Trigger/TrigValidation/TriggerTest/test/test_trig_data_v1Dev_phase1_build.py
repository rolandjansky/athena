#!/usr/bin/env python
# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# art-description: Trigger BS->RDO_TRIG athena test of the Dev_pp_run3_v1 menu with rerunL1 enabled with Phase1 L1 items
# art-type: build
# art-include: master/Athena
# Skipping art-output which has no effect for build tests.
# If you create a grid version, check art-output in existing grid tests.

from TrigValTools.TrigValSteering import Test, ExecStep, CheckSteps

ex = ExecStep.ExecStep()
ex.type = 'athena'
ex.job_options = 'TriggerJobOpts/runHLT_standalone.py'
ex.input = 'data'
ex.threads = 1
precommand = ''.join([
  "setMenu='LS2_v1_TriggerValidation_prescale';",  # LS2_v1 soon to be renamed to Dev_pp_run3_v1
  "doL1Sim=True;",
  "enableL1MuonPhase1=True;",
  "enableL1CaloPhase1=True;",
  "doWriteBS=False;",
  "doWriteRDOTrigger=True;",
])
ex.args = '-c "{:s}"'.format(precommand)

test = Test.Test()
test.art_type = 'build'
test.exec_steps = [ex]
test.check_steps = CheckSteps.default_check_steps(test)
test.check_steps.remove(test.get_step("ZeroCounts"))

checkFile = test.get_step("CheckFile")
checkFile.input_file = 'RDO_TRIG.pool.root'

import sys
sys.exit(test.run())
