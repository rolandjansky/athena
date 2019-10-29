#!/usr/bin/env python

# art-description: athenaMT trigger test using the full LS2_v1 menu from TriggerMenuMT
# art-type: build
# art-include: master/Athena
# Skipping art-output which has no effect for build tests.
# If you create a grid version, check art-output in existing grid tests.

from TrigValTools.TrigValSteering import Test, ExecStep, CheckSteps

ex = ExecStep.ExecStep()
ex.type = 'athena'
ex.job_options = 'TrigUpgradeTest/full_menu.py'
ex.input = 'data'
ex.args = '-c "isOnline=True;doWriteBS=False;doWriteRDOTrigger=True;"'
ex.skip_events = 10

test = Test.Test()
test.art_type = 'build'
test.exec_steps = [ex]
test.check_steps = CheckSteps.default_check_steps(test)

# Overwrite default RegTest settings
regtest = test.get_step('RegTest')
regtest.regex = 'TriggerSummaryStep.*HLT_.*|TriggerMonitorFinal.*HLT_.*|TrigSignatureMoniMT.*HLT_.*'
regtest.reference = 'TrigUpgradeTest/full_menu_build.ref'
regtest.required = True # Final exit code depends on this step

import sys
sys.exit(test.run())
