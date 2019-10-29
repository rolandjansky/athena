#!/usr/bin/env python

# art-description: CalibPeb test where chains are executed in parallel doing concurrent ROB requests
# art-type: grid
# art-include: master/Athena
# art-output: *.txt
# art-output: *.log
# art-output: log.*
# art-output: *.new
# art-output: *.json
# art-output: *.root
# art-output: *.pmon.gz
# art-output: *perfmon*
# art-output: *.check*
# art-athena-mt: 12

from TrigValTools.TrigValSteering import Test, ExecStep, CheckSteps

ex = ExecStep.ExecStep()
ex.type = 'athenaHLT'
ex.job_options = 'TrigExPartialEB/MTCalibPeb.py'
ex.input = 'data'
ex.forks = 2
ex.threads = 6
ex.concurrent_events = 2 # three threads per event
ex.args = '-c \'concurrent=True\''
ex.perfmon = False # perfmon with athenaHLT doesn't work at the moment

test = Test.Test()
test.art_type = 'grid'
test.exec_steps = [ex]
test.check_steps = CheckSteps.default_check_steps(test)

import sys
sys.exit(test.run())
