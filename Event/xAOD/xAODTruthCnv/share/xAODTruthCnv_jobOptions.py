printMCOutput = False

import AthenaPoolCnvSvc.ReadAthenaPool
svcMgr.EventSelector.InputCollections = ["evnt.pool.root"]

# Access the algorithm sequence:
from AthenaCommon.AlgSequence import AlgSequence
theJob = AlgSequence()

# Create the xAOD::EventInfo object:
theJob += CfgMgr.xAODMaker__EventInfoCnvAlg( "EventInfoCnvAlg" )

# Run the xAOD truth builder
from xAODTruthCnv.xAODTruthCnvConf import xAODMaker__xAODTruthCnvAlg
alg = xAODMaker__xAODTruthCnvAlg()
# Pile up options... default is no pile-up
alg.WriteInTimePileUpTruth = False
alg.WriteAllPileUpTruth = False
alg.AODContainerName = "GEN_EVENT"
alg.OutputLevel = INFO  
theJob += alg

# Output
from OutputStreamAthenaPool.MultipleStreamManager import MSMgr
xaodStream = MSMgr.NewPoolRootStream( "StreamAOD", "xAOD.pool.root" )

# Additional type(s) created automatically by RecExCommon:
xaodStream.AddItem( "xAOD::EventInfo#EventInfo" )
xaodStream.AddItem( "xAOD::EventAuxInfo#EventInfoAux." )

# The types created explicitly by this job:
xaodStream.AddItem( "xAOD::TruthEventContainer#*" )
xaodStream.AddItem( "xAOD::TruthEventAuxContainer#*" )
xaodStream.AddItem( "xAOD::TruthPileupEventContainer#*" )
xaodStream.AddItem( "xAOD::TruthPileupEventAuxContainer#*" )
xaodStream.AddItem( "xAOD::TruthVertexContainer#*" )
xaodStream.AddItem( "xAOD::TruthVertexAuxContainer#*" )
xaodStream.AddItem( "xAOD::TruthParticleContainer#*" ) 
xaodStream.AddItem( "xAOD::TruthParticleAuxContainer#*" )

xaodStream.AddMetaDataItem( [ "xAOD::TruthMetaDataContainer#TruthMetaData", "xAOD::TruthMetaDataAuxContainer#TruthMetaDataAux." ] )

# Check what happened to the stream:
xaodStream.Print()

# Verbose output
if printMCOutput:
	from xAODTruthCnv.xAODTruthCnvConf import xAODReader__xAODTruthReader
	alg2 = xAODReader__xAODTruthReader()
	alg2.OutputLevel = INFO
	theJob += alg2

# Do some additional tweaking:
from AthenaCommon.AppMgr import theApp
theApp.EvtMax = -1
ServiceMgr.MessageSvc.OutputLevel = INFO
ServiceMgr.MessageSvc.defaultLimit = 1000000
