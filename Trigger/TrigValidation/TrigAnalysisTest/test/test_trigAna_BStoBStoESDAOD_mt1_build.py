#!/usr/bin/env python

# art-description: Run Trigger on data with athena and write ByteStream output, then run offline reco
# art-type: build
# art-include: master/Athena

from TrigValTools.TrigValSteering import Test, ExecStep, CheckSteps
from TrigAnalysisTest.TrigAnalysisSteps import add_analysis_steps

def findFile(pattern):
    '''Bash inline file name finder'''
    return '`find . -name \'{:s}\' | tail -n 1`'.format(pattern)

# Run athena BS->BS job
BStoBS = ExecStep.ExecStep("BStoBS")
BStoBS.type = 'athena'
BStoBS.job_options = 'TriggerJobOpts/runHLT_standalone.py'
BStoBS.input = 'data'
BStoBS.threads = 1
BStoBS.args += ' -c "setMenu=\'LS2_v1\';doWriteBS=True;doWriteRDOTrigger=False;forceEnableAllChains=True;"'

# Extract the Main stream data
filterBS = ExecStep.ExecStep('FilterBS')
filterBS.type = 'other'
filterBS.executable = 'athenaHLT-select-PEB-stream.py'
filterBS.input = ''
filterBS.args = '-s Main ' + findFile('data_test.*.Single_Stream.daq.RAW.*.data')

# Reconstruction step, BS->ESD->AOD
recoPreExec = ' '.join([
  "from TriggerJobOpts.TriggerFlags import TriggerFlags;",
  "TriggerFlags.configForStartup=\'HLToffline\';",
  "TriggerFlags.triggerMenuSetup=\'PhysicsP1_pp_run3_v1\';",
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
