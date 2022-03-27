#!/Usr/bin/env python

# Slices = ['fsjet']
# Events = 10
# Threads = 1
# Slots = 1
# Input = 'ttbar'    # defined in TrigValTools/share/TrigValInputs.json   
# TrackReference = 'Truth'

import re

from TrigValTools.TrigValSteering import Test, CheckSteps
from TrigInDetValidation.TrigInDetArtSteps import TrigInDetReco, TrigInDetAna, TrigCostStep, TrigInDetRdictStep, TrigInDetCompStep, TrigInDetCpuCostStep

import os,sys,getopt

try:
    opts, args = getopt.getopt(sys.argv[1:],"lcxptirmn:",["local","config"])
except getopt.GetoptError:
    print("Usage:  ")
    print("-l | --local   run locally with input file from art eos grid-input")
    print("-x             don't run athena or post post-processing, only plotting")
    print("-m             run cost monitoring plotting, even if -x is set")
    print("-p             run post-processing, even if -x is set")
    print("-n  N          run only on N events per job")
    print("-c | --config  run with config_only and print to a pkl file")
    print("-t             test steering, dry run for all steps")
    print("-i             force the dry AbortDryRun flag to be False")
    print("")
    sys.exit(1)

Events_local  = 0
local         = False
exclude       = False
costplot      = False
postproc      = False
testconfig    = False
dry_run       = False
runstuff      = True
abort_dry_run = True


if "Art_type"  not in locals(): Art_type = 'grid'
if "GridFiles" not in locals(): GridFiles=False
if "Malloc" not in locals(): Malloc=False
if "AbortDryRun" in locals(): abort_dry_run=AbortDryRun

for opt,arg in opts:
    if opt in ("-l", "--local"):
        local=True
    if opt=="-x":
        exclude=True
    if opt=="-p":
        postproc=True
    if opt=="-m":
        costplot=True
    if opt=="-n":
        Events_local=int(arg)
    if opt=="-r":
        runstuff=False
    if opt in ("-c", "--config"):
        testconfig = True
    if opt=="-t":
        dry_run=True
    if opt=="-i":
        abort_dry_run=False

vdry_run = os.environ.get('TRIGVALSTEERING_DRY_RUN')

if vdry_run == '1':
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
rdo2aod.costmon = True
rdo2aod.timeout = 18*3600
rdo2aod.input   = Input    # defined in TrigValTools/share/TrigValInputs.json  

if GridFiles:
    if local:
#   rdo2aod.input = 'Single_el_larged0'    # defined in TrigValTools/share/TrigValInputs.json  
       rdo2aod.input = Input   # should match definition in TrigValTools/share/TrigValInputs.json  
    else:
       rdo2aod.input = ''
       rdo2aod.args += ' --inputRDOFile=$ArtInFile '

if (Malloc):
    import os
    os.environ["MALLOC_CHECK_"] = "3"
    rdo2aod.malloc = True

for a in ["preexec_trig", "preexec_reco", "preexec_aod", "preexec_all", "postexec_trig", "postexec_reco"]:
    if a in locals():
        v = locals()[a]
        if type(v) is list: v = ";".join(v)
        v0 = getattr (rdo2aod, a)
        if v0 is None or v0 == "":
            v0 = v
        else:
            v0 += ";"+v
        setattr (rdo2aod, a, v0)

# Run athena analysis to produce TrkNtuple

test = Test.Test()
test.art_type = Art_type


if 'ExtraAna' not in locals() :
    ExtraAna = None
aod_to_ntup = TrigInDetAna(extraArgs = ExtraAna)


rdo_to_cost = TrigCostStep()

if dry_run:
    if abort_dry_run:
        sys.exit(0)
    test.dry_run = True
if (not exclude):
    if runstuff:
        test.exec_steps = [rdo2aod, aod_to_ntup, rdo_to_cost]
    else:
        test.exec_steps = [aod_to_ntup]
    test.check_steps = CheckSteps.default_check_steps(test)

# Run TIDArdict

if ((not exclude) or postproc ):
    for job in Jobs :
        if len(job) >= 3:
            rdict = TrigInDetRdictStep( name=job[0], args=job[1], testbin=job[2], config=(testconfig or dry_run)  )
        else:
            rdict = TrigInDetRdictStep( name=job[0], args=job[1], config=(testconfig or dry_run) )
        print( "\n\033[0;32m TIDArdict "+job[1]+" \033[0m" )
        test.check_steps.append(rdict)



       
        
for _slice in Comp :
    compstep = TrigInDetCompStep( name=_slice[0], slice=_slice[1], file=_slice[2], args=_slice[3] ) 
    test.check_steps.append(compstep)

# CPU cost steps
# cputest defined with "name" "output directory" "filename" "args" 
cputest = [ ( "CpuCostStep1", " times ",               "expert-monitoring.root",    " --auto -p TIME" ),
            ( "CpuCostStep2", " times-FTF ",           "expert-monitoring.root",    " --auto -p TIME -d TrigFastTrackFinder_" ),
            ( "CpuCostStep3", " cost-perCall ",        "TrigCostRoot_Results.root", " --auto -p _Time_perCall  -d /Algorithm  --logx " ),
            ( "CpuCostStep4", " cost-perEvent ",       "TrigCostRoot_Results.root", " --auto -p _Time_perEvent -d /Algorithm  --logx " ),
            ( "CpuCostStep5", " cost-perCall-chain ",  "TrigCostRoot_Results.root", " --auto -p _Time_perCall  -d /Chain_Algorithm  --logx " ),
            ( "CpuCostStep6", " cost-perEvent-chain ", "TrigCostRoot_Results.root", " --auto -p _Time_perEvent -d /Chain_Algorithm  --logx " ) ]
            

if ((not exclude) or postproc or costplot ):
    for job in cputest :
        cpucost = TrigInDetCpuCostStep( name=job[0], outdir=job[1], infile=job[2], extra=job[3] )
        test.check_steps.append(cpucost)


import sys
sys.exit(test.run())
