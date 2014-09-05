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

outStream = AthenaPoolOutputStream("StreamEVGEN", runArgs.outputEVNT_MRGFile)

## copy everything from the input file
## must force reading of all input objects
outStream.TakeItemsFromInput = True
outStream.ForceRead          = True
########## EOF ###############



