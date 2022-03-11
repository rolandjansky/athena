#################
### Steering options
#################
## Load common flags
from AthenaCommon.JobProperties import jobproperties as athCommonFlags

## Steer output file
from OutputStreamAthenaPool.MultipleStreamManager import MSMgr
from PrimaryDPDMaker.PrimaryDPDHelpers import buildFileName
from PrimaryDPDMaker.PrimaryDPDFlags import primDPD
streamName = primDPD.WriteDAOD_L1CALO1.StreamName
fileName   = buildFileName( primDPD.WriteDAOD_L1CALO1 )
L1CALO1Stream = MSMgr.NewPoolRootStream( streamName, fileName )
L1CALO1Stream.AcceptAlgs(["DFL1CALO1_KERN"])
augStream = MSMgr.GetStream( streamName )
evtStream = augStream.GetEventStream()

#################
### Setup Augmentation tools
#################
augmentationTools=[]

from AthenaCommon import CfgMgr

# Set up stream auditor
from AthenaCommon.AppMgr import ServiceMgr as svcMgr
if not hasattr(svcMgr, "DecisionSvc"):
        svcMgr += CfgMgr.DecisionSvc()
svcMgr.DecisionSvc.CalcStats = True


#====================================================================
# Skimming Tools
#====================================================================
skimmingTools = []

#====================================================================
# Thinning Tools
#====================================================================

thinningTools = []

from TrigT1CaloCalibTools.TrigT1CaloCalibToolsConf import DerivationFramework__TriggerTowerThinningAlg
L1CALO1CaloThinningTool = DerivationFramework__TriggerTowerThinningAlg( name = "L1CALO1CaloThinningTool",
                                                                        StreamName              = streamName,
    									TriggerTowerLocation = "xAODTriggerTowers",
    									MinCaloCellET = 0.8,
    									MinADC = 36,
    									UseRandom = True,
    									MinRandom = 0.01)
ToolSvc += L1CALO1CaloThinningTool
thinningTools.append(L1CALO1CaloThinningTool)


#====================================================================
# Create the derivation Kernel and setup output stream
#====================================================================
# Add the derivation job to the top AthAlgSeqeuence
# DerivationJob is COMMON TO ALL DERIVATIONS
DerivationFrameworkJob = CfgMgr.AthSequencer("L1CALO1Seq")
from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__CommonAugmentation
DerivationFrameworkJob += CfgMgr.DerivationFramework__DerivationKernel("DFL1CALO1_KERN",
                                                                       AugmentationTools = augmentationTools,
                                                                       SkimmingTools = skimmingTools,
                                                                       ThinningTools = thinningTools)

topSequence += DerivationFrameworkJob
print(DerivationFrameworkJob)
print(DerivationFrameworkJob.properties())

#################
### Steer output file content
#################
## Add service for metadata
ToolSvc += CfgMgr.xAODMaker__TriggerMenuMetaDataTool(
"TriggerMenuMetaDataTool" )
svcMgr.MetaDataSvc.MetaDataTools += [ ToolSvc.TriggerMenuMetaDataTool ]


trackParticleAuxExclusions="-caloExtension.-cellAssociation.-clusterAssociation.-trackParameterCovarianceMatrices.-parameterX.-parameterY.-parameterZ.-parameterPX.-parameterPY.-parameterPZ.-parameterPosition"

# Generic event info
L1CALO1Stream.AddItem("xAOD::EventInfo#*")
L1CALO1Stream.AddItem("xAOD::EventAuxInfo#*")
L1CALO1Stream.AddItem("xAOD::EventShape#Kt4EMTopoOriginEventShape")
L1CALO1Stream.AddItem("xAOD::EventShapeAuxInfo#Kt4EMTopoOriginEventShapeAux.")

