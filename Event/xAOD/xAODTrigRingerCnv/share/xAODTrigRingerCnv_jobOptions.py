
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
logger = logging.getLogger( "xAODTrigCaloCnv_jobOptions" )

# Create a POOL output file with the StoreGate contents:
from OutputStreamAthenaPool.MultipleStreamManager import MSMgr
xaodStream = MSMgr.NewPoolStream( "StreamAOD", "xAOD.pool.root" )

# Create the xAOD trigger calorimeter cluster:
from xAODTrigRingerCnv.xAODTrigRingerCreator import xAODTrigRingerCreator
xAODTrigRingerCreator()
alg = getattr( theJob, "xAODMaker::TrigRingerCnvAlg" )
alg.OutputLevel = DEBUG

# Create the xAOD trigger EM cluster:
from xAODTrigCaloCnv.xAODTrigRNNOutputCreator import xAODTrigRNNOutputCreator
xAODTrigRNNOutputCreator()
alg = getattr( theJob, "xAODMaker::TrigRNNOutputCnvAlg" )
alg.OutputLevel = DEBUG

# Additional type(s) created automatically by RecExCommon:
xaodStream.AddItem( "xAOD::TrigRingerContainer_v1#*" )
xaodStream.AddItem( "xAOD::TrigRingerAuxContainer_v1#*" )
xaodStream.AddItem( "xAOD::TrigRNNOutputContainer_v1#*" )
xaodStream.AddItem( "xAOD::TrigRNNOutputAuxContainer_v1#*" )

# Make sure that the event format object is added to the output:
theApp.CreateSvc += [ "xAODMaker::EventFormatSvc" ]
xaodStream.AddMetaDataItem( "xAOD::EventFormat_v1#EventFormat" )

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
