# $Id: xAODEventInfoCnv_jobOptions.py 682548 2015-07-13 13:57:15Z krasznaa $

# Set up the reading of a file:
FNAME = "AOD.pool.root"
import AthenaPoolCnvSvc.ReadAthenaPool
ServiceMgr.EventSelector.InputCollections = [ FNAME ]

# Access the algorithm sequence:
from AthenaCommon.AlgSequence import AlgSequence
theJob = AlgSequence()

# Create a POOL output file with the StoreGate contents:
from OutputStreamAthenaPool.MultipleStreamManager import MSMgr
xaodStream = MSMgr.NewPoolRootStream( "StreamXAOD", "xAOD.pool.root" )

# Create the xAOD EventInfo object:
from xAODEventInfoCnv.xAODEventInfoCreator import xAODEventInfoCreator
xAODEventInfoCreator( outkey = "EventInfoTest" )

# Check what happened to the stream:
xaodStream.Print()

# Do some additional tweaking:
from AthenaCommon.AppMgr import theApp
theApp.EvtMax = 10
ServiceMgr.MessageSvc.OutputLevel = INFO
ServiceMgr.MessageSvc.defaultLimit = 1000000
