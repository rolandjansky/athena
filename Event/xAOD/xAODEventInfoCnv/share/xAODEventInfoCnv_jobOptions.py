# $Id: xAODEventInfoCnv_jobOptions.py 631751 2014-11-28 09:59:20Z krasznaa $

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
xAODEventInfoCreator()

# Check what happened to the stream:
xaodStream.Print()

# Do some additional tweaking:
from AthenaCommon.AppMgr import theApp
theApp.EvtMax = -1
ServiceMgr.MessageSvc.OutputLevel = INFO
ServiceMgr.MessageSvc.defaultLimit = 1000000
