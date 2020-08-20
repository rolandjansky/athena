#!/usr/bin/env python
# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# art-description: transform test of BSRDOtoRAW + T0Reco + T0Mon, using v1PhysP1 menu
# art-type: build
# art-include: master/Athena

from TrigValTools.TrigValSteering import Test, ExecStep, CheckSteps

#====================================================================================================
# HLT BS_RDO->RAW
hlt = ExecStep.ExecStep('BSRDOtoRAW')
hlt.type = 'Trig_reco_tf'
hlt.forks = 1
hlt.threads = 1
hlt.concurrent_events = 1
hlt.max_events = 50
hlt.args = '--precommand=\\\"setMenu=\\\'PhysicsP1_pp_run3_v1\\\'\\\"'
hlt.args += ' --prodSysBSRDO True'
hlt.args += ' --outputBSFile=RAW.pool.root'
hlt.args += ' --outputHIST_HLTMONFile=hltmon.root'
hlt.input = 'data'

#====================================================================================================
# Tier-0 reco step BS->ESD->AOD
tzrecoPreExec = ' '.join([
 "from AthenaConfiguration.AllConfigFlags import ConfigFlags;",
 "ConfigFlags.Trigger.triggerMenuSetup=\'PhysicsP1_pp_run3_v1\';",
 "from TriggerJobOpts.TriggerFlags import TriggerFlags;",
 "TriggerFlags.configForStartup=\'HLToffline\';",
 "TriggerFlags.inputHLTconfigFile.set_Value_and_Lock(\'NONE\');",
 "TriggerFlags.AODEDMSet.set_Value_and_Lock(\'AODFULL\')",
])

tzreco = ExecStep.ExecStep('Tier0Reco')
tzreco.type = 'Trig_reco_tf'
tzreco.threads = 1
tzreco.explicit_input = True
tzreco.input = ''
tzreco.max_events = 50
tzreco.args = '--inputBSFile=RAW.pool.root'  # output of the previous step
tzreco.args += ' --outputESDFile=ESD.pool.root --outputAODFile=AOD.pool.root'
tzreco.args += ' --conditionsTag=\'CONDBR2-BLKPA-2018-11\' --geometryVersion=\'ATLAS-R2-2016-01-00-01\''
tzreco.args += ' --preExec="{:s}"'.format(tzrecoPreExec)
tzreco.args += ' --postInclude="TriggerTest/disableChronoStatSvcPrintout.py"'

#====================================================================================================
# Tier-0 monitoring step (AOD->HIST)                                                         
tzmon = ExecStep.ExecStep('Tier0Mon')                                                        
tzmon.type = 'other'
tzmon.executable = 'Run3DQTestingDriver.py'                                                  
tzmon.input = ''                                                                             
tzmon.args = '--dqOffByDefault Input.Files="[\'AOD.pool.root\']" DQ.Steering.doHLTMon=True'  

#====================================================================================================
# The full test
test = Test.Test()
test.art_type = 'build'
test.exec_steps = [hlt,tzreco,tzmon]
test.check_steps = CheckSteps.default_check_steps(test)

# Overwrite default histogram file name for checks
for step in [test.get_step(name) for name in ['HistCount', 'RootComp']]:
   step.input_file = 'ExampleMonitorOutput.root'

import sys
sys.exit(test.run())
