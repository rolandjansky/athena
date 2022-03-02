#!/usr/bin/env python
# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

# art-description: athenaHLT test of the Dev_pp_run3_v1 menu, with forks=2, threads=4, concurrent_events=4
# art-type: build                                                                  
# art-include: master/Athena                                                       

from TrigValTools.TrigValSteering import Test, ExecStep, CheckSteps

ex = ExecStep.ExecStep()
ex.type = 'athenaHLT'
ex.job_options = 'TriggerJobOpts/runHLT_standalone.py'
ex.input = 'data'
ex.max_events = 50
ex.forks = 2
ex.threads = 4
ex.concurrent_events = 4
precommand = ''.join([
  "setMenu='Dev_pp_run3_v1_TriggerValidation_prescale';",
  "doL1Sim=True;",
  "rewriteLVL1=True;",
  "doRuntimeNaviVal=True;",
])
ex.args = '-c "{:s}"'.format(precommand)
ex.args += ' --dump-config-reload'

test = Test.Test()
test.art_type = 'build'
test.exec_steps = [ex]
test.check_steps = CheckSteps.default_check_steps(test)

# Add a step comparing counts against a reference from test_trigP1_v1Dev_decodeBS_build
chaindump = test.get_step("ChainDump")
chaindump.args = '--json --yaml ref_v1Dev_decodeBS_build.new'
refcomp = CheckSteps.ChainCompStep("CountRefComp")
refcomp.input_file = 'ref_v1Dev_decodeBS_build.new'
refcomp.reference_from_release = True # installed from TrigP1Test/share
refcomp.required = True # Final exit code depends on this step
CheckSteps.add_step_after_type(test.check_steps, CheckSteps.ChainDumpStep, refcomp)

# Use RootComp reference from test_trigP1_v1Dev_decodeBS_build
test.get_step('RootComp').ref_test_name = 'trigP1_v1Dev_decodeBS_build'

import sys
sys.exit(test.run())
