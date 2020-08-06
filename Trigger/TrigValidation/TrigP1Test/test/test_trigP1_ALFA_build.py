#!/usr/bin/env python
# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# art-description: Test running on ALFACalib data with ALFA ROB Monitor enabled
# art-type: build
# art-include: master/Athena

from TrigValTools.TrigValSteering import Test, ExecStep, CheckSteps

ex = ExecStep.ExecStep()
ex.type = 'athenaHLT'
ex.job_options = 'TriggerJobOpts/runHLT_standalone.py'
ex.input = 'data_ALFACalib'
ex.args = '-c "enableALFAMon=True;"'
# This test should ultimately run just ALFA chains, but until this is available we use full PhysicsP1 menu
ex.args += ' -c "setMenu=\'PhysicsP1_pp_run3_v1\';"'

test = Test.Test()
test.art_type = 'build'
test.exec_steps = [ex]
test.check_steps = CheckSteps.default_check_steps(test)

import sys
sys.exit(test.run())
