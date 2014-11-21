# $Id: xAODTrigL1CaloCnv_jobOptions.py 575898 2013-12-17 13:58:54Z morrisj $

# Set up the reading of a file:
FNAME = "ESD.pool.root"
include( "AthenaPython/iread_file.py" )

# Access the algorithm sequence:
from AthenaCommon.AlgSequence import AlgSequence
theJob = AlgSequence()

# Create a POOL output file with the StoreGate contents:
from OutputStreamAthenaPool.MultipleStreamManager import MSMgr
xaodStream = MSMgr.NewPoolStream( "StreamXAOD", "xAOD.pool.root" )

# Create the xAOD RoIs:
from xAODTrigL1CaloCnv.xAODTrigL1CaloCreator import xAODTrigL1CaloCreator
xAODTrigL1CaloCreator()

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
