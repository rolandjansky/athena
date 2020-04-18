#!/usr/bin/env python

# art-description: Trigger AthenaMT test running new-style job options
# art-type: build
# art-include: master/Athena
# Skipping art-output which has no effect for build tests.
# If you create a grid version, check art-output in existing grid tests.

from TrigValTools.TrigValSteering import Test, Step, ExecStep #, CheckSteps

# Copy the job options to the working directory
copy_jo = ExecStep.ExecStep('CopyJO')
copy_jo.type = 'other'
copy_jo.input = ''
copy_jo.executable = 'get_files'
copy_jo.args = '-remove -jo TrigUpgradeTest/newJOtest.py'
copy_jo.prmon = False
copy_jo.auto_report_result = False  # Do not set art-result for this step
copy_jo.output_stream = Step.Step.OutputStream.STDOUT_ONLY  # Do not create a log file for this step

# Generate configuration pickle file
pickle = ExecStep.ExecStep('GeneratePickle')
pickle.type = 'other'
pickle.input = ''
pickle.executable = 'python'
pickle.args = 'newJOtest.py'
pickle.prmon = False

# The main job running athena from pickle
run_athena = ExecStep.ExecStep('newJO')
run_athena.type = 'athena'
run_athena.use_pickle = True
run_athena.job_options = 'newJOtest.pkl'
run_athena.input = ''

# The full test configuration
test = Test.Test()
test.art_type = 'build'
#test.exec_steps = [copy_jo, pickle, run_athena]
#test.check_steps = CheckSteps.default_check_steps(test)
test.exec_steps = [copy_jo]
test.check_steps = [] 

# Change RegTest pattern
#regtest = test.get_step('RegTest')
#regtest.regex = 'TrigSignatureMoniMT.*HLT_.*|TrigSignatureMoniMT.*-- #[0-9]+ (Events|Features).*'

import sys
sys.exit(test.run())
