#!/usr/bin/env python
# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# art-description: CalibPeb test where chains are executed in parallel doing concurrent ROB requests
# art-type: grid
# art-include: master/Athena
# art-output: *.txt
# art-output: *.log
# art-output: log.*
# art-output: *.out
# art-output: *.err
# art-output: *.log.tar.gz
# art-output: *.new
# art-output: *.json
# art-output: *.root
# art-output: *.pmon.gz
# art-output: *perfmon*
# art-output: prmon*
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

test = Test.Test()
test.art_type = 'grid'
test.exec_steps = [ex]
test.check_steps = CheckSteps.default_check_steps(test)

# Make RootComp step required
rc = test.get_step('RootComp')
rc.required = True
rc.args += ' --sortLabels'

import sys
sys.exit(test.run())
