#!/usr/bin/env python
# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

# art-description: Test of P1+Tier0 workflow, runs athenaHLT with PhysicsP1_pp_run3_v1 menu followed by offline reco and monitoring
# art-type: build
# art-include: master/Athena

from TrigValTools.TrigValSteering import Test, ExecStep, CheckSteps
from TrigValTools.TrigValSteering.Common import find_file

# HLT step (BS->BS)
hlt = ExecStep.ExecStep()
hlt.type = 'athenaHLT'
hlt.job_options = 'TriggerJobOpts/runHLT_standalone.py'
hlt.forks = 1
hlt.threads = 4
hlt.concurrent_events = 4
hlt.input = 'data'
hlt.max_events = 50
hlt.args = '-c "setMenu=\'PhysicsP1_pp_run3_v1\';doL1Sim=True;rewriteLVL1=True;"'
hlt.args += ' -o output'

# Extract the physics_Main stream out of the BS file with many streams
filter_bs = ExecStep.ExecStep('FilterBS')
filter_bs.type = 'other'
filter_bs.executable = 'trigbs_extractStream.py'
filter_bs.input = ''
filter_bs.args = '-s Main ' + find_file('*_HLTMPPy_output.*.data')

# Tier-0 reco step (BS->ESD->AOD)
tzrecoPreExec = ' '.join([
  "from AthenaConfiguration.AllConfigFlags import ConfigFlags;",
  "ConfigFlags.Trigger.triggerMenuSetup=\'PhysicsP1_pp_run3_v1\';",
  "ConfigFlags.Trigger.AODEDMSet=\'AODFULL\';",
  "ConfigFlags.Trigger.enableL1MuonPhase1=True;",
  "ConfigFlags.Trigger.enableL1CaloPhase1=True;",
])

tzreco = ExecStep.ExecStep('Tier0Reco')
tzreco.type = 'Reco_tf'
tzreco.threads = 4
tzreco.concurrent_events = 4
tzreco.input = ''
tzreco.explicit_input = True
tzreco.max_events = 50
tzreco.args = '--inputBSFile=' + find_file('*.physics_Main*._athenaHLT*.data')  # output of the previous step
tzreco.args += ' --outputESDFile=ESD.pool.root --outputAODFile=AOD.pool.root'
tzreco.args += ' --conditionsTag=\'CONDBR2-BLKPA-RUN2-06\' --geometryVersion=\'ATLAS-R2-2016-01-00-01\''
tzreco.args += ' --preExec="{:s}"'.format(tzrecoPreExec)
tzreco.args += ' --postInclude="TriggerTest/disableChronoStatSvcPrintout.py"'

# Tier-0 monitoring step (AOD->HIST)
tzmon = ExecStep.ExecStep('Tier0Mon')
tzmon.type = 'other'
tzmon.executable = 'Run3DQTestingDriver.py'
tzmon.input = ''
tzmon.args = '--threads=1'
tzmon.args += ' --dqOffByDefault'
tzmon.args += ' Input.Files="[\'AOD.pool.root\']" DQ.Steering.doHLTMon=True Trigger.triggerMenuSetup=\'PhysicsP1_pp_run3_v1\''

# The full test
test = Test.Test()
test.art_type = 'build'
test.exec_steps = [hlt, filter_bs, tzreco, tzmon]
test.check_steps = CheckSteps.default_check_steps(test)

# Overwrite default histogram file name for checks
for step in [test.get_step(name) for name in ['HistCount', 'RootComp']]:
    step.input_file = 'ExampleMonitorOutput.root'

import sys
sys.exit(test.run())
