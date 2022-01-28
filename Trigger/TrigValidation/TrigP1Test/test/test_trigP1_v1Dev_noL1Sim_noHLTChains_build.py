#!/usr/bin/env python
# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

# art-description: Test running only L1 result decoding
# art-type: build                                                                  
# art-include: master/Athena                                                       

from TrigValTools.TrigValSteering import Test, ExecStep, CheckSteps

ex = ExecStep.ExecStep()
ex.type = 'athenaHLT'
ex.job_options = 'TriggerJobOpts/runHLT_standalone.py'
ex.input = 'data'
ex.args = '-c "setMenu=\'Dev_pp_run3_v1\';doL1Sim=False;doEmptyMenu=True;forceEnableAllChains=True;BFieldAutoConfig=False;"'
ex.args += ' --dump-config-reload'

test = Test.Test()
test.art_type = 'build'
test.exec_steps = [ex]
test.check_steps = CheckSteps.default_check_steps(test)

# Skip ZeroCounts check because empty menu has no counts
test.check_steps.remove(test.get_step("ZeroCounts"))

import sys
sys.exit(test.run())
