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


from TrigValTools.TrigValSteering import Test, CheckSteps
from TrigInDetValidation.TrigInDetArtSteps import TrigInDetReco, TrigInDetAna, TrigInDetdictStep, TrigInDetCompStep, TrigInDetCpuCostStep


import sys,getopt

try:
    opts, args = getopt.getopt(sys.argv[1:],"l",["local"])
except getopt.GetoptError:
    print("Usage: -l(--local)")


local=False
for opt,arg in opts:
    if opt in ("-l", "--local"):
        local=True




rdo2aod = TrigInDetReco()
rdo2aod.slices = ['muon','electron','tau','bjet']
rdo2aod.max_events = 1000 # TODO: 2000 events
rdo2aod.threads = 1 # TODO: change to 4
rdo2aod.concurrent_events = 4 
rdo2aod.perfmon = False
rdo2aod.timeout = 18*3600
rdo2aod.input = 'ttbar_pu80'   


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
