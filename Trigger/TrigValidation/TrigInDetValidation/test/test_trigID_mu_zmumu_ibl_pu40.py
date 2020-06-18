#!/usr/bin/env python

# art-description: art job for mu_Zmumu_IBL_pu40
# art-type: grid
# art-include: master/Athena
# art-input: mc15_13TeV.361107.PowhegPythia8EvtGen_AZNLOCTEQ6L1_Zmumu.recon.RDO.e3601_s2576_s2132_r7143
# art-input-nfiles: 4
# art-athena-mt: 4
# art-output: *.txt
# art-output: *.log
# art-output: log.*
# art-output: *.out
# art-output: *.err
# art-output: *.log.tar.gz
# art-output: *.new
# art-output: *.json
# art-output: *.root
# art-output: *.check*

from TrigValTools.TrigValSteering import Test, ExecStep, CheckSteps

chains = [
    'HLT_mu6_idperf_L1MU6',
    'HLT_mu24_idperf_L1MU20'
]

preexec_trig = ';'.join([
    'doEmptyMenu=True',
    'doMuonSlice=True',
    'selectChains='+str(chains)
])

preexec_reco = ';'.join([
    'pass' # TODO: figure out a working set of options to disable parts of reco
    # 'from RecExConfig.RecFlags import rec',
    # 'rec.doForwardDet=False',
    # 'rec.doEgamma=False',
    # 'rec.doMuonCombined=False',
    # 'rec.doJetMissingETTag=False',
    # 'rec.doTau=False'
])

preexec_all = ';'.join([
    'from TriggerJobOpts.TriggerFlags import TriggerFlags',
    'TriggerFlags.AODEDMSet.set_Value_and_Lock(\\\"AODFULL\\\")',
])

rdo2aod = ExecStep.ExecStep()
rdo2aod.type = 'Reco_tf'
rdo2aod.input = ''  # specified in inputRDOFile below
rdo2aod.max_events = 100 # TODO: 2000 events
rdo2aod.threads = 1 # TODO: change to 4
rdo2aod.concurrent_events = 1 # TODO: change to 4
rdo2aod.perfmon = False
rdo2aod.args = '--inputRDOFile=$ArtInFile --outputAODFile=AOD.pool.root --steering="doRDO_TRIG"'
rdo2aod.args += ' --preExec "RDOtoRDOTrigger:{:s};" "all:{:s};" "RAWtoESD:{:s};" "ESDtoAOD:{:s};"'.format(
    preexec_trig, preexec_all, preexec_reco, preexec_reco)

test = Test.Test()
test.art_type = 'grid'
test.exec_steps = [rdo2aod]
test.check_steps = CheckSteps.default_check_steps(test)

import sys
sys.exit(test.run())
