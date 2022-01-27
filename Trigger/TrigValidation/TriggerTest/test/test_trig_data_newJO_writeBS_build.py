#!/usr/bin/env python
# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

# art-description: Trigger AthenaMT test running new-style job options
# art-type: build
# art-include: master/Athena
# Skipping art-output which has no effect for build tests.
# If you create a grid version, check art-output in existing grid tests.

from TrigValTools.TrigValSteering import Test, Step, ExecStep, CheckSteps, Input

# Copy the job options to the working directory
copy_jo = ExecStep.ExecStep('CopyJO')
copy_jo.type = 'other'
copy_jo.input = ''
copy_jo.executable = 'get_files'
copy_jo.args = '-remove -jo TriggerJobOpts/runHLT_standalone_newJO.py'
copy_jo.prmon = False
copy_jo.auto_report_result = False  # Do not set art-result for this step
copy_jo.output_stream = Step.Step.OutputStream.STDOUT_ONLY  # Do not create a log file for this step

# Generate configuration run file
run = ExecStep.ExecStep('athena_run')
run.type = 'other'
run.input = 'data'
run.executable = 'python'
run.args = 'runHLT_standalone_newJO.py'
run.args += ' --filesInput='+Input.get_input('data').paths[0]
run.args += ' Trigger.triggerMenuSetup="Dev_pp_run3_v1"'
run.args += ' Trigger.doRuntimeNaviVal=True'
run.args += ' Output.doWriteRDO=False  Output.doWriteBS=True Trigger.writeBS=True'
run.prmon = False


# The full test configuration
test = Test.Test()
test.art_type = 'build'
test.exec_steps = [copy_jo, run]
test.check_steps = CheckSteps.default_check_steps(test)
test.get_step('MessageCount').log_regex = 'athena_run.log'


import sys
sys.exit(test.run())
