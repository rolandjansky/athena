#!/usr/bin/env python

# art-description: HelloWorld in athenaHLT with OH monitoring
# art-type: build
# art-include: master/Athena
# Skipping art-output which has no effect for build tests.
# If you create a grid version, check art-output in existing grid tests.

from TrigValTools.TrigValSteering import Test, ExecStep, CheckSteps

ex = ExecStep.ExecStep()
ex.type = 'athenaHLT'
ex.job_options = 'AthExHelloWorld/HelloWorldOptions.py'
ex.input = 'data'
ex.args = '-M'
ex.perfmon = False # perfmon with athenaHLT doesn't work at the moment

test = Test.Test()
test.art_type = 'build'
test.exec_steps = [ex]
test.check_steps = CheckSteps.default_check_steps(test)

# Overwrite default histogram file name for checks
for step in [test.get_step(name) for name in ['HistCount', 'RootComp', 'ChainDump']]:
  step.input_file = 'r0000327265_athenaHLT_HLT-Histogramming.root'

import sys
sys.exit(test.run())
