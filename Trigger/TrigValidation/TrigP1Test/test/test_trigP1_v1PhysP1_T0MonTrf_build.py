#!/usr/bin/env python
# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

# art-description: transform test of BSRDOtoRAW + T0Reco + T0Mon, using v1PhysP1 menu
# art-type: build
# art-include: master/Athena

from TrigValTools.TrigValSteering import Test, ExecStep, CheckSteps

#====================================================================================================
# HLT BS_RDO->RAW
hlt = ExecStep.ExecStep('BSRDOtoRAW')
hlt.type = 'Trig_reco_tf'
hlt.forks = 1
hlt.threads = 4
hlt.concurrent_events = 4
hlt.max_events = 50
hlt.args = '--precommand=\\\"setMenu=\\\'PhysicsP1_pp_run3_v1\\\'\\;doL1Sim=True\\;rewriteLVL1=True\\;\\\"'
hlt.args += ' --prodSysBSRDO True'
hlt.args += ' --outputBSFile=RAW.pool.root'
hlt.args += ' --outputHIST_HLTMONFile=hltmon.root'
hlt.args += ' --outputDRAW_TRIGCOSTFile=TRIGCOST.pool.root'
hlt.args += ' --outputNTUP_TRIGCOSTFile=cost.ntup.root'
hlt.args += ' --runNumber 360026'  # RunNumber is set by Panda, but ignored by Trf to avoid changes from !48070
hlt.input = 'data'

#====================================================================================================
# Tier-0 reco step BS->ESD->AOD
tzrecoPreExec = ' '.join([
 "from AthenaConfiguration.AllConfigFlags import ConfigFlags;",
 "ConfigFlags.Trigger.triggerMenuSetup=\'PhysicsP1_pp_run3_v1\';",
 "ConfigFlags.Trigger.AODEDMSet=\'AODFULL\';",
 "ConfigFlags.Trigger.enableL1MuonPhase1=True;",
 "ConfigFlags.Trigger.enableL1CaloPhase1=True;",
])

tzreco = ExecStep.ExecStep('Tier0Reco')
tzreco.type = 'Trig_reco_tf'
tzreco.threads = 4
tzreco.concurrent_events = 4
tzreco.explicit_input = True
tzreco.input = ''
tzreco.max_events = 50
tzreco.args = '--inputBSFile=RAW.pool.root'  # output of the previous step
tzreco.args += ' --outputESDFile=ESD.pool.root --outputAODFile=AOD.pool.root'
tzreco.args += ' --outputNTUP_TRIGRATEFile=rate.ntup.root'
tzreco.args += ' --conditionsTag=\'CONDBR2-BLKPA-RUN2-06\' --geometryVersion=\'ATLAS-R2-2016-01-00-01\''
tzreco.args += ' --preExec="{:s}"'.format(tzrecoPreExec)
tzreco.args += ' --postInclude="TriggerTest/disableChronoStatSvcPrintout.py"'

#====================================================================================================
# Tier-0 monitoring step (AOD->HIST)
tzmon = ExecStep.ExecStep('Tier0Mon')
tzmon.type = 'other'
tzmon.executable = 'Run3DQTestingDriver.py'
tzmon.input = ''
tzmon.args = '--threads=1'
tzmon.args += ' --dqOffByDefault'
tzmon.args += ' Input.Files="[\'AOD.pool.root\']" DQ.Steering.doHLTMon=True Trigger.triggerMenuSetup=\'PhysicsP1_pp_run3_v1\''

#====================================================================================================
# Merging NTUP_TRIGRATE/COST
tzmergecost = ExecStep.ExecStep('Tier0MergeCost')
tzmergecost.type = 'other'
tzmergecost.executable = 'NTUPMerge_tf.py'
tzmergecost.input = ''                                                                             
tzmergecost.args = '--inputNTUP_TRIGCOSTFile="cost.ntup.root" --outputNTUP_TRIGCOST_MRGFile="cost.ntup.merge.root"'

tzmergerate = ExecStep.ExecStep('Tier0MergeRate')
tzmergerate.type = 'other'
tzmergerate.executable = 'NTUPMerge_tf.py'
tzmergerate.input = ''                                                                             
tzmergerate.args = '--inputNTUP_TRIGRATEFile="rate.ntup.root" --outputNTUP_TRIGRATE_MRGFile="rate.ntup.merge.root"'

#====================================================================================================
# The full test
test = Test.Test()
test.art_type = 'build'
test.exec_steps = [hlt,tzreco,tzmon,tzmergecost,tzmergerate]
test.check_steps = CheckSteps.default_check_steps(test)

# Overwrite default histogram file name for checks
for step in [test.get_step(name) for name in ['HistCount', 'RootComp']]:
   step.input_file = 'ExampleMonitorOutput.root'

import sys
sys.exit(test.run())
