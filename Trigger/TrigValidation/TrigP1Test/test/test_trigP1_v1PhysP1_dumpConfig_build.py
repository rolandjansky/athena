#!/usr/bin/env python
# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# art-description: PhysicsP1_pp_run3_v1 menu test only dumping options for SMK generation
# art-type: build
# art-include: master/Athena

from TrigValTools.TrigValSteering import Test, ExecStep, CheckSteps

ex = ExecStep.ExecStep()
ex.type = 'athenaHLT'
ex.job_options = 'TriggerJobOpts/runHLT_standalone.py'
ex.input = 'data'
ex.args = '-c "setMenu=\'PhysicsP1_pp_run3_v1\';"'
ex.args += ' -M --dump-config-exit'
ex.perfmon = False  # Cannot use PerfMon with -M

test = Test.Test()
test.art_type = 'build'
test.exec_steps = [ex]
# Only keep relevant checks from the defaults
test.check_steps = [chk for chk in CheckSteps.default_check_steps(test)
                    if type(chk) in (CheckSteps.LogMergeStep, CheckSteps.CheckLogStep)]

import sys
sys.exit(test.run())
