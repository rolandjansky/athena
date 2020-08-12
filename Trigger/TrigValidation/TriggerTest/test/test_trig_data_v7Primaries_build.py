#!/usr/bin/env python
# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# art-description: Legacy trigger test on data with v7 primaries menu
# art-type: build
# art-include: master/Athena
# Skipping art-output which has no effect for build tests.
# If you create a grid version, check art-output in existing grid tests.

from TrigValTools.TrigValSteering import Test, ExecStep, CheckSteps
from TrigValTools.TrigValSteering.Input import get_input

ex = ExecStep.ExecStep()
ex.type = 'athena'
ex.job_options = 'TriggerJobOpts/runHLT_standalone_run2.py'
ex.input = ''  # These JO expect BSRDOInput variable instead of normal athena input handling
ex.explicit_input = True
ex.args = '-c \'setMenu="Physics_pp_v7_primaries"; BSRDOInput=["{:s}"]\''.format(get_input('data').paths[0])

test = Test.Test()
test.art_type = 'build'
test.exec_steps = [ex]
test.check_steps = CheckSteps.default_check_steps(test)

import sys
sys.exit(test.run())
