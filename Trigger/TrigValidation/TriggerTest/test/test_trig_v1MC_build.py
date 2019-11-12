#!/usr/bin/env python

# art-description: Trigger RDO->RDO_TRIG athena test of the MC_pp_run3_v1 menu
# art-type: build
# art-include: master/Athena
# Skipping art-output which has no effect for build tests.
# If you create a grid version, check art-output in existing grid tests.

from TrigValTools.TrigValSteering import Test, ExecStep, CheckSteps

ex = ExecStep.ExecStep()
ex.type = 'athena'
ex.job_options = 'TrigUpgradeTest/full_menu.py'
ex.input = 'ttbar'
ex.threads = 1
ex.args = '-c "setMenu=\'MC_pp_run3_v1\';doWriteBS=False;doWriteRDOTrigger=True;"'

test = Test.Test()
test.art_type = 'build'
test.exec_steps = [ex]
test.check_steps = CheckSteps.default_check_steps(test)

import sys
sys.exit(test.run())
