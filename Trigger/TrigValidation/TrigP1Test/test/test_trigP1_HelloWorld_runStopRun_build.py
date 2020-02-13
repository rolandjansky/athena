#!/usr/bin/env python

# art-description: HelloWorld test with start/stop/start transition
# art-type: build
# art-include: master/Athena
# Skipping art-output which has no effect for build tests.
# If you create a grid version, check art-output in existing grid tests.

from TrigValTools.TrigValSteering import Test, ExecStep, CheckSteps

ex = ExecStep.ExecStep()
ex.type = 'athenaHLT'
ex.job_options = 'AthExHelloWorld/HelloWorldOptions.py'
ex.input = 'data'
ex.args = '-i -M -ul'
ex.perfmon = False # perfmon currently not fully supported with athenaHLT -M

# Trick to pass the transitions file into athenaHLT -i
ex.job_options += ' < `find_data.py run-stop-run-saveHist.trans`'

test = Test.Test()
test.art_type = 'build'
test.exec_steps = [ex]
test.check_steps = CheckSteps.default_check_steps(test)

# Extra merging pattern for logs produced with -ul option
logmerge = test.get_step("LogMerge")
logmerge.extra_log_regex = 'athenaHLT-.*-.*(.out|.err)'

import sys
sys.exit(test.run())
