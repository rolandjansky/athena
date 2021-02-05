#!/usr/bin/env python
# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# art-description: Trigger BS->RDO_TRIG athena test of the Dev_pp_run3_v1 menu with rerunL1 enabled and without any HLT chains
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
  "setMenu='LS2_v1';",  # LS2_v1 soon to be renamed to Dev_pp_run3_v1
  "doL1Sim=True;",
  "enableL1Phase1=True;",
  "doEmptyMenu=True;",
  "doWriteBS=False;",
  "doWriteRDOTrigger=True;",
  "forceEnableAllChains=True;",
  "failIfNoProxy=True;"
])
ex.args = '-c "{:s}"'.format(precommand)

test = Test.Test()
test.art_type = 'build'
test.exec_steps = [ex]
test.check_steps = CheckSteps.default_check_steps(test)
test.check_steps.remove(test.get_step("ZeroCounts"))

checkFile = test.get_step("CheckFile")
checkFile.input_file = 'RDO_TRIG.pool.root'

chaindump = test.get_step("ChainDump")
chaindump.args = "--json --yaml ref_data_v1Dev_rerunL1Only_phase1_build.new --yamlL1"
refcomp = CheckSteps.ChainCompStep("CountRefComp")
refcomp.input_file = 'ref_data_v1Dev_rerunL1Only_phase1_build.new'
refcomp.required = True # Final exit code depends on this step
CheckSteps.add_step_after_type(test.check_steps, CheckSteps.ChainDumpStep, refcomp)

import sys
sys.exit(test.run())
