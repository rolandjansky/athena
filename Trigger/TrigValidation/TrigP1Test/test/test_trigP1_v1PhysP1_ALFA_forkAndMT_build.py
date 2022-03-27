#!/usr/bin/env python
# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

# art-description: Test running on ALFACalib data with ALFA ROB Monitor chains, 2 forks, 3 slots, 6 threads
# art-type: build
# art-include: master/Athena

from TrigValTools.TrigValSteering import Test, ExecStep, CheckSteps

ex = ExecStep.ExecStep()
ex.type = 'athenaHLT'
ex.job_options = 'TriggerJobOpts/runHLT_standalone.py'
ex.input = 'data_ALFACalib'
ex.forks = 2
ex.threads = 6
ex.concurrent_events = 3
ex.max_events = 300
precommand = ''.join([
  "setMenu='PhysicsP1_pp_lowMu_run3_v1';",
  "doL1Sim=True;",
  "selectChains=['HLT_mb_alfaperf_L1RD0_FILLED','HLT_mb_alfaperf_L1RD0_EMPTY'];",
])
ex.args = '-c "{:s}"'.format(precommand)
ex.args += ' --dump-config-reload'

test = Test.Test()
test.art_type = 'build'
test.exec_steps = [ex]
test.check_steps = CheckSteps.default_check_steps(test)

# Skip ZeroCounts check because alfaperf chains reject all events by design
test.check_steps.remove(test.get_step("ZeroCounts"))

import sys
sys.exit(test.run())
