#!/usr/bin/env python
# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

# art-description: Trigger AthenaMT test running new-style job options
# art-type: build
# art-include: master/Athena
# Skipping art-output which has no effect for build tests.
# If you create a grid version, check art-output in existing grid tests.

from TrigValTools.TrigValSteering import Test, ExecStep, CheckSteps, Input

# Generate configuration run file
run = ExecStep.ExecStep('athena')
run.type = 'other'
run.input = 'ttbar'
run.executable = 'python'
run.args = 'runHLT_standalone_newJO.py'
run.args += ' --filesInput='+Input.get_input(run.input).paths[0]
run.args += ' Trigger.triggerMenuSetup="Dev_pp_run3_v1"'
run.args += ' Trigger.doRuntimeNaviVal=True'
run.args += ' Output.doWriteRDO=False' #TODO enable once fixes issue with missing containers
run.prmon = False


# The full test configuration
test = Test.Test()
test.art_type = 'build'
#test.check_steps = CheckSteps.default_check_steps(test)
test.exec_steps = [run]
check_log = CheckSteps.CheckLogStep('CheckLog')
check_log.log_file = run.get_log_file_name()
test.check_steps = [check_log]

# Change RegTest pattern
#regtest = test.get_step('RegTest')
#regtest.regex = r'TrigSignatureMoniMT\s*INFO\sHLT_.*|TrigSignatureMoniMT\s*INFO\s-- #[0-9]+ (Events|Features).*'

import sys
sys.exit(test.run())
