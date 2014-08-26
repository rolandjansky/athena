# Enable xAOD making:
from RecExConfig.RecFlags import rec
rec.doWritexAOD = True
rec.doTruth = True

printMCOutput = False
 
# Set up the reading of a file:
FNAME = "/afs/cern.ch/work/j/jcatmore/public/mc12_8TeV.208002.Pythia8B_AU2_CTEQ6L1_pp_Jpsimu6mu6.merge.AOD.e1331_a159_a173_r3549_tid00862287_00/AOD.00862287._000853.pool.root.1"
include( "AthenaPython/iread_file.py" )
 
# Access the algorithm sequence:
from AthenaCommon.AlgSequence import AlgSequence
theJob = AlgSequence()

# Run the xAOD truth builder
from xAODTruthCnv.xAODTruthCnvConf import xAODMaker__xAODTruthCnvAlg
alg = xAODMaker__xAODTruthCnvAlg()
alg.OutputLevel = INFO  
theJob += alg


# Output
from OutputStreamAthenaPool.MultipleStreamManager import MSMgr
xaodStream = MSMgr.NewPoolStream( "StreamAOD", "xAOD.pool.root" )
# Additional type(s) created automatically by RecExCommon:
xaodStream.AddItem( "xAOD::EventInfo_v1#EventInfo" )
xaodStream.AddItem( "xAOD::EventAuxInfo_v1#EventInfoAux." )
#xaodStream.AddItem( "xAOD::CaloClusterContainer_v1#*" )
#xaodStream.AddItem( "xAOD::CaloClusterAuxContainer_v1#*" )
#xaodStream.AddItem( "xAOD::MuonContainer_v1#*" )
#xaodStream.AddItem( "xAOD::MuonAuxContainer_v1#*" )
#xaodStream.AddItem( "xAOD::TrackParticleContainer_v1#*" )
#xaodStream.AddItem( "xAOD::TrackParticleAuxContainer_v1#*" )
#xaodStream.AddItem( "xAOD::VertexContainer_v1#*" )
#xaodStream.AddItem( "xAOD::VertexAuxContainer_v1#*" )
#xaodStream.AddItem( "xAOD::TauJetContainer_v1#*" )
#xaodStream.AddItem( "xAOD::TauJetAuxContainer_v1#*" )
xaodStream.AddItem( "xAOD::TruthEventContainer_v1#*" )
xaodStream.AddItem( "xAOD::TruthEventAuxContainer_v1#*" )
xaodStream.AddItem( "xAOD::TruthVertexContainer_v1#*" )
xaodStream.AddItem( "xAOD::TruthVertexAuxContainer_v1#*" )
xaodStream.AddItem( "xAOD::TruthParticleContainer_v1#*" ) 
xaodStream.AddItem( "xAOD::TruthParticleAuxContainer_v1#*" )


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

# Verbose output
if printMCOutput:
	from xAODTruthCnv.xAODTruthCnvConf import xAODReader__xAODTruthReader
	alg2 = xAODReader__xAODTruthReader()
	alg2.OutputLevel = INFO
	theJob += alg2

# Do some additional tweaking:
from AthenaCommon.AppMgr import theApp
theApp.EvtMax = 2
ServiceMgr.MessageSvc.OutputLevel = INFO
ServiceMgr.MessageSvc.defaultLimit = 1000000
ServiceMgr.AthenaSealSvc.OutputLevel = VERBOSE

