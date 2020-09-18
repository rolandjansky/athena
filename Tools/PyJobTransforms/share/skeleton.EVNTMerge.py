#############################
## basic jobO configuration
include("PATJobTransforms/CommonSkeletonJobOptions.py")
## load pool support
import AthenaPoolCnvSvc.ReadAthenaPool
import AthenaPoolCnvSvc.WriteAthenaPool

## input
ServiceMgr.EventSelector.InputCollections = runArgs.inputEVNTFile

## output stream
from AthenaPoolCnvSvc.WriteAthenaPool import AthenaPoolOutputStream

outStream = AthenaPoolOutputStream("StreamEVGEN", runArgs.outputEVNT_MRGFile, noTag=True)

## copy everything from the input file
## must force reading of all input objects
outStream.TakeItemsFromInput = True


#==============================================================
# Job Configuration parameters:
#==============================================================
## Pre-exec
if hasattr(runArgs, "preExec"):
    for cmd in runArgs.preExec:
        exec(cmd)
        
## Pre-include
if hasattr(runArgs, "preInclude"):
    for fragment in runArgs.preInclude:
        include(fragment)

# Avoid command line preInclude for Event Service
if hasattr(runArgs, "eventService") and runArgs.eventService:
    include('AthenaMP/AthenaMP_EventService.py')
        
## Post-include
if hasattr(runArgs, "postInclude"):
    for fragment in runArgs.postInclude:
        include(fragment)

## Post-exec
if hasattr(runArgs, "postExec"):
    for cmd in runArgs.postExec:
        exec(cmd)
              
########## EOF ###############



