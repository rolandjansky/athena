#-----------------------------------------------------------------------------
# TAG
#-----------------------------------------------------------------------------

# for LAr EC timing stuff...
if rec.doLArg():
    include("LArCellRec/LArCollisionTime_jobOptions.py")


#Configure Algorithm to fill the tags 
from EventTagRawAlgs.EventTagRawAlgsConf import *
from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()
risftwriter = RawInfoSummaryForTagWriter()

if rec.doInDet():
    risftwriter.IDTrackKey = InDetKeys.Tracks()
    
#MBTS threshold
risftwriter.MBTS_Threshold = (60.0/222.0)
        
topSequence += risftwriter

    
# store the object in the ESD
from RecExConfig.ObjKeyStore import objKeyStore
objKeyStore.addStreamESD( "RawInfoSummaryForTag", "RawInfoSummaryForTag" )
objKeyStore.addStreamAOD( "RawInfoSummaryForTag", "RawInfoSummaryForTag" )



