###  #!/usr/bin/env python



# Slices = ['fsjet']
# RunEF  = False
# Events = 10
# Threads = 1
# Slots = 1
# Input = 'ttbar'    # defined in TrigValTools/share/TrigValInputs.json   
# TrackReference = 'Truth'

from TrigValTools.TrigValSteering import Test, CheckSteps
from TrigInDetValidation.TrigInDetArtSteps import TrigInDetReco, TrigInDetAna, TrigInDetdictStep, TrigInDetCompStep, TrigInDetCpuCostStep


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



rdo2aod = TrigInDetReco()

# test specific variables ...

rdo2aod.slices            = Slices
rdo2aod.max_events        = Events 
rdo2aod.threads           = Threads
rdo2aod.concurrent_events = Slots 



rdo2aod.perfmon = False
rdo2aod.timeout = 18*3600
rdo2aod.input = Input    # defined in TrigValTools/share/TrigValInputs.json  


test = Test.Test()
test.art_type = 'grid'
if (not exclude):
    test.exec_steps = [rdo2aod]
    test.exec_steps.append(TrigInDetAna()) # Run analysis to produce TrkNtuple
    test.check_steps = CheckSteps.default_check_steps(test)

# Run Tidardict
if ((not exclude) or postproc ):
    rdict = TrigInDetdictStep()
    if ( TrackReference == "Truth" ) : 
       rdict.args='TIDAdata-run3.dat  -f data-hists.root -b Test_bin.dat '
    if ( TrackReference == "Offline" ) : 
       rdict.args='TIDAdata-run3-offline.dat  -f data-hists.root -b Test_bin.dat '

    test.check_steps.append(rdict)

 
for slice in Slices :
  # Now the comparitor steps
  comp1=TrigInDetCompStep('Comp_L2'+slice,'L2',slice)
  test.check_steps.append(comp1)

  if ( RunEF ) : 
    comp2=TrigInDetCompStep('Comp_EF'+slice,'EF', slice)
    test.check_steps.append(comp2)


# CPU cost steps
cpucost=TrigInDetCpuCostStep('CpuCostStep1', ftf_times=False)
test.check_steps.append(cpucost)

cpucost2=TrigInDetCpuCostStep('CpuCostStep2')
test.check_steps.append(cpucost2)

import sys
sys.exit(test.run())
