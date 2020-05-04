#!/usr/bin/env python

# art-description: athenaMT trigger test using the full LS2_v1 menu from TriggerMenuMT
# art-type: build
# art-include: master/Athena
# Skipping art-output which has no effect for build tests.
# If you create a grid version, check art-output in existing grid tests.

from TrigValTools.TrigValSteering import Test, ExecStep, CheckSteps

ex = ExecStep.ExecStep()
ex.type = 'athena'
ex.job_options = 'TriggerJobOpts/runHLT_standalone.py'
ex.input = 'data'
#ex.args = '-c "isOnline=True;doWriteBS=False;doWriteRDOTrigger=True;"'
#ex.args = '-c "doEmptyMenu=True;doBjetSlice=True;isOnline=True;doWriteBS=False;doWriteRDOTrigger=True;"'
ex.args = '-c "doEmptyMenu=True;forceEnableAllChais=True;isOnline=True;doWriteBS=False;doWriteRDOTrigger=True;"'
ex.max_events = 10000

test = Test.Test()
test.art_type = 'build'
test.exec_steps = [ex]
test.check_steps = CheckSteps.default_check_steps(test)

import sys
sys.exit(test.run())
