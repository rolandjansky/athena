#!/usr/bin/env python
# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# art-description: Trigger BS->RDO_TRIG athena test of the Dev_pp_run3_v1 menu
# art-type: build
# art-include: master/Athena
# Skipping art-output which has no effect for build tests.
# If you create a grid version, check art-output in existing grid tests.

from TrigValTools.TrigValSteering import Test, ExecStep, CheckSteps

ex = ExecStep.ExecStep()
ex.type = 'athena'
ex.job_options = 'TriggerJobOpts/runHLT_standalone.py'
ex.input = 'data'
ex.threads = 2
ex.concurrent_events = 2
precommand = ''.join([
  "setMenu='LS2_v1';",  # LS2_v1 soon to be renamed to Dev_pp_run3_v1
  "doWriteBS=False;",
  "doWriteRDOTrigger=True;",
  "forceEnableAllChains=True;",
  "fpeAuditor=True;",
])
ex.args = '-c "{:s}"'.format(precommand)

test = Test.Test()
test.art_type = 'build'
test.exec_steps = [ex]
test.check_steps = CheckSteps.default_check_steps(test)

# Add a step comparing counts in the log against reference from test_trig_data_v1Dev_build
refcomp = CheckSteps.RegTestStep("CountRefComp")
refcomp.regex = r'TrigSignatureMoniMT\s*INFO\sHLT_.*|TrigSignatureMoniMT\s*INFO\s-- #[0-9]+ (Events|Features).*'
refcomp.reference = 'TriggerTest/ref_data_v1Dev_build.ref'
refcomp.required = True # Final exit code depends on this step
CheckSteps.add_step_after_type(test.check_steps, CheckSteps.LogMergeStep, refcomp)

# Use RootComp reference from test_trig_data_v1Dev_build
test.get_step('RootComp').ref_test_name = 'trig_data_v1Dev_build'

import sys
sys.exit(test.run())
