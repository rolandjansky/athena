#!/usr/bin/env python
# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# art-description: magnetic field toggle (on->off->on) test with PhysicsP1_pp_run3_v1 menu
# art-type: build
# art-include: master/Athena
# Skipping art-output which has no effect for build tests.
# If you create a grid version, check art-output in existing grid tests.

from TrigValTools.TrigValSteering import Test, ExecStep, CheckSteps

import os
# This file is needed by magFieldOnOff.trans
os.system('get_files -symlink -jo setMagFieldCurrents.py')

ex = ExecStep.ExecStep()
ex.type = 'athenaHLT'
ex.job_options = 'TriggerJobOpts/runHLT_standalone.py'
ex.input = 'data'
ex.args = '-i -M -ul -c "setMenu=\'PhysicsP1_pp_run3_v1\';BFieldAutoConfig=False;"'
ex.perfmon = False # perfmon currently not fully supported with athenaHLT -M

# Pass the transitions file into athenaHLT -i
ex.cmd_suffix = ' < `find_data.py magFieldOnOff.trans`'

test = Test.Test()
test.art_type = 'build'
test.exec_steps = [ex]
test.check_steps = CheckSteps.default_check_steps(test)

# Extra merging pattern for logs produced with -ul option
logmerge = test.get_step("LogMerge")
logmerge.extra_log_regex = 'athenaHLT-.*-.*(.out|.err)'

# Compare to reference to check field status
refcomp = CheckSteps.RegTestStep('RegTest')
refcomp.regex = '^AtlasFieldSvc.*Initializing the field map'
refcomp.reference = 'TrigP1Test/test_trigP1_v1PhysP1_magFieldToggle.ref'
refcomp.required = True              # Final exit code depends on this step
test.check_steps.insert(-1, refcomp) # Add before the last (zip) step

# Modify rootcomp to compare histograms between the two field ON runs
rc = test.get_step("RootComp")
rc.input_file = 'run_3.root'
rc.reference = 'run_1.root'
rc.explicit_reference = True  # Don't check if reference exists at configuration time
rc.required = True

import sys
sys.exit(test.run())
