# Ketevi A. Assamagan
# BNL, September 2009

include.block ("DPDUtils/ObjectThinner_jobOptions.py")

from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

from AthenaCommon.AppMgr import ToolSvc

from AthenaCommon.AppMgr import ServiceMgr as svcMgr

# make an instance of the thinning service
from AthenaServices.Configurables import ThinningSvc
svcMgr += ThinningSvc()

# the output stream(s) to apply thinning on
svcMgr.ThinningSvc.Streams += ['StreamDPD']

# object selection tools - for the details selections of the objects to pass to the DPD 
include ( "UserAnalysisUtils/UserAnalysisSelectionTool_jobOptions.py" )

from DPDUtils.DPDUtilsConf import ObjectThinner
ObjectThinner=ObjectThinner(
     ThinService             = svcMgr.ThinningSvc,
     AnalysisSelectionTool   = ToolSvc.UserAnalysisSelectionTool,
     InDetTrackParticles     = "TrackParticleCandidate",
     ObjectContainers        = [ "ElectronAODCollection", "PhotonAODCollection", "StacoMuonCollection", "TauRecContainer","Cone4H1TopoJets" ],
     DoObjectThinning        = [ True, True, True, True, True ],
     CaloClusters            = [ "egClusterCollection", "egClusterCollection", "LArClusterEMSofte", "MuonClusterCollection", "EMTopoCluster430","CaloCalTopoCluster" ],
     MuonSPTrackParticles    = [ "MuonboyTrackParticles", "StacoTrackParticles", "MuTagTrackParticles", "MuonboyMuonSpectroOnlyTrackParticles" ],
     NumberOfPreSelectedMuons     = 1,
     NumberOfPreSelectedElectrons = 1,
     DoSkimming                   = True )
topSequence += ObjectThinner

####### If the data contains the Truth, then do this, uncomment this lines 
# Make Truth Jet on the Fly if there is truth in this data
#from JetRec.JetGetters import *
#from JetRec.JetRecFlags import jetFlags
#jetFlags.inputFileType = 'GEN'
#make_StandardJetGetter('Cone',0.4,'Truth').jetAlgorithmHandle()

# Slim the McEventCollection
#include ( "DPDUtils/TruthSlimming.py" )
#################### End the treatment of the Truth ########

from AthenaPoolCnvSvc.WriteAthenaPool import AthenaPoolOutputStream
StreamDPD = AthenaPoolOutputStream("StreamDPD")
StreamDPD.OutputFile = "D2PD.pool.root"

#Metadata and luminosity block
StreamDPD.ItemList += ["IOVMetaDataContainer#*"]
StreamDPD.ItemList += ["LumiBlockCollection#*"]

#Event Level
StreamDPD.ItemList += ['EventInfo#*']
StreamDPD.ItemList += ['PileUpEventInfo#*']
StreamDPD.ItemList += ['EventStreamInfo#*']
StreamDPD.ItemList += ['MergedEventInfo#*']

StreamDPD.ItemList += [ "TrigDec::TrigDecision#TrigDecision" ]
StreamDPD.ItemList += [ "CTP_Decision#CTP_Decision" ]

# Inner Detector TrackParticles - 
# for Inner detector TrackParticles associated to Electrons/Photons/Muons/TauJets
StreamDPD.ItemList += ['Rec::TrackParticleContainer#TrackParticleCandidate']

#egammas and soft-electrons
StreamDPD.ItemList += ['ElectronContainer#ElectronAODCollection']
StreamDPD.ItemList += ['PhotonContainer#PhotonAODCollection']
StreamDPD.ItemList += ['CaloClusterContainer#egClusterCollection']
StreamDPD.ItemList += ['CaloClusterContainer#LArClusterEMSofte']

# muons
StreamDPD.ItemList += ['Analysis::MuonContainer#StacoMuonCollection']
StreamDPD.ItemList += ['CaloClusterContainer#MuonClusterCollection']
StreamDPD.ItemList += [ "Rec::TrackParticleContainer#MuonboyTrackParticles" ] 
StreamDPD.ItemList += [ "Rec::TrackParticleContainer#StacoTrackParticles" ]
StreamDPD.ItemList += [ "Rec::TrackParticleContainer#MuTagTrackParticles"  ]
StreamDPD.ItemList += [ "Rec::TrackParticleContainer#MuonboyMuonSpectroOnlyTrackParticles" ]

# tau jets
StreamDPD.ItemList += ['Analysis::TauJetContainer#TauRecContainer']
StreamDPD.ItemList += ['CaloClusterContainer#EMTopoCluster430']

# Jets - save also the truth jets - or rerun the truth jet in AOD -> DPD
StreamDPD.ItemList += ['JetKeyDescriptor#JetKeyMap']
StreamDPD.ItemList += ['JetCollection#Cone4H1TopoJets']
 
# missing Et - save as well the truth missing et
StreamDPD.ItemList += ['MissingET#MET_RefFinal']
   
# truth
StreamDPD.ItemList += [ 'TruthParticleContainer#SpclMC']
StreamDPD.ItemList += [ 'McEventCollection#GEN_AOD']
StreamDPD.ItemList += ['JetCollection#Cone4TruthJets']

# do the skimming
#StreamDPD.ForceRead=True  #force read of output data objs
StreamDPD.AcceptAlgs += ["ObjectThinner"]

