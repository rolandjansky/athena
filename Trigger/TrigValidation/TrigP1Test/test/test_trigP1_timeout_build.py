#!/usr/bin/env python
# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

# art-description: athenaHLT test of timeout handling
# art-type: build
# art-include: master/Athena
# art-include: 22.0/Athena
# Skipping art-output which has no effect for build tests.
# If you create a grid version, check art-output in existing grid tests.

from TrigValTools.TrigValSteering import Test, ExecStep, CheckSteps
from TrigP1Test.TrigP1TestSteps import TrigBSDumpGrepStep

output_name_base = 'output.test_trigP1_timeout'

ex = ExecStep.ExecStep()
ex.type = 'athenaHLT'
ex.job_options = 'TrigP1Test/testHLT_timeout.py'
ex.input = 'data'
ex.max_events = 20
ex.args = '--timeout 2000'
ex.args += ' -o ' + output_name_base

test = Test.Test()
test.art_type = 'build'
test.exec_steps = [ex]
test.check_steps = CheckSteps.default_check_steps(test)

# Use special check_log config to ignore timeout errors which are expected in this test
test.get_step('CheckLog').config_file = 'checklogTrigP1TimeoutTest.conf'

# Step checking if there is at least one event in HltTimeout debug stream
debug_count_step = TrigBSDumpGrepStep('DebugCount')
debug_count_step.args += '--stag'
debug_count_step.file_name_base = output_name_base
debug_count_step.regex = 'debug_HltTimeout'
debug_count_step.comparator = lambda num: num > 0
test.check_steps.append(debug_count_step)

# Step checking if number of output events is equal to number of input events
event_count_step = TrigBSDumpGrepStep('EventCount')
event_count_step.file_name_base = output_name_base
event_count_step.regex = r'Global_ID'
event_count_step.comparator = lambda num: num == ex.max_events
test.check_steps.append(event_count_step)

# Step checking if useful debug metadata is written for all events
meta_count_step = TrigBSDumpGrepStep('MetaCount')
meta_count_step.args += '--confKeys --runtimeMetadata'
meta_count_step.file_name_base = output_name_base
meta_count_step.regex = r'TrigConfKeys\|hostname'
meta_count_step.comparator = lambda num: num == 2*ex.max_events
test.check_steps.append(meta_count_step)

import sys
sys.exit(test.run())
