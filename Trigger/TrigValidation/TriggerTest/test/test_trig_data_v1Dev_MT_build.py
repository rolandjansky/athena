#!/usr/bin/env python

# art-description: Trigger BS->RDO_TRIG athena test of the Dev_pp_run3_v1 menu
# art-type: build
# art-include: master/Athena
# Skipping art-output which has no effect for build tests.
# If you create a grid version, check art-output in existing grid tests.

from TrigValTools.TrigValSteering import Test, ExecStep, CheckSteps

ex = ExecStep.ExecStep()
ex.type = 'athena'
ex.job_options = 'TriggerJobOpts/runHLT_standalone.py'
ex.input = 'data'
ex.threads = 2
ex.concurrent_events = 2
# LS2_v1 soon to be renamed to Dev_pp_run3_v1
ex.args = '-c "setMenu=\'LS2_v1\';doWriteBS=False;doWriteRDOTrigger=True;"'
ex.args += ' -c "fpeAuditor=True;"'
ex.args += ' -c "forceEnableAllChains=True;"'

test = Test.Test()
test.art_type = 'build'
test.exec_steps = [ex]
test.check_steps = CheckSteps.default_check_steps(test)

# Overwrite default RegTest settings
regtest = test.get_step('RegTest')
regtest.regex = 'TrigSignatureMoniMT.*HLT_.*|TrigSignatureMoniMT.*-- #[0-9]+ (Events|Features).*'
regtest.reference = 'TriggerTest/ref_data_v1Dev_build.ref'
regtest.required = True # Final exit code depends on this step

import sys
sys.exit(test.run())