#Physics Objects
L1CALO1Stream.AddItem("xAOD::JetContainer#AntiKt4EMTopoJets")
L1CALO1Stream.AddItem("xAOD::JetAuxContainer#AntiKt4EMTopoJetsAux.")
L1CALO1Stream.AddItem("xAOD::MuonContainer#Muons")
L1CALO1Stream.AddItem("xAOD::MuonAuxContainer#MuonsAux.")
L1CALO1Stream.AddItem("xAOD::ElectronContainer#Electrons")
L1CALO1Stream.AddItem("xAOD::ElectronAuxContainer#ElectronsAux.")
L1CALO1Stream.AddItem("xAOD::PhotonContainer#Photons")
L1CALO1Stream.AddItem("xAOD::PhotonAuxContainer#PhotonsAux.")
L1CALO1Stream.AddItem("xAOD::TauJetContainer#TauJets")
L1CALO1Stream.AddItem("xAOD::TauJetAuxContainer#TauJetsAux.-VertexedClusters.")
L1CALO1Stream.AddItem("xAOD::MissingETContainer#MET_Reference_AntiKt4EMTopo")
L1CALO1Stream.AddItem("xAOD::MissingETAuxContainer#MET_Reference_AntiKt4EMTopoAux.-ConstitObjectLinks.-ConstitObjectWeights")
L1CALO1Stream.AddItem("xAOD::VertexContainer#PrimaryVertices")
L1CALO1Stream.AddItem("xAOD::VertexAuxContainer#PrimaryVerticesAux.-vxTrackAtVertex.-MvfFitInfo.-isInitialized.-VTAV")
L1CALO1Stream.AddItem("xAOD::MissingETAssociationMap#METAssoc_AntiKt4EMTopo")
L1CALO1Stream.AddItem("xAOD::MissingETAuxAssociationMap#METAssoc_AntiKt4EMTopoAux.")
L1CALO1Stream.AddItem("xAOD::MissingETContainer#MET_Core_AntiKt4EMTopo")
L1CALO1Stream.AddItem("xAOD::MissingETAuxContainer#MET_Core_AntiKt4EMTopoAux.name.mpx.mpy.sumet.source")
L1CALO1Stream.AddItem("xAOD::MissingETContainer#MET_Track")
L1CALO1Stream.AddItem("xAOD::MissingETAuxContainer#MET_TrackAux.name.mpx.mpy")

#RoI Containers
L1CALO1Stream.AddItem("xAOD::EmTauRoIContainer#LVL1EmTauRoIs")
L1CALO1Stream.AddItem("xAOD::EmTauRoIAuxContainer#LVL1EmTauRoIsAux.")
L1CALO1Stream.AddItem("xAOD::EnergySumRoI#LVL1EnergySumRoI")
L1CALO1Stream.AddItem("xAOD::EnergySumRoIAuxInfo#LVL1EnergySumRoIAux.")
L1CALO1Stream.AddItem("xAOD::JetEtRoI#LVL1JetEtRoI")
L1CALO1Stream.AddItem("xAOD::JetEtRoIAuxInfo#LVL1JetEtRoIAux.")
L1CALO1Stream.AddItem("xAOD::JetRoIContainer#LVL1JetRoIs")
L1CALO1Stream.AddItem("xAOD::JetRoIAuxContainer#LVL1JetRoIsAux.")
L1CALO1Stream.AddItem("xAOD::MuonRoIContainer#LVL1MuonRoIs")
L1CALO1Stream.AddItem("xAOD::MuonRoIAuxContainer#LVL1MuonRoIsAux.")

#L1Calo containers
L1CALO1Stream.AddItem("xAOD::JEMTobRoIContainer#JEMTobRoIsRoIB")
L1CALO1Stream.AddItem("xAOD::JEMTobRoIAuxContainer#JEMTobRoIsRoIBAux.")
L1CALO1Stream.AddItem("xAOD::JEMTobRoIContainer#JEMTobRoIs")
L1CALO1Stream.AddItem("xAOD::JEMTobRoIAuxContainer#JEMTobRoIsAux.")
L1CALO1Stream.AddItem("xAOD::JEMEtSumsContainer#JEMEtSums")
L1CALO1Stream.AddItem("xAOD::JEMEtSumsAuxContainer#JEMEtSumsAux.")
L1CALO1Stream.AddItem("xAOD::CMXCPHitsAuxContainer#CMXCPHitsAux.")
L1CALO1Stream.AddItem("xAOD::CMXCPHitsContainer#CMXCPHits")
L1CALO1Stream.AddItem("xAOD::CMXCPTobAuxContainer#CMXCPTobsAux.")
L1CALO1Stream.AddItem("xAOD::CMXCPTobContainer#CMXCPTobs")
L1CALO1Stream.AddItem("xAOD::CMXEtSumsAuxContainer#CMXEtSumsAux.")
L1CALO1Stream.AddItem("xAOD::CMXEtSumsContainer#CMXEtSums")
L1CALO1Stream.AddItem("xAOD::CMXJetHitsAuxContainer#CMXJetHitsAux.")
L1CALO1Stream.AddItem("xAOD::CMXJetHitsContainer#CMXJetHits")
L1CALO1Stream.AddItem("xAOD::CMXJetTobAuxContainer#CMXJetTobsAux.")
L1CALO1Stream.AddItem("xAOD::CMXJetTobContainer#CMXJetTobs")
L1CALO1Stream.AddItem("xAOD::CMXRoIAuxContainer#CMXRoIsAux.")
L1CALO1Stream.AddItem("xAOD::CMXRoIContainer#CMXRoIs")
L1CALO1Stream.AddItem("xAOD::CPMTobRoIAuxContainer#CPMTobRoIsAux.")
L1CALO1Stream.AddItem("xAOD::CPMTobRoIAuxContainer#CPMTobRoIsRoIBAux.")
L1CALO1Stream.AddItem("xAOD::CPMTobRoIContainer#CPMTobRoIs")
L1CALO1Stream.AddItem("xAOD::CPMTobRoIContainer#CPMTobRoIsRoIB")
L1CALO1Stream.AddItem("xAOD::CPMTowerAuxContainer#CPMTowersAux.")
L1CALO1Stream.AddItem("xAOD::CPMTowerAuxContainer#CPMTowersOverlapAux.")
L1CALO1Stream.AddItem("xAOD::CPMTowerContainer#CPMTowers")
L1CALO1Stream.AddItem("xAOD::CPMTowerContainer#CPMTowersOverlap")
L1CALO1Stream.AddItem("xAOD::RODHeaderContainer#RODHeaders")
L1CALO1Stream.AddItem("xAOD::RODHeaderAuxContainer#RODHeadersAux.")
L1CALO1Stream.AddItem("xAOD::JetElementContainer#JetElements")
L1CALO1Stream.AddItem("xAOD::JetElementAuxContainer#JetElementsAux.")
L1CALO1Stream.AddItem("xAOD::JetElementContainer#JetElementsOverlap")
L1CALO1Stream.AddItem("xAOD::JetElementAuxContainer#JetElementsOverlapAux.")
L1CALO1Stream.AddItem("xAOD::TriggerTowerContainer#xAODTriggerTowers")
L1CALO1Stream.AddItem("xAOD::TriggerTowerAuxContainer#xAODTriggerTowersAux.")
L1CALO1Stream.AddItem("xAOD::L1TopoRawDataContainer#L1TopoRawData")
L1CALO1Stream.AddItem("xAOD::L1TopoRawDataAuxContainer#L1TopoRawDataAux.")

