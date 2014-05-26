###############################################################
#
#==============================================================


import logging
eiLog = logging.getLogger('pool_to_ei')
eiLog.info( '****************** STARTING POOL->EI MAKING *****************' )


## Input

if hasattr(runArgs,"inputPOOLFile"):
    In=runArgs.inputPOOLFile
    if type(In) == type(''):
        In = [In]

## Output

if hasattr(runArgs,"outputEIFile"):
    Out=runArgs.outputEIFile

# options

if hasattr(runArgs,"maxEvents"):
    EvtMax=runArgs.maxEvents

if hasattr(runArgs,"trigger"):
    DoTriggerInfo = runArgs.trigger
else:
    DoTriggerInfo = False

if hasattr(runArgs,"provenance"):
    DoProvenanceRef = runArgs.provenance
else:
    DoProvenanceRef = True


# load joboptions
#include("PyAnalysisCore/InitPyAnalysisCore.py")
include('EventIndexProducer/POOL2EI_joboptions.py')


## Pre-exec
if hasattr(runArgs,"preExec"):
    eiLog.info("transform pre-exec")
    for cmd in runArgs.preExec:
        eiLog.info(cmd)
        exec(cmd)

## Pre-include
if hasattr(runArgs,"preInclude"): 
    for fragment in runArgs.preInclude:
        include(fragment)

## Post-include
if hasattr(runArgs,"postInclude"): 
    for fragment in runArgs.postInclude:
        include(fragment)

## Post-exec
if hasattr(runArgs,"postExec"):
    eiLog.info("transform post-exec")
    for cmd in runArgs.postExec:
        eiLog.info(cmd)
        exec(cmd)
        
