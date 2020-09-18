#!/usr/bin/env python
# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# art-description: magnetic field toggle (on->off->on) test with dummy job options
# art-type: build
# art-include: master/Athena
# Skipping art-output which has no effect for build tests.
# If you create a grid version, check art-output in existing grid tests.

# This test simulates magnetic field switches between runs, i.e. between stop/start
# transitions like it would happen at P1. In order to be able to increment the run
# number we employ a rather nasty trick:
#   1) the first run is done on the regular input file symlinked to raw.data
#   2) after stop, we switch the symlink to a copy of the file with the run number incremented
#   3) the same happens for the third run
#
# The files are prepared here, and the symlinking happens in magFieldOnOff.trans.

from TrigValTools.TrigValSteering import Test, Step, ExecStep, CheckSteps
from TrigValTools.TrigValSteering.Input import get_input

import os
import eformat

# This file is needed by magFieldOnOff.trans
os.system('get_files -symlink -remove -jo setMagFieldCurrents.py')

# Create a symlink to the first input file
input_file = get_input('data').paths[0]
run_no = eformat.istream(str(input_file))[0].run_no()  # `str` needed only for Python2
try:
   os.remove('raw.data')
except Exception:
   pass
os.symlink(input_file, 'raw.data')

# Delete any previous bytestream file
ex_rm = ExecStep.ExecStep('cleanup')
ex_rm.type = 'other'
ex_rm.input = ''
ex_rm.executable = 'rm'
ex_rm.args = '-f magField*.data'
ex_rm.auto_report_result = False  # Do not set art-result for this step
ex_rm.output_stream = Step.Step.OutputStream.STDOUT_ONLY  # Do not create a log file for this step

# Make two copies of input BS file with incremented run numbers
ex_bs1 = ExecStep.ExecStep('create_bs1')
ex_bs2 = ExecStep.ExecStep('create_bs2')
ex_bs1.type = ex_bs2.type = 'other'
ex_bs1.input = ex_bs2.input = ''
ex_bs1.executable = ex_bs2.executable = 'trigbs_modifyEvent.py'
ex_bs1.args = '-r %d -n 20 -o magField1 %s' % (run_no+1, input_file)
ex_bs2.args = '-r %d -n 20 -o magField2 %s' % (run_no+2, input_file)

ex = ExecStep.ExecStep()
ex.type = 'athenaHLT'
ex.job_options = 'TrigP1Test/testHLT_magField.py'
ex.input = ''
ex.max_events = 20
ex.args = '-f raw.data -i -M -ul'
ex.perfmon = False # perfmon currently not fully supported with athenaHLT -M

# Pass the transitions file into athenaHLT -i
ex.cmd_suffix = ' < `find_data.py magFieldOnOff.trans`'

test = Test.Test()
test.art_type = 'build'
test.exec_steps = [ex_rm, ex_bs1, ex_bs2, ex]
test.check_steps = CheckSteps.default_check_steps(test)

# Extra merging pattern for logs produced with -ul option
logmerge = test.get_step("LogMerge")
logmerge.extra_log_regex = 'athenaHLT-.*-.*(.out|.err)'

# Compare to reference to check field status
refcomp = CheckSteps.RegTestStep('RegTest')
refcomp.regex = '^AtlasFieldCacheCondAlg.*INFO.*|^MagFieldCondReader.*INFO.*'
refcomp.reference = 'TrigP1Test/test_trigP1_magFieldToggle.ref'
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
