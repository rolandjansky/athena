#!/usr/bin/env python

# art-description: art job for all_ttbar_pu80_mp_grid
# art-type: grid
# art-include: master/Athena
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
rdo2aod.concurrent_events = 4 
rdo2aod.perfmon = False
rdo2aod.args = '--outputAODFile=AOD.pool.root --steering="doRDO_TRIG" '
rdo2aod.input = 'ttbar_pu80'   ## This isn't the same sample as the grid test but for not lets use it.

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
comp.chains = 'HLT_mu24_idperf_L1MU20:HLT_IDTrack_Muon_FTF'
comp.output_dir = 'HLTL2-plots-muon'
test.check_steps.append(comp)
 
 
comp2=TrigInDetCompStep('CompareStep2')
comp2.chains='HLT_mu24_idperf_L1MU20:HLT_IDTrack_Muon_FTF  HLT_mu24_idperf_L1MU20:HLT_IDTrack_Muon_IDTrig'
comp2.output_dir = 'HLTEF-plots-muon'
test.check_steps.append(comp2)

comp3=TrigInDetCompStep('CompareStep3')
comp3.chains='HLT_j45_ftf_subjesgscIS_boffperf_split_L1J20:HLT_IDTrack_Bjet_FTF HLT_j45_ftf_subjesgscIS_boffperf_split_L1J20:HLT_IDTrack_Bjet_IDTrig'
comp3.output_dir = 'HLTEF-plots-bjet'
test.check_steps.append(comp3)

comp4=TrigInDetCompStep('CompareStep4')
comp4.chains='HLT_e5_etcut_L1EM3:HLT_IDTrack_Electron_FTF HLT_e5_etcut_L1EM3:HLT_IDTrack_Electron_IDTrig'
comp4.output_dir = 'HLTEF-plots-electron'
test.check_steps.append(comp4)

comp5=TrigInDetCompStep('CompareStep5')
comp5.chains='HLT_tau25_idperf_tracktwo_L1TAU12IM:HLT_IDTrack_TauCore_FTF HLT_tau25_idperf_tracktwo_L1TAU12IM:HLT_IDTrack_Tau_IDTrig'
comp5.output_dir = 'HLTEF-plots-tau'
test.check_steps.append(comp5)


cpucost=TrigInDetCpuCostStep('CpuCostStep1')
test.check_steps.append(cpucost)

cpucost2=TrigInDetCpuCostStep('CpuCostStep2')
cpucost2.args += '  -p FastTrack'
cpucost2.output_dir = 'times-FTF' 
test.check_steps.append(cpucost2)


import sys
sys.exit(test.run())
