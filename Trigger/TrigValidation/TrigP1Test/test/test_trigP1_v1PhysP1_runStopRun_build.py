#!/usr/bin/env python
# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# art-description: start/stop/start transition test with PhysicsP1_pp_run3_v1 menu
# art-type: build
# art-include: master/Athena
# Skipping art-output which has no effect for build tests.
# If you create a grid version, check art-output in existing grid tests.

from TrigValTools.TrigValSteering import Test, ExecStep
from TrigP1Test import TrigP1TestSteps

ex = ExecStep.ExecStep()
ex.type = 'athenaHLT'
ex.job_options = 'TriggerJobOpts/runHLT_standalone.py'
ex.input = 'data'
ex.args = '-i -M -ul -c "setMenu=\'PhysicsP1_pp_run3_v1\';enableCostMonitoring=False;"'
ex.perfmon = False # perfmon currently not fully supported with athenaHLT -M

# Pass the transitions file into athenaHLT -i
ex.cmd_suffix = ' < `find_data.py run-stop-run-saveHist.trans`'

test = Test.Test()
test.art_type = 'build'
test.exec_steps = [ex]
test.check_steps = TrigP1TestSteps.default_check_steps_OHMon(test, 'run_2.root')

# Extra merging pattern for logs produced with -ul option
logmerge = test.get_step("LogMerge")
logmerge.extra_log_regex = 'athenaHLT-.*-.*(.out|.err)'

# Change RootComp step to compare histograms between the two runs
rc = test.get_step("RootComp")
rc.input_file = 'run_2.root'
rc.reference = 'run_1.root'
rc.explicit_reference = True  # Don't check if reference exists at configuration time
rc.required = True  # Final exit code depends on this step

import sys
sys.exit(test.run())
