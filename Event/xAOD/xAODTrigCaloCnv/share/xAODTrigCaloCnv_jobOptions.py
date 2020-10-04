# $Id: xAODTrigCaloCnv_jobOptions.py 592334 2014-04-10 11:01:56Z krasznaa $

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
from xAODTrigCaloCnv.xAODTrigCaloClusterCreator import xAODTrigCaloClusterCreator
xAODTrigCaloClusterCreator()
alg = getattr( theJob, "xAODMaker::TrigCaloClusterCnvAlg" )
alg.OutputLevel = DEBUG

# Create the xAOD trigger EM cluster:
from xAODTrigCaloCnv.xAODTrigEMClusterCreator import xAODTrigEMClusterCreator
xAODTrigEMClusterCreator()
alg = getattr( theJob, "xAODMaker::TrigEMClusterCnvAlg" )
alg.OutputLevel = DEBUG

# Additional type(s) created automatically by RecExCommon:
xaodStream.AddItem( "xAOD::TrigCaloClusterContainer_v1#*" )
xaodStream.AddItem( "xAOD::TrigCaloClusterAuxContainer_v1#*" )
xaodStream.AddItem( "xAOD::TrigEMClusterContainer_v1#*" )
xaodStream.AddItem( "xAOD::TrigEMClusterAuxContainer_v1#*" )

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
