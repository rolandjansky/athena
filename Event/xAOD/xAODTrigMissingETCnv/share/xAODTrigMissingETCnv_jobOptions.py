# $Id: xAODTrigMissingETCnv_jobOptions.py 592539 2014-04-11 10:17:29Z krasznaa $

# Enable xAOD making:
from RecExConfig.RecFlags import rec
rec.doWritexAOD = True

# Set up the reading of a file:
FNAME = "AOD.pool.root"
include( "AthenaPython/iread_file.py" )

# Access the algorithm sequence:
from AthenaCommon.AlgSequence import AlgSequence
theJob = AlgSequence()

# Create a logger object:
from AthenaCommon.Logging import logging
logger = logging.getLogger( "xAODTrigMissingETCnv_jobOptions" )

# Create a POOL output file with the StoreGate contents:
from OutputStreamAthenaPool.MultipleStreamManager import MSMgr
xaodStream = MSMgr.NewPoolStream( "StreamAOD", "xAOD.pool.root" )

# Create some xAOD::TrigMissingET objects:
from xAODTrigMissingETCnv.xAODTrigMissingETCreator import \
    xAODTrigMissingETCreator
xAODTrigMissingETCreator()

# Check what happened to the stream:
xaodStream.Print()

# Split all branches:
ServiceMgr.AthenaPoolCnvSvc.PoolAttributes += [
    "DEFAULT_SPLITLEVEL='1'" ]

# Force POOL to just simply use the StoreGate keys as branch names:
ServiceMgr.AthenaPoolCnvSvc.SubLevelBranchName = "<key>"

# Do some additional tweaking:
from AthenaCommon.AppMgr import theApp
theApp.EvtMax = 100
ServiceMgr.MessageSvc.OutputLevel = INFO
ServiceMgr.MessageSvc.defaultLimit = 1000
