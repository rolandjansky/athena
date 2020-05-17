#!/usr/bin/env python

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
ex.threads = 1
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

# Overwrite default MessageCount settings
# We are trying to lower the limits step by step
# Ultimately there should be no per-event messages
msgcount = test.get_step("MessageCount")
msgcount.warning_threshold = 500
msgcount.info_threshold = 1200
msgcount.other_threshold = 80
msgcount.required = True # make the test exit code depend on this step

# Add a step comparing counts in the log against reference
refcomp = CheckSteps.RegTestStep("CountRefComp")
refcomp.regex = 'TrigSignatureMoniMT.*HLT_.*|TrigSignatureMoniMT.*-- #[0-9]+ (Events|Features).*'
refcomp.reference = 'TriggerTest/ref_data_v1Dev_build.ref'
refcomp.required = True # Final exit code depends on this step
CheckSteps.add_step_after_type(test.check_steps, CheckSteps.LogMergeStep, refcomp)

import sys
sys.exit(test.run())
