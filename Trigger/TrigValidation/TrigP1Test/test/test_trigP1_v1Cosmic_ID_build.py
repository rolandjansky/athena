#!/usr/bin/env python

# art-description: Trigger athenaHLT test of the Cosmic_pp_run3_v1 menu on physics_IDCosmic stream from a cosmic run
# art-type: build
# art-include: master/Athena

from TrigValTools.TrigValSteering import Test, ExecStep, CheckSteps

ex = ExecStep.ExecStep()
ex.type = 'athenaHLT'
ex.job_options = 'TriggerJobOpts/runHLT_standalone.py'
ex.input = 'data_cos_IDCosmic'
ex.max_events = 100
ex.args = '-c "setMenu=\'Cosmic_pp_run3_v1\';doCosmics=True;"'

test = Test.Test()
test.art_type = 'build'
test.exec_steps = [ex]
test.check_steps = CheckSteps.default_check_steps(test)

import sys
sys.exit(test.run())