###Egamma CP additions###

L1CALO1Stream.AddItem("xAOD::TrackParticleContainer#GSFTrackParticles")
L1CALO1Stream.AddItem("xAOD::TrackParticleAuxContainer#GSFTrackParticlesAux."+trackParticleAuxExclusions)
L1CALO1Stream.AddItem("xAOD::VertexContainer#GSFConversionVertices")
L1CALO1Stream.AddItem("xAOD::VertexAuxContainer#GSFConversionVerticesAux.-vxTrackAtVertex")
L1CALO1Stream.AddItem("xAOD::CaloClusterContainer#egammaTopoSeededClusters")
L1CALO1Stream.AddItem("xAOD::CaloClusterAuxContainer#egammaTopoSeededClustersAux.")

###############



L1CALO1Stream.AddItem("xAOD::CaloClusterContainer#egammaClusters")
L1CALO1Stream.AddItem("xAOD::CaloClusterAuxContainer#egammaClustersAux.")
L1CALO1Stream.AddItem("xAOD::CaloClusterContainer#TauPi0Clusters")
L1CALO1Stream.AddItem("xAOD::CaloClusterAuxContainer#TauPi0ClustersAux.")
L1CALO1Stream.AddItem("xAOD::CaloClusterContainer#CaloCalTopoClusters")
L1CALO1Stream.AddItem("xAOD::CaloClusterAuxContainer#CaloCalTopoClustersAux.")
L1CALO1Stream.AddItem("xAOD::MuonSegmentContainer#MuonSegments")
L1CALO1Stream.AddItem("xAOD::MuonSegmentAuxContainer#MuonSegmentsAux.")



L1CALO1Stream.AddItem("xAOD::TruthParticleContainer#*")
L1CALO1Stream.AddItem("xAOD::TruthParticleAuxContainer#TruthParticlesAux.-caloExtension")
L1CALO1Stream.AddItem("xAOD::TruthVertexContainer#*")
L1CALO1Stream.AddItem("xAOD::TruthVertexAuxContainer#*")
L1CALO1Stream.AddItem("xAOD::TruthEventContainer#*")
L1CALO1Stream.AddItem("xAOD::TruthEventAuxContainer#*")


L1CALO1Stream.AddMetaDataItem("xAOD::TriggerMenuContainer#TriggerMenu")
L1CALO1Stream.AddMetaDataItem("xAOD::TriggerMenuAuxContainer#TriggerMenuAux.")
L1CALO1Stream.AddItem("TileCellContainer#MBTSContainer")
L1CALO1Stream.AddItem("xAOD::TrigDecision#xTrigDecision")
L1CALO1Stream.AddItem("xAOD::TrigNavigation#TrigNavigation")
L1CALO1Stream.AddItem("xAOD::TrigConfKeys#TrigConfKeys")
L1CALO1Stream.AddItem("HLT::HLTResult#HLTResult_HLT")
L1CALO1Stream.AddItem("xAOD::TrigDecisionAuxInfo#xTrigDecisionAux.")
L1CALO1Stream.AddItem("xAOD::TrigNavigationAuxInfo#TrigNavigationAux.")

print(L1CALO1Stream)


