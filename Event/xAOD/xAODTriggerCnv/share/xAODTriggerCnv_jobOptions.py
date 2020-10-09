# $Id: xAODTriggerCnv_jobOptions.py 583381 2014-02-14 15:58:20Z krasznaa $

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
logger = logging.getLogger( "xAODTriggerCnv_jobOptions" )

# Create a POOL output file with the StoreGate contents:
from OutputStreamAthenaPool.MultipleStreamManager import MSMgr
xaodStream = MSMgr.NewPoolStream( "StreamAOD", "xAOD.pool.root" )

# Create the xAOD RoIs:
from xAODTriggerCnv.xAODRoICreator import xAODRoICreator
xAODRoICreator()

# Create the xAOD trigger decision:
from xAODTriggerCnv.xAODTrigDecisionCreator import xAODTrigDecisionCreator
xAODTrigDecisionCreator()

# Add bunch configuration metadata to the file:
from xAODTriggerCnv.xAODBunchConfCreator import xAODBunchConfCreator
xAODBunchConfCreator()

# Additional type(s) created automatically by RecExCommon:
xaodStream.AddItem( "xAOD::EventInfo_v1#EventInfo" )
xaodStream.AddItem( "xAOD::EventAuxInfo_v1#EventInfoAux." )
xaodStream.AddItem( "xAOD::CaloClusterContainer_v1#*" )
xaodStream.AddItem( "xAOD::CaloClusterAuxContainer_v1#*" )
xaodStream.AddItem( "xAOD::MuonContainer_v1#*" )
xaodStream.AddItem( "xAOD::MuonAuxContainer_v1#*" )
xaodStream.AddItem( "xAOD::TrackParticleContainer_v1#*" )
xaodStream.AddItem( "xAOD::TrackParticleAuxContainer_v1#*" )
xaodStream.AddItem( "xAOD::VertexContainer_v1#*" )
xaodStream.AddItem( "xAOD::VertexAuxContainer_v1#*" )
xaodStream.AddItem( "xAOD::TauJetContainer_v1#*" )
xaodStream.AddItem( "xAOD::TauJetAuxContainer_v1#*" )

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
