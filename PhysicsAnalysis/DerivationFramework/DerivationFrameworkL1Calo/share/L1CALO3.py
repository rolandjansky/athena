#################
### Steering options
#################
## Load common flags
from AthenaCommon.JobProperties import jobproperties as athCommonFlags

## Steer output file
from OutputStreamAthenaPool.MultipleStreamManager import MSMgr
from PrimaryDPDMaker.PrimaryDPDHelpers import buildFileName
from PrimaryDPDMaker.PrimaryDPDFlags import primDPD
streamName = primDPD.WriteDAOD_L1CALO3.StreamName
fileName   = buildFileName( primDPD.WriteDAOD_L1CALO3 )
L1CALO3Stream = MSMgr.NewPoolRootStream( streamName, fileName )
L1CALO3Stream.AcceptAlgs(["DFL1CALO3_KERN"])
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
L1CALO3CaloThinningTool = DerivationFramework__TriggerTowerThinningAlg( name = "L1CALO3CaloThinningTool",
                                                                        StreamName              = streamName,
    									TriggerTowerLocation = "xAODTriggerTowers",
    									MinCaloCellET = 0.8,
    									MinADC = 36,
    									UseRandom = True,
    									MinRandom = 0.01)
ToolSvc += L1CALO3CaloThinningTool
thinningTools.append(L1CALO3CaloThinningTool)


#====================================================================
# Create the derivation Kernel and setup output stream
#====================================================================
# Add the derivation job to the top AthAlgSeqeuence
# DerivationJob is COMMON TO ALL DERIVATIONS
DerivationFrameworkJob = CfgMgr.AthSequencer("L1CALO3Seq")
from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__CommonAugmentation
DerivationFrameworkJob += CfgMgr.DerivationFramework__DerivationKernel("DFL1CALO3_KERN",
                                                                       AugmentationTools = augmentationTools,
                                                                       SkimmingTools = skimmingTools,
                                                                       ThinningTools = thinningTools)

topSequence += DerivationFrameworkJob
print( DerivationFrameworkJob)
print( DerivationFrameworkJob.properties())

#################
### Steer output file content
#################
## Add service for metadata
ToolSvc += CfgMgr.xAODMaker__TriggerMenuMetaDataTool(
"TriggerMenuMetaDataTool" )
svcMgr.MetaDataSvc.MetaDataTools += [ ToolSvc.TriggerMenuMetaDataTool ]


trackParticleAuxExclusions="-caloExtension.-cellAssociation.-clusterAssociation.-trackParameterCovarianceMatrices.-parameterX.-parameterY.-parameterZ.-parameterPX.-parameterPY.-parameterPZ.-parameterPosition"

# Generic event info
L1CALO3Stream.AddItem("xAOD::EventInfo#*")
L1CALO3Stream.AddItem("xAOD::EventAuxInfo#*")
L1CALO3Stream.AddItem("xAOD::EventShape#Kt4EMTopoOriginEventShape")
L1CALO3Stream.AddItem("xAOD::EventShapeAuxInfo#Kt4EMTopoOriginEventShapeAux.")

#Physics Objects
L1CALO3Stream.AddItem("xAOD::JetContainer#AntiKt4EMTopoJets")
L1CALO3Stream.AddItem("xAOD::JetAuxContainer#AntiKt4EMTopoJetsAux.")
L1CALO3Stream.AddItem("xAOD::MuonContainer#Muons")
L1CALO3Stream.AddItem("xAOD::MuonAuxContainer#MuonsAux.")
L1CALO3Stream.AddItem("xAOD::ElectronContainer#Electrons")
L1CALO3Stream.AddItem("xAOD::ElectronAuxContainer#ElectronsAux.")
L1CALO3Stream.AddItem("xAOD::PhotonContainer#Photons")
L1CALO3Stream.AddItem("xAOD::PhotonAuxContainer#PhotonsAux.")
L1CALO3Stream.AddItem("xAOD::TauJetContainer#TauJets")
L1CALO3Stream.AddItem("xAOD::TauJetAuxContainer#TauJetsAux.-VertexedClusters.")
L1CALO3Stream.AddItem("xAOD::MissingETContainer#MET_Reference_AntiKt4EMTopo")
L1CALO3Stream.AddItem("xAOD::MissingETAuxContainer#MET_Reference_AntiKt4EMTopoAux.-ConstitObjectLinks.-ConstitObjectWeights")
L1CALO3Stream.AddItem("xAOD::VertexContainer#PrimaryVertices")
L1CALO3Stream.AddItem("xAOD::VertexAuxContainer#PrimaryVerticesAux.-vxTrackAtVertex.-MvfFitInfo.-isInitialized.-VTAV")
L1CALO3Stream.AddItem("xAOD::MissingETAssociationMap#METAssoc_AntiKt4EMTopo")
L1CALO3Stream.AddItem("xAOD::MissingETAuxAssociationMap#METAssoc_AntiKt4EMTopoAux.")
L1CALO3Stream.AddItem("xAOD::MissingETContainer#MET_Core_AntiKt4EMTopo")
L1CALO3Stream.AddItem("xAOD::MissingETAuxContainer#MET_Core_AntiKt4EMTopoAux.name.mpx.mpy.sumet.source")
L1CALO3Stream.AddItem("xAOD::MissingETContainer#MET_Track")
L1CALO3Stream.AddItem("xAOD::MissingETAuxContainer#MET_TrackAux.name.mpx.mpy")

