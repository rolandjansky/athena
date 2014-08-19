# $Id: xAODEventInfoCnv_jobOptions.py 594319 2014-04-25 17:39:50Z krasznaa $

# Set up the reading of a file:
FNAME = "AOD.pool.root"
include( "AthenaPython/iread_file.py" )

# Access the algorithm sequence:
from AthenaCommon.AlgSequence import AlgSequence
theJob = AlgSequence()

# Create a POOL output file with the StoreGate contents:
from OutputStreamAthenaPool.MultipleStreamManager import MSMgr
xaodStream = MSMgr.NewPoolStream( "StreamXAOD", "xAOD.pool.root" )

# Add EventFormat information to the file:
theApp.CreateSvc += [ "xAODMaker::EventFormatSvc" ]
xaodStream.AddMetaDataItem( "xAOD::EventFormat_v1#EventFormat" )

# Create the xAOD EventInfo object:
from xAODEventInfoCnv.xAODEventInfoCreator import xAODEventInfoCreator
xAODEventInfoCreator()

# Check what happened to the stream:
xaodStream.Print()

# Split all branches:
ServiceMgr.AthenaPoolCnvSvc.PoolAttributes += [
    "DEFAULT_SPLITLEVEL='1'" ]

# Force POOL to just simply use the StoreGate keys as branch names:
ServiceMgr.AthenaPoolCnvSvc.SubLevelBranchName = "<key>"

# Do some additional tweaking:
from AthenaCommon.AppMgr import theApp
theApp.EvtMax = -1
ServiceMgr.MessageSvc.OutputLevel = INFO
ServiceMgr.MessageSvc.defaultLimit = 1000000
