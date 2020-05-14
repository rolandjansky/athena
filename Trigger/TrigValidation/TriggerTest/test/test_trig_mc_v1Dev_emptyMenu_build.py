#!/usr/bin/env python

# art-description: Trigger RDO->RDO_TRIG athena test with L1 simulation but without any HLT chains
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
ex.args = '-c "setMenu=\'LS2_v1\';doL1Sim=True;doEmptyMenu=True;doWriteBS=False;doWriteRDOTrigger=True;"'

test = Test.Test()
test.art_type = 'build'
test.exec_steps = [ex]
test.check_steps = CheckSteps.default_check_steps(test)

# Skip ZeroCounts check because empty menu has no counts
test.check_steps.remove(test.get_step("ZeroCounts"))

import sys
sys.exit(test.run())