#RoI Containers
L1CALO3Stream.AddItem("xAOD::EmTauRoIContainer#LVL1EmTauRoIs")
L1CALO3Stream.AddItem("xAOD::EmTauRoIAuxContainer#LVL1EmTauRoIsAux.")
L1CALO3Stream.AddItem("xAOD::EnergySumRoI#LVL1EnergySumRoI")
L1CALO3Stream.AddItem("xAOD::EnergySumRoIAuxInfo#LVL1EnergySumRoIAux.")
L1CALO3Stream.AddItem("xAOD::JetEtRoI#LVL1JetEtRoI")
L1CALO3Stream.AddItem("xAOD::JetEtRoIAuxInfo#LVL1JetEtRoIAux.")
L1CALO3Stream.AddItem("xAOD::JetRoIContainer#LVL1JetRoIs")
L1CALO3Stream.AddItem("xAOD::JetRoIAuxContainer#LVL1JetRoIsAux.")
L1CALO3Stream.AddItem("xAOD::MuonRoIContainer#LVL1MuonRoIs")
L1CALO3Stream.AddItem("xAOD::MuonRoIAuxContainer#LVL1MuonRoIsAux.")

#L1Calo containers
L1CALO3Stream.AddItem("xAOD::JEMTobRoIContainer#JEMTobRoIsRoIB")
L1CALO3Stream.AddItem("xAOD::JEMTobRoIAuxContainer#JEMTobRoIsRoIBAux.")
L1CALO3Stream.AddItem("xAOD::JEMTobRoIContainer#JEMTobRoIs")
L1CALO3Stream.AddItem("xAOD::JEMTobRoIAuxContainer#JEMTobRoIsAux.")
L1CALO3Stream.AddItem("xAOD::JEMEtSumsContainer#JEMEtSums")
L1CALO3Stream.AddItem("xAOD::JEMEtSumsAuxContainer#JEMEtSumsAux.")
L1CALO3Stream.AddItem("xAOD::CMXCPHitsAuxContainer#CMXCPHitsAux.")
L1CALO3Stream.AddItem("xAOD::CMXCPHitsContainer#CMXCPHits")
L1CALO3Stream.AddItem("xAOD::CMXCPTobAuxContainer#CMXCPTobsAux.")
L1CALO3Stream.AddItem("xAOD::CMXCPTobContainer#CMXCPTobs")
L1CALO3Stream.AddItem("xAOD::CMXEtSumsAuxContainer#CMXEtSumsAux.")
L1CALO3Stream.AddItem("xAOD::CMXEtSumsContainer#CMXEtSums")
L1CALO3Stream.AddItem("xAOD::CMXJetHitsAuxContainer#CMXJetHitsAux.")
L1CALO3Stream.AddItem("xAOD::CMXJetHitsContainer#CMXJetHits")
L1CALO3Stream.AddItem("xAOD::CMXJetTobAuxContainer#CMXJetTobsAux.")
L1CALO3Stream.AddItem("xAOD::CMXJetTobContainer#CMXJetTobs")
L1CALO3Stream.AddItem("xAOD::CMXRoIAuxContainer#CMXRoIsAux.")
L1CALO3Stream.AddItem("xAOD::CMXRoIContainer#CMXRoIs")
L1CALO3Stream.AddItem("xAOD::CPMTobRoIAuxContainer#CPMTobRoIsAux.")
L1CALO3Stream.AddItem("xAOD::CPMTobRoIAuxContainer#CPMTobRoIsRoIBAux.")
L1CALO3Stream.AddItem("xAOD::CPMTobRoIContainer#CPMTobRoIs")
L1CALO3Stream.AddItem("xAOD::CPMTobRoIContainer#CPMTobRoIsRoIB")
L1CALO3Stream.AddItem("xAOD::CPMTowerAuxContainer#CPMTowersAux.")
L1CALO3Stream.AddItem("xAOD::CPMTowerAuxContainer#CPMTowersOverlapAux.")
L1CALO3Stream.AddItem("xAOD::CPMTowerContainer#CPMTowers")
L1CALO3Stream.AddItem("xAOD::CPMTowerContainer#CPMTowersOverlap")
L1CALO3Stream.AddItem("xAOD::RODHeaderContainer#RODHeaders")
L1CALO3Stream.AddItem("xAOD::RODHeaderAuxContainer#RODHeadersAux.")
L1CALO3Stream.AddItem("xAOD::JetElementContainer#JetElements")
L1CALO3Stream.AddItem("xAOD::JetElementAuxContainer#JetElementsAux.")
L1CALO3Stream.AddItem("xAOD::JetElementContainer#JetElementsOverlap")
L1CALO3Stream.AddItem("xAOD::JetElementAuxContainer#JetElementsOverlapAux.")
L1CALO3Stream.AddItem("xAOD::TriggerTowerContainer#xAODTriggerTowers")
L1CALO3Stream.AddItem("xAOD::TriggerTowerAuxContainer#xAODTriggerTowersAux.")
L1CALO3Stream.AddItem("xAOD::L1TopoRawDataContainer#L1TopoRawData")
L1CALO3Stream.AddItem("xAOD::L1TopoRawDataAuxContainer#L1TopoRawDataAux.")

