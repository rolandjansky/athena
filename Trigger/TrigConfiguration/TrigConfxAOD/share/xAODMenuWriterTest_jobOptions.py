# $Id: xAODMenuWriterTest_jobOptions.py 582808 2014-02-12 16:05:44Z krasznaa $

# This jobO is meant to test the ability of the code to write an xAOD file
# with ROOT-readable trigger configuration in it.


# Make xAOD objects using RecExCommon:
from RecExConfig.RecFlags import rec
rec.doWritexAOD = True

# Set up the reading of a file:
FNAME = "AOD.pool.root"
include( "AthenaPython/iread_file.py" )

# Access the algorithm sequence:
from AthenaCommon.AlgSequence import AlgSequence
theJob = AlgSequence()

# Create a POOL output file with the StoreGate contents:
from OutputStreamAthenaPool.MultipleStreamManager import MSMgr
xaodStream = MSMgr.NewPoolStream( "StreamAOD", "xAOD.pool.root" )

# Add the trigger menu conversion algorithm:
from TrigConfxAOD.TrigConfxAODConf import TrigConf__xAODMenuWriter
alg = TrigConf__xAODMenuWriter()
#alg.OutputLevel = VERBOSE
theJob += alg

# Add the objects created by the algorithm to the output stream:
xaodStream.AddItem( "xAOD::TrigConfKeys_v1#TrigConfKeys" )
xaodStream.AddMetaDataItem( "xAOD::TriggerMenuContainer_v1#TriggerMenu" )
xaodStream.AddMetaDataItem( "xAOD::TriggerMenuAuxContainer_v1#TriggerMenuAux." )

# Add the (auto-generated) EventInfo object to the output as well:
xaodStream.AddItem( "xAOD::EventInfo_v1#EventInfo" )
xaodStream.AddItem( "xAOD::EventAuxInfo_v1#EventInfoAux." )

# Add the (auto-generated) xAOD::CaloCluster objects:
xaodStream.AddItem( "xAOD::CaloClusterContainer_v1#*" )
xaodStream.AddItem( "xAOD::CaloClusterAuxContainer_v1#*" )

# Add the (auto-generated) xAOD::Muon objects:
xaodStream.AddItem( "xAOD::MuonContainer_v1#*" )
xaodStream.AddItem( "xAOD::MuonAuxContainer_v1#*" )

# Add the (auto-generated) xAOD::TauJet objects:
xaodStream.AddItem( "xAOD::TauJetContainer_v1#*" )
xaodStream.AddItem( "xAOD::TauJetAuxContainer_v1#*" )

# Add the (auto-generated) truth objects:
xaodStream.AddItem( "xAOD::TruthParticleContainer_v1#*" )
xaodStream.AddItem( "xAOD::TruthParticleAuxContainer_v1#*" )
xaodStream.AddItem( "xAOD::TruthVertexContainer_v1#*" )
xaodStream.AddItem( "xAOD::TruthVertexAuxContainer_v1#*" )
xaodStream.AddItem( "xAOD::TruthEventContainer_v1#*" )
xaodStream.AddItem( "xAOD::TruthEventAuxContainer_v1#*" )

# Convert the LVL1 information:
from xAODTriggerCnv.xAODRoICreator import xAODRoICreator
xAODRoICreator()

# Convert the trigger decision:
from xAODTriggerCnv.xAODTrigDecisionCreator import xAODTrigDecisionCreator
xAODTrigDecisionCreator()

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
#ServiceMgr.StoreGateSvc.Dump = True
