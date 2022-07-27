#!/usr/bin/env python
# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

# art-description: Test of P1+Tier0 workflow, runs athenaHLT with PhysicsP1_pp_run3_v1 menu followed by offline reco, monitoring and analysis step for EDM monitoring
# art-type: grid
# art-athena-mt: 4
# art-include: master/Athena
# art-include: 22.0/Athena
# art-output: *.txt
# art-output: *.log
# art-output: log.*
# art-output: *.new
# art-output: *.json
# art-output: *.root
# art-output: *.pmon.gz
# art-output: *perfmon*
# art-output: prmon*
# art-output: *.check*

from TrigValTools.TrigValSteering import Test, ExecStep, CheckSteps
from TrigValTools.TrigValSteering.Common import find_file
from TrigAnalysisTest.TrigAnalysisSteps import add_analysis_steps

# Specify trigger menu once here:
triggermenu = 'PhysicsP1_pp_run3_v1_HLTReprocessing_prescale'

# HLT step (BS->BS)
hlt = ExecStep.ExecStep()
hlt.type = 'athenaHLT'
hlt.job_options = 'TriggerJobOpts/runHLT_standalone.py'
hlt.forks = 1
hlt.threads = 4
hlt.concurrent_events = 4
hlt.input = 'data'
hlt.args = f'-c "setMenu=\'{triggermenu}\';doL1Sim=True;rewriteLVL1=True;"'
hlt.args += ' -o output'
hlt.args += ' --dump-config-reload'

# Extract the physics_Main stream out of the BS file with many streams
filter_bs = ExecStep.ExecStep('FilterBS')
filter_bs.type = 'other'
filter_bs.executable = 'trigbs_extractStream.py'
filter_bs.input = ''
filter_bs.args = '-s Main ' + find_file('*_HLTMPPy_output.*.data')

# Tier-0 reco step (BS->ESD->AOD)
tzrecoPreExec = ' '.join([
  "from AthenaConfiguration.AllConfigFlags import ConfigFlags;",
  f"ConfigFlags.Trigger.triggerMenuSetup=\'{triggermenu}\';",
  "ConfigFlags.Trigger.AODEDMSet=\'AODFULL\';",
  "ConfigFlags.Trigger.enableL1CaloPhase1=True;",
  "from AthenaMonitoring.DQMonFlags import DQMonFlags;",
  "DQMonFlags.set_All_Off();",
  "DQMonFlags.doDataFlowMon=True;",
  "DQMonFlags.doHLTMon=True;",
  "DQMonFlags.doLVL1CaloMon=True;",
  "DQMonFlags.doGlobalMon=True;", 
  "DQMonFlags.doLVL1InterfacesMon=True;",
  "DQMonFlags.doCTPMon=True;",
  "ConfigFlags.DQ.Steering.HLT.doBjet=True;",
  "ConfigFlags.DQ.Steering.HLT.doInDet=True;",
  "ConfigFlags.DQ.Steering.HLT.doBphys=True;",
  "ConfigFlags.DQ.Steering.HLT.doCalo=True;",
  "ConfigFlags.DQ.Steering.HLT.doEgamma=True;",
  "ConfigFlags.DQ.Steering.HLT.doJet=True;",
  "ConfigFlags.DQ.Steering.HLT.doMET=True;",
  "ConfigFlags.DQ.Steering.HLT.doMinBias=True;",
  "ConfigFlags.DQ.Steering.HLT.doMuon=True;",
  "ConfigFlags.DQ.Steering.HLT.doTau=True;",
])

tzreco = ExecStep.ExecStep('Tier0Reco')
tzreco.type = 'Reco_tf'
tzreco.threads = 4
tzreco.concurrent_events = 4
tzreco.input = ''
tzreco.explicit_input = True
tzreco.args = '--inputBSFile=' + find_file('*.physics_Main*._athenaHLT*.data')  # output of the previous step
tzreco.args += ' --outputESDFile=ESD.pool.root --outputAODFile=AOD.pool.root'
tzreco.args += ' --outputHISTFile=hist.root'
tzreco.args += ' --conditionsTag=\'CONDBR2-BLKPA-RUN2-06\' --geometryVersion=\'ATLAS-R2-2016-01-00-01\''
tzreco.args += ' --preExec="{:s}"'.format(tzrecoPreExec)
tzreco.args += ' --postInclude="TriggerTest/disableChronoStatSvcPrintout.py"'
tzreco.args += ' --steering doRAWtoALL'

# The full test
test = Test.Test()
test.art_type = 'grid'
test.exec_steps = [hlt, filter_bs, tzreco]
test.check_steps = CheckSteps.default_check_steps(test)
add_analysis_steps(test)

# Overwrite default histogram file name for checks
for step in [test.get_step(name) for name in ['HistCount', 'RootComp']]:
    step.input_file = 'ExampleMonitorOutput.root'

import sys
sys.exit(test.run())
