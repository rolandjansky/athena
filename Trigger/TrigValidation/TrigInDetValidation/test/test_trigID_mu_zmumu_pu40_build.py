#!/usr/bin/env python

# art-description: art job for mu_Zmumu_IBL_pu40
# art-type: build
# art-include: master/Athena


from TrigValTools.TrigValSteering import Test, ExecStep, CheckSteps
from TrigInDetValidation.TrigInDetArtSteps import TrigInDetAna, TrigInDetdictStep, TrigInDetCompStep

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
rdo2aod.input = 'Zmumu_pu40'  
rdo2aod.max_events = 10 # TODO: 2000 events
rdo2aod.threads = 1 # TODO: change to 4
rdo2aod.concurrent_events = 1 # TODO: change to 4
rdo2aod.perfmon = False
rdo2aod.args = '--outputAODFile=AOD.pool.root --steering="doRDO_TRIG"'
rdo2aod.args += ' --preExec "RDOtoRDOTrigger:{:s};" "all:{:s};" "RAWtoESD:{:s};" "ESDtoAOD:{:s};"'.format(
    preexec_trig, preexec_all, preexec_reco, preexec_reco)

test = Test.Test()
test.art_type = 'build'
test.exec_steps = [rdo2aod]
test.check_steps = CheckSteps.default_check_steps(test)

# Run analysis to produce TrkNtuple
test.exec_steps.append(TrigInDetAna())
 
# Run Tidardict
test.check_steps.append(TrigInDetdictStep())
 
# Now the comparitor steps
comp=TrigInDetCompStep('CompareStep1')
comp.chains = 'HLT_mu24_idperf_InDetTrigTrackingxAODCnv_Muon_FTF'
comp.output_dir = 'HLT-plots-FTF'
test.check_steps.append(comp)
 
 
comp2=TrigInDetCompStep('CompareStep2')
comp2.chains='HLT_mu24_idperf_InDetTrigTrackingxAODCnv_Muon_FTF HLT_mu24_idperf_InDetTrigTrackingxAODCnv_Muon_IDTrig'
comp2.output_dir = 'HLT-plots-IDTrig'
test.check_steps.append(comp2)


import sys
sys.exit(test.run())
