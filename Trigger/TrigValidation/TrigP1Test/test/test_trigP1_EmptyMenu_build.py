#!/usr/bin/env python

# art-description: Same as empty_menu test from TrigUpgradeTest, but with athenaHLT
# art-type: build                                                                  
# art-include: master/Athena                                                       

from TrigValTools.TrigValSteering import Test, ExecStep, CheckSteps

ex = ExecStep.ExecStep()
ex.type = 'athenaHLT'
ex.job_options = 'TrigUpgradeTest/full_menu.py'
ex.input = 'data'
ex.args = '-c "doEmptyMenu=True;BFieldAutoConfig=False;doWriteESD=False"'
ex.perfmon = False # perfmon with athenaHLT doesn't work at the moment

test = Test.Test()
test.art_type = 'build'
test.exec_steps = [ex]
test.check_steps = CheckSteps.default_check_steps(test)

import sys
sys.exit(test.run())
