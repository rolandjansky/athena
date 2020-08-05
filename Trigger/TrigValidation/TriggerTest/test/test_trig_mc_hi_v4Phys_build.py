#!/usr/bin/env python
# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# art-description: Legacy trigger test on MC with Physics_HI_v4 menu
# art-type: build
# art-include: master/Athena
# Skipping art-output which has no effect for build tests.
# If you create a grid version, check art-output in existing grid tests.

from TrigValTools.TrigValSteering import Test, ExecStep, CheckSteps

ex = ExecStep.ExecStep()
ex.type = 'athena'
ex.job_options = 'TriggerTest/testCommonSliceAthenaTrigRDO.py'
ex.input = 'ttbar_rel21'
ex.args = '-c \'setMenu="Physics_HI_v4";\''

test = Test.Test()
test.art_type = 'build'
test.exec_steps = [ex]
test.check_steps = CheckSteps.default_check_steps(test)

import sys
sys.exit(test.run())
