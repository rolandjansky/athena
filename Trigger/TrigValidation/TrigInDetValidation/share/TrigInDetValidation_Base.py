###  #!/usr/bin/env python

# Slices = ['fsjet']
# Events = 10
# Threads = 1
# Slots = 1
# Input = 'ttbar'    # defined in TrigValTools/share/TrigValInputs.json   
# TrackReference = 'Truth'

import re

from TrigValTools.TrigValSteering import Test, CheckSteps
from TrigInDetValidation.TrigInDetArtSteps import TrigInDetReco, TrigInDetAna, TrigInDetRdictStep, TrigInDetCompStep, TrigInDetCpuCostStep


import sys,getopt

try:
    opts, args = getopt.getopt(sys.argv[1:],"lcxptn:",["local","config"])
except getopt.GetoptError:
    print("Usage:  ")
    print("-l(--local)    run locally with input file from art eos grid-input")
    print("-x             don't run athena or post post-processing, only plotting")
    print("-p             run post-processing, even if -x is set")
    print("-n  N          run only on N events per job")
    print("-c(--config)   run with config_only and print to a pkl file")
    print("-t             test steering, dry run for all steps")
    print("")
    sys.exit(1)

Events_local  = 0
local         = False
exclude       = False
postproc      = False
testconfig    = False
dry_run       = False


if "Art_type"  not in locals(): Art_type = 'grid'
if "GridFiles" not in locals(): GridFiles=False

for opt,arg in opts:
    if opt in ("-l", "--local"):
        local=True
    if opt=="-x":
        exclude=True
    if opt=="-p":
        postproc=True
    if opt=="-n":
        Events_local=arg
    if opt in ("-c", "--config"):
        testconfig = True
    if opt=="-t":
        dry_run=True


if 'postinclude_file' in dir() :
    if 'preinclude_file' in dir() :
        rdo2aod = TrigInDetReco( postinclude_file=postinclude_file, preinclude_file=preinclude_file )
    else :
        rdo2aod = TrigInDetReco( postinclude_file=postinclude_file )
else :
    if 'preinclude_file' in dir() :
        rdo2aod = TrigInDetReco( preinclude_file=preinclude_file )
    else :
        rdo2aod = TrigInDetReco()

# test specific variables ...

rdo2aod.slices            = Slices
rdo2aod.threads           = Threads
rdo2aod.concurrent_events = Slots 
rdo2aod.config_only       = testconfig
if 'Release' in dir():
    rdo2aod.release           = Release


if "Args" not in locals() : 
    Args = " "


# allow command line to override programed number of events to process

if Events_local != 0 : 
    rdo2aod.max_events        = Events_local 
else :
    rdo2aod.max_events        = Events 


rdo2aod.perfmon = False
rdo2aod.timeout = 18*3600
rdo2aod.input   = Input    # defined in TrigValTools/share/TrigValInputs.json  

if GridFiles:
    if local:
#   rdo2aod.input = 'Single_el_larged0'    # defined in TrigValTools/share/TrigValInputs.json  
       rdo2aod.input = Input   # should match definition in TrigValTools/share/TrigValInputs.json  
    else:
       rdo2aod.input = ''
       rdo2aod.args += ' --inputRDOFile=$ArtInFile '



# Run athena analysis to produce TrkNtuple

test = Test.Test()
test.art_type = Art_type

lrt_mode = False
if 'LRT' in dir() :
    lrt_mode = LRT
aod_to_ntup = TrigInDetAna(lrt=lrt_mode)


if dry_run:
    test.dry_run = True
if (not exclude):
    test.exec_steps = [rdo2aod, aod_to_ntup]
    test.check_steps = CheckSteps.default_check_steps(test)

# Run TIDArdict


if ((not exclude) or postproc ):
    for job in Jobs :
        if len(job) >= 3:
            rdict = TrigInDetRdictStep( name=job[0], args=job[1], testbin=job[2] )
        else:
            rdict = TrigInDetRdictStep( name=job[0], args=job[1] )
        print( "\n\033[0;32m TIDArdict "+job[1]+" \033[0m" )
        test.check_steps.append(rdict)
       
        
for _slice in Comp :
    compstep = TrigInDetCompStep( name=_slice[0], slice=_slice[1], file=_slice[2], args=_slice[3] ) 
    test.check_steps.append(compstep)

# CPU cost steps

cpucost=TrigInDetCpuCostStep('CpuCostStep1', ftf_times=False)
test.check_steps.append(cpucost)

cpucost2=TrigInDetCpuCostStep('CpuCostStep2')
test.check_steps.append(cpucost2)

import sys
sys.exit(test.run())
