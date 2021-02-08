#!/usr/bin/env python
# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# art-description: Trigger RDO->RDO_TRIG athena test with L1 simulation with Phase-I path enabled but without any HLT chains
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
  "setMenu='LS2_v1';",  # LS2_v1 soon to be renamed to Dev_pp_run3_v1
  "doL1Sim=True;",
  "enableL1Phase1=True;",
  "enableL1CaloLegacy=True;",
  "doEmptyMenu=True;",
  "doWriteBS=False;",
  "doWriteRDOTrigger=True;"
])
ex.args = '-c "{:s}"'.format(precommand)

test = Test.Test()
test.art_type = 'build'
test.exec_steps = [ex]
test.check_steps = CheckSteps.default_check_steps(test)

# Skip ZeroCounts check because empty menu has no counts
test.check_steps.remove(test.get_step("ZeroCounts"))

chaindump = test.get_step("ChainDump")
chaindump.args = '--json --yaml ref_mc_v1Dev_L1SimOnly_phase1_build.new --yamlL1'

refcomp = CheckSteps.ChainCompStep("CountRefComp")
refcomp.input_file = 'ref_mc_v1Dev_L1SimOnly_phase1_build.new'
CheckSteps.add_step_after_type(test.check_steps, CheckSteps.ChainDumpStep, refcomp)

import sys
sys.exit(test.run())
