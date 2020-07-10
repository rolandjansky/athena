#!/usr/bin/env python

# art-description: art job for all_ttbar_pu80_mp_grid
# art-type: grid
# art-include: master/Athena
# art-input: mc15_13TeV.410000.PowhegPythiaEvtGen_P2012_ttbar_hdamp172p5_nonallhad.recon.RDO.e3698_s2608_s2183_r7195
# art-input-nfiles: 3
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
# art-output: HLTEF-plots
# art-output: HLTL2-plots
# art-output: times
# art-output: times-FTF
# art-output: cost-perCall
# art-output: cost-perEvent
# art-output: cost-perCall-chain
# art-output: cost-perEvent-chain
# art-output: *.dat 


from TrigValTools.TrigValSteering import Test, ExecStep, CheckSteps
from TrigInDetValidation.TrigInDetArtSteps import TrigInDetAna, TrigInDetdictStep, TrigInDetCompStep


import sys,getopt

try:
    opts, args = getopt.getopt(sys.argv[1:],"l",["local"])
except getopt.GetoptError:
    print("Usage: -l(--local)")


local=False
for opt,arg in opts:
    if opt in ("-l", "--local"):
        local=True



chains = [
    'HLT_mu6_idperf_L1MU6',
    'HLT_mu24_idperf_L1MU20',
    'HLT_e5_etcut_L1EM3',  ## need an idperf chain once one is in the menu
    'HLT_tau25_idperf_tracktwo_L1TAU12IM',
    'HLT_j45_ftf_subjesgscIS_boffperf_split_L1J20'
]

preexec_trig = ';'.join([
    'doEmptyMenu=True',
    'doMuonSlice=True',
    'doEgammaSlice=True',
    'doTauSlice=True',
    'doBjetSlice=True',
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
rdo2aod.max_events = 1000 # TODO: 2000 events
rdo2aod.threads = 1 # TODO: change to 4
rdo2aod.concurrent_events = 4 # TODO: change to 4
rdo2aod.perfmon = False
rdo2aod.args = '--outputAODFile=AOD.pool.root --steering="doRDO_TRIG" '
if local:
    rdo2aod.input = 'ttbar_pu80'   ## This isn't the same sample as the grid test but for not lets use it.
else:
    rdo2aod.input = ''
    rdo2aod.args += '--inputRDOFile=$ArtInFile '

rdo2aod.args += ' --preExec "RDOtoRDOTrigger:{:s};" "all:{:s};" "RAWtoESD:{:s};" "ESDtoAOD:{:s};"'.format(
    preexec_trig, preexec_all, preexec_reco, preexec_aod)

test = Test.Test()
test.art_type = 'grid'
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
