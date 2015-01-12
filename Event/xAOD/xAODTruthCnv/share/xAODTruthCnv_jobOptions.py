# Enable xAOD making:
from RecExConfig.RecFlags import rec
rec.doWritexAOD = True
rec.doTruth = True

printMCOutput = False
 
# Set up the reading of a file:
FNAME = "/afs/cern.ch/work/j/jcatmore/public/mc12_8TeV.117050.PowhegPythia_P2011C_ttbar.merge.AOD.e1728_s1975_s1776_r3925_r3549_tid01485606_00/AOD.01485606._002019.pool.root.1"
include( "AthenaPython/iread_file.py" )
 
# Access the algorithm sequence:
from AthenaCommon.AlgSequence import AlgSequence
theJob = AlgSequence()

# Run the xAOD truth builder
from xAODTruthCnv.xAODTruthCnvConf import xAODMaker__xAODTruthCnvAlg
alg = xAODMaker__xAODTruthCnvAlg()
# Pile up options... default is no pile-up
#alg.WriteInTimePileUpTruth = True
#alg.WriteAllPileUpTruth = False
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

