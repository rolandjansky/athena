#!/usr/bin/env python
# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# art-description: Events processed by hardcoded chains with data requests, accepted randomly and sent to Full Event build, PEB and DS streams in athenaHLT
# art-type: build
# art-include: master/Athena
# Skipping art-output which has no effect for build tests.
# If you create a grid version, check art-output in existing grid tests.

from TrigValTools.TrigValSteering import Test, ExecStep, CheckSteps

ex = ExecStep.ExecStep()
ex.type = 'athenaHLT'
ex.job_options = 'TrigExPartialEB/MTCalibPeb.py'
ex.input = 'data'

test = Test.Test()
test.art_type = 'build'
test.exec_steps = [ex]
test.check_steps = CheckSteps.default_check_steps(test)

# Make RootComp step required
rc = test.get_step('RootComp')
rc.required = True
rc.args += ' --sortLabels'

import sys
sys.exit(test.run())
