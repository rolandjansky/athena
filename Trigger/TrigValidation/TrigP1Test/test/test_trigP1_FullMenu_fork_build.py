#!/usr/bin/env python

# art-description: Same as full_menu test from TrigUpgradeTest, but with athenaHLT, MP
# art-type: build
# art-include: master/Athena

from TrigValTools.TrigValSteering import Test, ExecStep, CheckSteps

ex = ExecStep.ExecStep()
ex.type = 'athenaHLT'
ex.job_options = 'TrigUpgradeTest/full_menu.py'
ex.input = 'data'
ex.forks = 8
ex.threads = 1
ex.concurrent_events = 1
ex.args = '-c "doWriteESD=False"'
ex.perfmon = False # perfmon with athenaHLT doesn't work at the moment

test = Test.Test()
test.art_type = 'build'
test.exec_steps = [ex]
test.check_steps = CheckSteps.default_check_steps(test)

import sys
sys.exit(test.run())
