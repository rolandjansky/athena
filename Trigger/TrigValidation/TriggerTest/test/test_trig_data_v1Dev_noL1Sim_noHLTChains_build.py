#!/usr/bin/env python
# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# art-description: Trigger BS->RDO_TRIG athena test without any HLT chains and without L1Sim (only tests L1 decoding)
# art-type: build
# art-include: master/Athena
# Skipping art-output which has no effect for build tests.
# If you create a grid version, check art-output in existing grid tests.

from TrigValTools.TrigValSteering import Test, ExecStep, CheckSteps

ex = ExecStep.ExecStep()
ex.type = 'athena'
ex.job_options = 'TriggerJobOpts/runHLT_standalone.py'
ex.input = 'data'
ex.max_events = 80
ex.threads = 4
ex.concurrent_events = 4
precommand = ''.join([
  "setMenu='Dev_pp_run3_v1';",  
  "doL1Sim=False;",
  "doEmptyMenu=True;",
  "doWriteBS=False;",
  "doWriteRDOTrigger=True;",
  "forceEnableAllChains=True;",
])
ex.args = '-c "{:s}"'.format(precommand)

test = Test.Test()
test.art_type = 'build'
test.exec_steps = [ex]
test.check_steps = CheckSteps.default_check_steps(test)

# Skip ZeroCounts check because empty menu has no counts
test.check_steps.remove(test.get_step("ZeroCounts"))

# Overwrite default MessageCount settings
msgcount = test.get_step("MessageCount")
msgcount.thresholds = {
  'INFO': 400,
  'other': 10
}
msgcount.required = True # make the test exit code depend on this step

import sys
sys.exit(test.run())
