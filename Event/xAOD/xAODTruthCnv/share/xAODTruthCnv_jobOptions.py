# Enable xAOD making:
from RecExConfig.RecFlags import rec
rec.doWritexAOD = True
rec.doTruth = True

printMCOutput = False
 
# Set up the reading of a file:
FNAME = "/afs/cern.ch/atlas/project/PAT/data/mc11/mc11_7TeV.106047.PythiaZmumu_no_filter.recon.AOD.e815_s1272_s1274_r2730_tid516371_00/AOD.516371._000209.pool.root.1"
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
xaodStream = MSMgr.NewPoolRootStream( "StreamAOD", "xAOD.pool.root" )

# Additional type(s) created automatically by RecExCommon:
xaodStream.AddItem( "xAOD::EventInfo_v1#EventInfo" )
xaodStream.AddItem( "xAOD::EventAuxInfo_v1#EventInfoAux." )

# The types created explicitly by this job:
xaodStream.AddItem( "xAOD::TruthEventContainer#*" )
xaodStream.AddItem( "xAOD::TruthEventAuxContainer#*" )
xaodStream.AddItem( "xAOD::TruthPileupEventContainer#*" )
xaodStream.AddItem( "xAOD::TruthPileupEventAuxContainer#*" )
xaodStream.AddItem( "xAOD::TruthVertexContainer#*" )
xaodStream.AddItem( "xAOD::TruthVertexAuxContainer#*" )
xaodStream.AddItem( "xAOD::TruthParticleContainer#*" ) 
xaodStream.AddItem( "xAOD::TruthParticleAuxContainer#*" )
 
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
theApp.EvtMax = 2
ServiceMgr.MessageSvc.OutputLevel = INFO
ServiceMgr.MessageSvc.defaultLimit = 1000000
ServiceMgr.AthenaSealSvc.OutputLevel = VERBOSE

