#!/usr/bin/env python

# art-description: Trigger BS->RDO_TRIG athena test of the Cosmic_pp_run3_v1 menu on physics_Main stream from a cosmic run
# art-type: grid
# art-include: master/Athena
# art-output: *.txt
# art-output: *.log
# art-output: log.*
# art-output: *.new
# art-output: *.json
# art-output: *.root
# art-output: *.pmon.gz
# art-output: *perfmon*
# art-output: *.check*

from TrigValTools.TrigValSteering import Test, ExecStep, CheckSteps

ex = ExecStep.ExecStep()
ex.type = 'athena'
ex.job_options = 'TrigUpgradeTest/full_menu.py'
ex.input = 'data_cos'
ex.threads = 1
ex.args = '-c "setMenu=\'Cosmic_pp_run3_v1\';doWriteBS=False;doWriteRDOTrigger=True;"'

test = Test.Test()
test.art_type = 'grid'
test.exec_steps = [ex]
test.check_steps = CheckSteps.default_check_steps(test)

import sys
sys.exit(test.run())
