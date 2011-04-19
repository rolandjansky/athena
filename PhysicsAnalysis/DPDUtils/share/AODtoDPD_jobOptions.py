from AthenaCommon.AppMgr import ServiceMgr as svcMgr
from AthenaServices.Configurables import ThinningSvc
svcMgr += ThinningSvc()

include( "PyAnalysisCore/InitPyAnalysisCore.py")
include( "DPDUtils/ThinningSvcWrapper_jobOptions.py" )

from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

from AthenaPoolCnvSvc.WriteAthenaPool import AthenaPoolOutputStream
topSequence += AthenaPoolOutputStream( "StreamDPD" )
StreamDPD=topSequence.StreamDPD

StreamDPD.ItemList =  ['EventInfo#*', 'TrackRecordCollection#*']
StreamDPD.ItemList += ['ElectronContainer#ElectronAODCollection']
StreamDPD.ItemList += ['egDetailContainer#egDetailAOD']
StreamDPD.ItemList += ['MissingET#MET_RefFinal']
StreamDPD.ItemList += ['MissingET#MET_LocHadObj']
StreamDPD.ItemList += ['CaloClusterContainer#CaloCalTopoCluster']
StreamDPD.ItemList += ['Analysis::MuonContainer#StacoMuonCollection']
StreamDPD.ItemList += ['JetKeyDescriptor#JetKeyMap']
StreamDPD.ItemList += ['JetCollection#Kt4H1TopoJets']
StreamDPD.ItemList += ['Rec::TrackParticleContainer#TrackParticleCandidate']
StreamDPD.ItemList += ['Rec::TrackParticleContainer#StacoTrackParticles']
StreamDPD.ItemList += ['TruthParticleContainer#SpclMC']
StreamDPD.ItemList += ['McEventCollection#GEN_AOD']

StreamDPD.WritingTool = "AthenaPoolOutputStreamTool" 
StreamDPD.ForceRead = True
StreamDPD.OutputFile = "DPD.pool.root"

