#!/usr/bin/env python
# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# art-description: Test with SGInputLoader.FailIfNoProxy=True to check for unmet data dependencies
# art-type: build
# art-include: master/Athena
# Skipping art-output which has no effect for build tests.
# If you create a grid version, check art-output in existing grid tests.

from TrigValTools.TrigValSteering import Test, ExecStep, CheckSteps

ex = ExecStep.ExecStep()
ex.type = 'athena'
ex.job_options = 'TriggerJobOpts/runHLT_standalone.py'
ex.input = 'ttbar'
ex.threads = 1
precommand = ''.join([
  "setMenu='LS2_v1_TriggerValidation_mc_prescale';",  # LS2_v1 soon to be renamed to Dev_pp_run3_v1
  "doWriteBS=False;",
  "doWriteRDOTrigger=True;",
  "fpeAuditor=True;",
  "failIfNoProxy=True;"
])
ex.args = '-c "{:s}"'.format(precommand)

test = Test.Test()
test.art_type = 'build'
test.exec_steps = [ex]
test.check_steps = CheckSteps.default_check_steps(test)
import sys
sys.exit(test.run())
