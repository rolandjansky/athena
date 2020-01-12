#!/usr/bin/env python

# art-description: Jet slice test, with athenaHLT, MP
# art-type: build
# art-include: master/Athena

from TrigValTools.TrigValSteering import Test, ExecStep, CheckSteps

ex = ExecStep.ExecStep()
ex.type = 'athenaHLT'
ex.job_options = 'TrigUpgradeTest/full_menu.py'
ex.input = 'data'
ex.forks = 2
ex.threads = 1
ex.concurrent_events = 1
ex.args = '--stdcmalloc -c "doEmptyMenu=True;doJetSlice=True;"'

test = Test.Test()
test.art_type = 'build'
test.exec_steps = [ex]
test.check_steps = CheckSteps.default_check_steps(test)

import sys
sys.exit(test.run())
