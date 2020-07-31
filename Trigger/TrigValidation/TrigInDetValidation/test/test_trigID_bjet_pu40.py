#!/usr/bin/env python

# art-description: art job for all_ttbar_pu80_grid
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
# art-output: HLT*
# art-output: times*
# art-output: cost-perCall
# art-output: cost-perEvent
# art-output: cost-perCall-chain
# art-output: cost-perEvent-chain
# art-output: *.dat 


from TrigValTools.TrigValSteering import Test, ExecStep, CheckSteps
from TrigInDetValidation.TrigInDetArtSteps import TrigInDetAna, TrigInDetdictStep, TrigInDetCompStep, TrigInDetCpuCostStep


import sys,getopt

try:
    opts, args = getopt.getopt(sys.argv[1:],"lxp",["local"])
except getopt.GetoptError:
    print("Usage:  ")
    print("-l(--local)    run locally with input file from art eos grid-input")
    print("-x             don't run athena or post post-processing, only plotting")
    print("-p             run post-processing, even if -x is set")


local=False
exclude=False
postproc=False
for opt,arg in opts:
    if opt in ("-l", "--local"):
        local=True
    if opt=="-x":
        exclude=True
    if opt=="-p":
        postproc=True


chains = [
    'HLT_j45_ftf_subjesgscIS_boffperf_split_L1J20'
]

preexec_trig = ';'.join([
    'doEmptyMenu=True',
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
rdo2aod.concurrent_events = 1 # TODO: change to 4
rdo2aod.perfmon = False
rdo2aod.timeout = 18*3600
rdo2aod.args = '--outputAODFile=AOD.pool.root --steering="doRDO_TRIG" '
rdo2aod.input = 'ttbar'   

rdo2aod.args += ' --preExec "RDOtoRDOTrigger:{:s};" "all:{:s};" "RAWtoESD:{:s};" "ESDtoAOD:{:s};"'.format(
    preexec_trig, preexec_all, preexec_reco, preexec_aod)

test = Test.Test()
test.art_type = 'grid'
if (not exclude):
    test.exec_steps = [rdo2aod]
    test.exec_steps.append(TrigInDetAna()) # Run analysis to produce TrkNtuple
    test.check_steps = CheckSteps.default_check_steps(test)

# Run Tidardict
if ((not exclude) or postproc ):
    rdict = TrigInDetdictStep()
    rdict.args='TIDAdata-run3.dat  -f data-hists.root -b Test_bin.dat '
    test.check_steps.append(rdict)

 
# Now the comparitor steps
comp1=TrigInDetCompStep('CompareStep1')
comp1.chains='HLT_j45_ftf_subjesgscIS_boffperf_split_L1J20:HLT_IDTrack_Bjet_FTF HLT_j45_ftf_subjesgscIS_boffperf_split_L1J20:HLT_IDTrack_Bjet_IDTrig'
comp1.output_dir = 'HLTEF-plots-bjet'
test.check_steps.append(comp1)



cpucost=TrigInDetCpuCostStep('CpuCostStep1')
test.check_steps.append(cpucost)

cpucost2=TrigInDetCpuCostStep('CpuCostStep2')
cpucost2.args += '  -p FastTrack'
cpucost2.output_dir = 'times-FTF' 
test.check_steps.append(cpucost2)


import sys
sys.exit(test.run())
