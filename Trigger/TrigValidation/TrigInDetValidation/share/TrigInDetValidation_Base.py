###  #!/usr/bin/env python



# Slices = ['fsjet']
# RunEF  = False
# Events = 10
# Threads = 1
# Slots = 1
# Input = 'ttbar'    # defined in TrigValTools/share/TrigValInputs.json   
# TrackReference = 'Truth'

import re

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

if local:
#   rdo2aod.input = 'Single_el_larged0'    # defined in TrigValTools/share/TrigValInputs.json  
    rdo2aod.input = Input   # should match definition in TrigValTools/share/TrigValInputs.json  
else:
    rdo2aod.input = ''
    rdo2aod.args += ' --inputRDOFile=$ArtInFile '



# Run athena analysis to produce TrkNtuple

test = Test.Test()
test.art_type = 'grid'
if (not exclude):
    test.exec_steps = [rdo2aod]
    test.exec_steps.append(TrigInDetAna())
    test.check_steps = CheckSteps.default_check_steps(test)



# Run TIDArdict

# first make sure that we have a proper list ..
if isinstance( TrackReference, str ):
    TrackReference = [ TrackReference ]

for ref in TrackReference : 

    hist_file = 'data-hists.root'
    ext       = ''
    ext1      = ''

    if   ( ref == 'Truth' ) :
        args      = 'TIDAdata-run3.dat  -b Test_bin.dat -o '+hist_file
    elif ( ref == 'Offline' ) :
        if len(TrackReference)>1 : 
            hist_file = 'data-hists-offline.root'
            ext       = '_off'
            ext1      = 'offl'
        args      = 'TIDAdata-run3-offline.dat  -b Test_bin.dat -o '+hist_file
    else :
        # here actually we should allow functionality 
        # to use different pdgid truth or offline as
        # a reference:
        # presumably we run offline muons etc as well 
        # now in the transform
        raise Exception( 'unknown reference: ', ref )

    if ((not exclude) or postproc ):
        rdict = TrigInDetdictStep( name=ref, reference=ref )
        rdict.args = args
        print( "\033[0;32m TIDArdict "+args+" \033[0m" )

        test.check_steps.append(rdict)
       
    # Now the comparitor steps
    # here, the compararitor must know the name of the root file to process
    # we set it in the comparitor job, using the _off extension
    # this isn't ideal, since we set the hist file in this code, 
    # so really we should pass it in consistently, and the options 
    # for the directory names should be unrelated 

    for slice in Slices :
      comp1=TrigInDetCompStep( 'Comp_L2'+slice+ext, 'L2', slice, type=ext1)
      test.check_steps.append(comp1)

      if ( RunEF ) : 
        comp2=TrigInDetCompStep( 'Comp_EF'+slice+ext, 'EF', slice, type=ext1)
        test.check_steps.append(comp2)



# CPU cost steps

cpucost=TrigInDetCpuCostStep('CpuCostStep1', ftf_times=False)
test.check_steps.append(cpucost)

cpucost2=TrigInDetCpuCostStep('CpuCostStep2')
test.check_steps.append(cpucost2)

import sys
sys.exit(test.run())
