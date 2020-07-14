#!/usr/bin/env python

# art-description: art job for mu_Zmumu_pu40_build
# art-type: build
# art-include: master/Athena

import sys,getopt

try:
    opts, args = getopt.getopt(sys.argv[1:],"lxp",["local"])
except getopt.GetoptError:
    print("Usage:  ")
    print ("-x             don't run athena or post post-processing, only plotting")
    print ("-p             run post-processing, even if -x is set")


exclude=False
postproc=False
for opt,arg in opts:
    if opt=="-x":
        exclude=True
    if opt=="-p":
        postproc=True

        
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
    'from RecExConfig.RecFlags import rec',
    'rec.doForwardDet=False',
    'rec.doEgamma=False',
    'rec.doMuonCombined=False',
    'rec.doJetMissingETTag=False',
    'rec.doTau=False'
])

preexec_aod = ';'.join([
     preexec_reco,
     'from ParticleBuilderOptions.AODFlags import AODFlags',
     'AODFlags.ThinGeantTruth.set_Value_and_Lock(False)',
     'AODFlags.ThinNegativeEnergyCaloClusters.set_Value_and_Lock(False)',
     'AODFlags.ThinNegativeEnergyNeutralPFOs.set_Value_and_Lock(False)',
     'AODFlags.ThinInDetForwardTrackParticles.set_Value_and_Lock(False)'
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
    preexec_trig, preexec_all, preexec_reco, preexec_aod)

test = Test.Test()
test.art_type = 'build'
if (not exclude):
    test.exec_steps = [rdo2aod]
    test.exec_steps.append(TrigInDetAna()) # Run analysis to produce TrkNtuple
    test.check_steps = CheckSteps.default_check_steps(test)

 
# Run Tidardict
if ((not exclude) or postproc ):
    rdict = TrigInDetdictStep()
    rdict.args='TIDAdata-run3.dat -f data-hists.root -p 13 -b Test_bin.dat '
    test.check_steps.append(rdict)
 

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
