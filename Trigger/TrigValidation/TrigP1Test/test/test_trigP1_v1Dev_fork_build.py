#!/usr/bin/env python
# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

# art-description: athenaHLT test of the Dev_pp_run3_v1 menu with multiple forks
# art-type: build
# art-include: master/Athena

from TrigValTools.TrigValSteering import Test, ExecStep, CheckSteps

ex = ExecStep.ExecStep()
ex.type = 'athenaHLT'
ex.job_options = 'TriggerJobOpts/runHLT_standalone.py'
ex.input = 'data'
ex.forks = 8
ex.threads = 1
ex.concurrent_events = 1
ex.args = '-c "setMenu=\'Dev_pp_run3_v1_TriggerValidation_prescale\';doL1Sim=True;"'  
ex.args += ' --dump-config-reload'

test = Test.Test()
test.art_type = 'build'
test.exec_steps = [ex]
test.check_steps = CheckSteps.default_check_steps(test)

import sys
sys.exit(test.run())
