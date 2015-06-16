# Set up the reading of the input xAOD:
import AthenaPoolCnvSvc.ReadAthenaPool
svcMgr.EventSelector.InputCollections = ["xAOD.pool.root"]
 
# Access the algorithm sequence:
from AthenaCommon.AlgSequence import AlgSequence
theJob = AlgSequence()
 
from DerivationFrameworkMCTruth.DerivationFrameworkMCTruthConf import DerivationFramework__xAODtoHepMCCnvAlg
alg2 = DerivationFramework__xAODtoHepMCCnvAlg()
alg2.OutputLevel = INFO
theJob += alg2
 
# Do some additional tweaking:
from AthenaCommon.AppMgr import theApp
theApp.EvtMax = -1
ServiceMgr.MessageSvc.OutputLevel = INFO
ServiceMgr.MessageSvc.defaultLimit = 1000000

# Output
from OutputStreamAthenaPool.MultipleStreamManager import MSMgr
hepmcStream = MSMgr.NewPoolStream( "StreamAOD", "test.pool.root" )
# Additional type(s) created automatically by RecExCommon:
hepmcStream.AddItem( "McEventCollection#McEventCollectionFromXAOD" )
hepmcStream.AddItem( "xAOD::TruthParticleContainer_v1#TruthParticle")
hepmcStream.AddItem( "xAOD::TruthParticleAuxContainer_v1#TruthParticleAux.")
hepmcStream.AddItem( "xAOD::TruthEventContainer_v1#TruthEvent")
hepmcStream.AddItem( "xAOD::TruthEventAuxContainer_v1#TruthEventAux.")
hepmcStream.AddItem( "xAOD::TruthVertexContainer_v1#TruthVertex")
hepmcStream.AddItem( "xAOD::TruthVertexAuxContainer_v1#TruthVertexAux.")
