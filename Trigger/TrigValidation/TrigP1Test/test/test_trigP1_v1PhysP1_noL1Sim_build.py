#!/usr/bin/env python
# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

# art-description: athenaHLT test of the PhysicsP1_pp_run3_v1 menu
# art-type: build                                                                  
# art-include: master/Athena
# art-include: 22.0/Athena                                                       

from TrigValTools.TrigValSteering import Test, ExecStep, CheckSteps

ex = ExecStep.ExecStep()
ex.type = 'athenaHLT'
ex.job_options = 'TriggerJobOpts/runHLT_standalone.py'
ex.input = 'data'
precommand = ''.join([
  "setMenu='PhysicsP1_pp_run3_v1_HLTReprocessing_prescale';",
  "doL1Sim=False;",
  "enableL1MuonPhase1=False;",
  "forceEnableAllChains=True;",
  "disableChains=['HLT_cosmic_id_L1MU3V_EMPTY','HLT_cosmic_id_L1MU8VF_EMPTY']", # Temporary workaround for ATR-25459
])
ex.args = f'-c "{precommand}"' 
ex.args += ' --dump-config-reload'

test = Test.Test()
test.art_type = 'build'
test.exec_steps = [ex]
test.check_steps = CheckSteps.default_check_steps(test)

import sys
sys.exit(test.run())
