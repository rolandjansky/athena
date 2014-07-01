#=================================================================
#
# JobOption for writing out a BS file with all the LVL1
# information. This is just intended as an example and for usage
# while developing/modifying the converters.
#
#=================================================================

#
# User configuration:
#
RDOInput = [ "/scratch/krasznaa/top_rdo/RDO.017303._00028.pool.root.1",
             "/scratch/krasznaa/top_rdo/RDO.017303._00037.pool.root.1" ]
EvtMax = 100

#
# Create the necessary Python objects:
#
from AthenaCommon.Logging import logging
log = logging.getLogger( "WriteLVL1BS" )

from AthenaCommon.AppMgr import ServiceMgr
from AthenaCommon.AppMgr import ToolSvc

#
# Set up reading of the POOL RDO file(s):
#
include( "AthenaPoolCnvSvc/ReadAthenaPool_jobOptions.py" )
ServiceMgr.EventSelector.InputCollections = RDOInput
log.info( "Set input POOL file" )

#
# Load the dinctionaries and converters for the LVL1 objects:
#
include( "TrigT1Result/TrigT1ResultDict_joboptions.py" )
include( "TrigT1EventAthenaPool/TrigT1EventAthenaPool_joboptions.py" )

#
# Configure the output BS file:
#
include( "ByteStreamCnvSvc/WriteByteStream_EventStorage_jobOptions.py" )
ServiceMgr.ByteStreamEventStorageOutputSvc.FileTag = "lvl1test"

oStream.ForceRead = True
oStream.ItemList += [ "ROIB::RoIBResult#*" ]
oStream.ItemList += [ "CTP_RDO#*" ]
oStream.ItemList += [ "MuCTPI_RDO#*" ]
log.info( "Configured output BS file" )

#
# Additional configuration:
#
theApp.EvtMax = EvtMax
ServiceMgr.MessageSvc.OutputLevel = 3
ServiceMgr.MessageSvc.Format = "% F%50W%S%7W%R%T %0W%M"
#ToolSvc.OutputLevel = 2
