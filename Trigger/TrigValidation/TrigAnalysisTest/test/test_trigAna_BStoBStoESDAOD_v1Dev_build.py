#!/usr/bin/env python
# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
#
# art-description: Run Trigger on data with athena and write ByteStream output, then run offline reco
# art-type: build
# art-include: master/Athena

from TrigValTools.TrigValSteering import Test, ExecStep, CheckSteps
from TrigAnalysisTest.TrigAnalysisSteps import add_analysis_steps

def findFile(pattern):
    '''Bash inline file name finder'''
    return '`find . -name \'{:s}\' | tail -n 1`'.format(pattern)

menu_name = 'LS2_v1_TriggerValidation_mc_prescale'

# Run athena BS->BS job
BStoBS = ExecStep.ExecStep("BStoBS")
BStoBS.type = 'athena'
BStoBS.job_options = 'TriggerJobOpts/runHLT_standalone.py'
BStoBS.input = 'data'
BStoBS.threads = 1
BStoBSPreExec = ''.join([
  'setMenu=\'{:s}\';'.format(menu_name),
  'rewriteLVL1=True;',  # Write L1 result to BS
  'doWriteBS=True;',    # Write HLT result to BS
  'forceEnableAllChains=True;',  # Ignore L1 decisions in seeding chains
])
BStoBS.args = ' -c "{:s}"'.format(BStoBSPreExec)

# Extract the Main stream data
filterBS = ExecStep.ExecStep('FilterBS')
filterBS.type = 'other'
filterBS.executable = 'trigbs_extractStream.py'
filterBS.input = ''
filterBS.args = '-s Main ' + findFile('data_test.*.Single_Stream.daq.RAW.*.data')

# Reconstruction step, BS->ESD->AOD
recoPreExec = ' '.join([
  # Reco flags to disable things missing inputs from previous step
  "from RecExConfig.RecFlags import rec;",
  "rec.doInDet=False;",
  "rec.doAFP=False;",
  "rec.doEgamma=False;",
  "rec.doForwardDet=False;",
  "rec.doMuon=False;",
  "rec.doMuonCombined=False;",
  "rec.doJetMissingETTag=False;",
  "rec.doTau=False;",
  "rec.doLucid=False;",
  "from ParticleBuilderOptions.AODFlags import AODFlags;",
  "AODFlags.ThinNegativeEnergyNeutralPFOs.set_Value_and_Lock(False);",
  "AODFlags.AddEgammaMuonTracksInAOD.set_Value_and_Lock(False);",
  # Trigger flags
  "from AthenaConfiguration.AllConfigFlags import ConfigFlags;",
  "ConfigFlags.Trigger.triggerMenuSetup=\'{:s}\';".format(menu_name),
  "from TriggerJobOpts.TriggerFlags import TriggerFlags;",
  "TriggerFlags.configForStartup=\'HLToffline\';",
  "TriggerFlags.inputHLTconfigFile.set_Value_and_Lock(\'NONE\');",
  "TriggerFlags.AODEDMSet.set_Value_and_Lock(\'AODFULL\');"])
reco = ExecStep.ExecStep('Tier0Reco')
reco.type = 'Reco_tf'
reco.threads = 1
reco.input = ''
reco.explicit_input = True
reco.args = '--inputBSFile=' + findFile('*.physics_Main*.data')  # output of the previous step
reco.args += ' --outputESDFile=ESD.pool.root --outputAODFile=AOD.pool.root'
reco.args += ' --conditionsTag=\'CONDBR2-BLKPA-2018-11\' --geometryVersion=\'ATLAS-R2-2016-01-00-01\''
reco.args += ' --preExec="{:s}"'.format(recoPreExec)
reco.args += ' --postInclude="TriggerTest/disableChronoStatSvcPrintout.py"'

# Test definition
test = Test.Test()
test.art_type = 'build'
test.exec_steps = [BStoBS, filterBS, reco]
test.check_steps = CheckSteps.default_check_steps(test)
add_analysis_steps(test)

import sys
sys.exit(test.run())
