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

        
from TrigValTools.TrigValSteering import Test, CheckSteps
from TrigInDetValidation.TrigInDetArtSteps import TrigInDetReco, TrigInDetAna, TrigInDetdictStep, TrigInDetCompStep, TrigInDetCpuCostStep

rdo2aod = TrigInDetReco()
rdo2aod.slices = ['muon']
rdo2aod.max_events = 100 # TODO: 2000 events
rdo2aod.threads = 1 # TODO: change to 4
rdo2aod.concurrent_events = 1 # TODO: change to 4
rdo2aod.perfmon = False
rdo2aod.input = 'Zmumu_pu40'   # defined in TrigValTools/share/TrigValInputs.json  


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
comp=TrigInDetCompStep( 'Comp_L2muon', 'L2', 'muon' )
comp.flag = 'L2muon'
test.check_steps.append(comp)
  
comp2=TrigInDetCompStep( 'Comp_EFmuon', 'EF', 'muon' )
comp2.flag = 'EFmuon'
test.check_steps.append(comp2)

# CPU cost steps
cpucost=TrigInDetCpuCostStep('CpuCostStep1', ftf_times=False)
test.check_steps.append(cpucost)

cpucost2=TrigInDetCpuCostStep('CpuCostStep2')
test.check_steps.append(cpucost2)

import sys
sys.exit(test.run())