###Egamma CP additions###

L1CALO3Stream.AddItem("xAOD::TrackParticleContainer#GSFTrackParticles")
L1CALO3Stream.AddItem("xAOD::TrackParticleAuxContainer#GSFTrackParticlesAux."+trackParticleAuxExclusions)
L1CALO3Stream.AddItem("xAOD::VertexContainer#GSFConversionVertices")
L1CALO3Stream.AddItem("xAOD::VertexAuxContainer#GSFConversionVerticesAux.-vxTrackAtVertex")
L1CALO3Stream.AddItem("xAOD::CaloClusterContainer#egammaTopoSeededClusters")
L1CALO3Stream.AddItem("xAOD::CaloClusterAuxContainer#egammaTopoSeededClustersAux.")

###############



L1CALO3Stream.AddItem("xAOD::CaloClusterContainer#egammaClusters")
L1CALO3Stream.AddItem("xAOD::CaloClusterAuxContainer#egammaClustersAux.")
L1CALO3Stream.AddItem("xAOD::CaloClusterContainer#TauPi0Clusters")
L1CALO3Stream.AddItem("xAOD::CaloClusterAuxContainer#TauPi0ClustersAux.")
L1CALO3Stream.AddItem("xAOD::CaloClusterContainer#CaloCalTopoClusters")
L1CALO3Stream.AddItem("xAOD::CaloClusterAuxContainer#CaloCalTopoClustersAux.")
L1CALO3Stream.AddItem("xAOD::MuonSegmentContainer#MuonSegments")
L1CALO3Stream.AddItem("xAOD::MuonSegmentAuxContainer#MuonSegmentsAux.")



L1CALO3Stream.AddItem("xAOD::TruthParticleContainer#*")
L1CALO3Stream.AddItem("xAOD::TruthParticleAuxContainer#TruthParticlesAux.-caloExtension")
L1CALO3Stream.AddItem("xAOD::TruthVertexContainer#*")
L1CALO3Stream.AddItem("xAOD::TruthVertexAuxContainer#*")
L1CALO3Stream.AddItem("xAOD::TruthEventContainer#*")
L1CALO3Stream.AddItem("xAOD::TruthEventAuxContainer#*")


L1CALO3Stream.AddMetaDataItem("xAOD::TriggerMenuContainer#TriggerMenu")
L1CALO3Stream.AddMetaDataItem("xAOD::TriggerMenuAuxContainer#TriggerMenuAux.")
L1CALO3Stream.AddItem("TileCellContainer#MBTSContainer")
L1CALO3Stream.AddItem("xAOD::TrigDecision#xTrigDecision")
L1CALO3Stream.AddItem("xAOD::TrigNavigation#TrigNavigation")
L1CALO3Stream.AddItem("xAOD::TrigConfKeys#TrigConfKeys")
L1CALO3Stream.AddItem("HLT::HLTResult#HLTResult_HLT")
L1CALO3Stream.AddItem("xAOD::TrigDecisionAuxInfo#xTrigDecisionAux.")
L1CALO3Stream.AddItem("xAOD::TrigNavigationAuxInfo#TrigNavigationAux.")

print( L1CALO3Stream)


