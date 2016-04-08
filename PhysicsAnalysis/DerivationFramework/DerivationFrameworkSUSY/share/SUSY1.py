#********************************************************************
# SUSY1.py 
# reductionConf flag SUSY1 in Reco_tf.py   
#********************************************************************

from DerivationFrameworkCore.DerivationFrameworkMaster import *
from DerivationFrameworkJetEtMiss.JetCommon import *
from DerivationFrameworkJetEtMiss.ExtendedJetCommon import *
from DerivationFrameworkEGamma.EGammaCommon import *
from DerivationFrameworkMuons.MuonsCommon import *
from DerivationFrameworkTau.TauTruthCommon import *
from DerivationFrameworkInDet.InDetCommon import *
from DerivationFrameworkJetEtMiss.METCommon import *

### Set up stream
streamName = derivationFlags.WriteDAOD_SUSY1Stream.StreamName
fileName   = buildFileName( derivationFlags.WriteDAOD_SUSY1Stream )
SUSY1Stream = MSMgr.NewPoolRootStream( streamName, fileName )
SUSY1Stream.AcceptAlgs(["SUSY1KernelSkim"])

### Init
from DerivationFrameworkCore.ThinningHelper import ThinningHelper
SUSY1ThinningHelper = ThinningHelper( "SUSY1ThinningHelper" )
thinningTools       = []
AugmentationTools   = []

# stream-specific sequence for on-the-fly jet building
SeqSUSY1 = CfgMgr.AthSequencer("SeqSUSY1")
DerivationFrameworkJob += SeqSUSY1


#====================================================================
# Trigger navigation thinning
#====================================================================
triggerRegEx = [ # FIXME: disable unless tested
  ".?j.*",
  "xe.*",
  "e.*",
  "mu.*",
  "tau.*",
  "g.*"
]
SUSY1ThinningHelper.TriggerChains = 'HLT_(' + ' | '.join(triggerRegEx) + ')'
SUSY1ThinningHelper.AppendToStream( SUSY1Stream )


#====================================================================
# THINNING TOOL 
#====================================================================\

# MET/Jet tracks -> no longer needed, 11.05.2015

from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__TrackParticleThinning

SUSY1TPThinningTool = DerivationFramework__TrackParticleThinning(name = "SUSY1TPThinningTool",
								 ThinningService	 = SUSY1ThinningHelper.ThinningSvc(),
								 SelectionString	 = "InDetTrackParticles.pt > 10*GeV",
								 InDetTrackParticlesKey  = "InDetTrackParticles")
ToolSvc += SUSY1TPThinningTool
thinningTools.append(SUSY1TPThinningTool)

# TrackParticles associated with Muons
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__MuonTrackParticleThinning
SUSY1MuonTPThinningTool = DerivationFramework__MuonTrackParticleThinning(name			 = "SUSY1MuonTPThinningTool",
									 ThinningService	 = SUSY1ThinningHelper.ThinningSvc(),
									 MuonKey		 = "Muons",
									 InDetTrackParticlesKey  = "InDetTrackParticles")
ToolSvc += SUSY1MuonTPThinningTool
thinningTools.append(SUSY1MuonTPThinningTool)

# TrackParticles associated with electrons
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__EgammaTrackParticleThinning
SUSY1ElectronTPThinningTool = DerivationFramework__EgammaTrackParticleThinning(name			 = "SUSY1ElectronTPThinningTool",
										 ThinningService	 = SUSY1ThinningHelper.ThinningSvc(),
										 SGKey			 = "Electrons",
										 InDetTrackParticlesKey  = "InDetTrackParticles")
ToolSvc += SUSY1ElectronTPThinningTool
thinningTools.append(SUSY1ElectronTPThinningTool)

# TrackParticles associated with photons
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__EgammaTrackParticleThinning
SUSY1PhotonTPThinningTool = DerivationFramework__EgammaTrackParticleThinning(name			 = "SUSY1PhotonTPThinningTool",
										 ThinningService	 = SUSY1ThinningHelper.ThinningSvc(),
										 SGKey			 = "Photons",
										 InDetTrackParticlesKey  = "InDetTrackParticles")
ToolSvc += SUSY1PhotonTPThinningTool
thinningTools.append(SUSY1PhotonTPThinningTool)

# TrackParticles associated with taus
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__TauTrackParticleThinning
SUSY1TauTPThinningTool = DerivationFramework__TauTrackParticleThinning( name		= "SUSY1TauTPThinningTool",
									ThinningService = SUSY1ThinningHelper.ThinningSvc(),
									TauKey  	= "TauJets",
									InDetTrackParticlesKey  = "InDetTrackParticles")
ToolSvc += SUSY1TauTPThinningTool
thinningTools.append(SUSY1TauTPThinningTool)

# Calo Clusters associated with Photons
#from DerivationFrameworkCalo.DerivationFrameworkCaloConf import DerivationFramework__CaloClusterThinning
#SUSY1PhotonCCThinningTool = DerivationFramework__CaloClusterThinning( name                    = "SUSY1PhotonCCThinningTool",
#                                                                                     ThinningService         = SUSY1ThinningHelper.ThinningSvc(),
#                                                                                     SGKey                   = "Photons",
#                                                                                     CaloClCollectionSGKey   = "egammaClusters",
#                                                                                     TopoClCollectionSGKey   = "CaloCalTopoClusters",
#                                                                                     SelectionString         = "Photons.pt > 10*GeV",
                                                                                     #FrwdClCollectionSGKey   = "ForwardElectronClusters",
#                                                                                     ConeSize                = 0.6)
#ToolSvc += SUSY1PhotonCCThinningTool
#thinningTools.append(SUSY1PhotonCCThinningTool)

# Calo Clusters associated with Electrons
#SUSY1ElectronCCThinningTool = DerivationFramework__CaloClusterThinning( name                  = "SUSY1ElectronCCThinningTool",
#                                                                                     ThinningService         = SUSY1ThinningHelper.ThinningSvc(),
#                                                                                     SGKey                   = "Electrons",
#                                                                                     CaloClCollectionSGKey   = "egammaClusters",
#                                                                                     TopoClCollectionSGKey   = "CaloCalTopoClusters",
#                                                                                     SelectionString         = "Electrons.pt > 7*GeV",
                                                                                     #FrwdClCollectionSGKey   = "ForwardElectronClusters",
#                                                                                     ConeSize                = 0.4)
#ToolSvc += SUSY1ElectronCCThinningTool
#thinningTools.append(SUSY1ElectronCCThinningTool)

# Calo Clusters associated with jets
#from DerivationFrameworkEGamma.DerivationFrameworkEGammaConf import DerivationFramework__JetCaloClusterThinning
#SUSY1aKt4CCThinningTool = DerivationFramework__JetCaloClusterThinning(name                    = "SUSY1aKt4CCThinningTool",
#                                                                     ThinningService         = SUSY1ThinningHelper.ThinningSvc(),
#                                                                      SGKey                   = "AntiKt4LCTopoJets",
#                                                                      TopoClCollectionSGKey   = "CaloCalTopoClusters",
#                                                                      SelectionString         = "AntiKt4LCTopoJets.pt > 20*GeV",
#                                                                      ConeSize                = 0)
#ToolSvc += SUSY1aKt4CCThinningTool
#thinningTools.append(SUSY1aKt4CCThinningTool)

#====================================================================
# TRUTH THINNING
#====================================================================
if DerivationFrameworkIsMonteCarlo:
  from MCTruthClassifier.MCTruthClassifierConf import MCTruthClassifier
  SUSY1Classifier = MCTruthClassifier( name                      = "SUSY1Classifier",
                                       ParticleCaloExtensionTool = "" )
  ToolSvc += SUSY1Classifier

  from DerivationFrameworkMCTruth.DerivationFrameworkMCTruthConf import DerivationFramework__TruthClassificationDecorator
  SUSY1ClassificationDecorator = DerivationFramework__TruthClassificationDecorator(
                                  name              = "SUSY1ClassificationDecorator",
                                  ParticlesKey      = "TruthParticles",
                                  MCTruthClassifier = SUSY1Classifier)
  ToolSvc += SUSY1ClassificationDecorator
  AugmentationTools.append(SUSY1ClassificationDecorator)
  
  from DerivationFrameworkMCTruth.DerivationFrameworkMCTruthConf import DerivationFramework__MenuTruthThinning
  SUSY1TruthThinningTool = DerivationFramework__MenuTruthThinning(name              = "SUSY1TruthThinningTool",
                                                       ThinningService              = SUSY1ThinningHelper.ThinningSvc(),
                                                       WritePartons                 = False,
                                                       WriteHadrons                 = False,
                                                       WriteBHadrons                = True,
                                                       WriteGeant                   = False,
                                                       GeantPhotonPtThresh          = 20000,
                                                       WriteTauHad                  = True,
                                                       PartonPtThresh               = -1.0,
                                                       WriteBSM                     = True,
                                                       WriteBosons                  = True,
                                                       WriteBosonProducts           = True,
                                                       WriteBSMProducts             = True,
                                                       WriteTopAndDecays            = True,
                                                       WriteEverything              = False,
                                                       WriteAllLeptons              = False,
                                                       WriteLeptonsNotFromHadrons   = False,
                                                       WriteStatus3                 = False,
                                                       WriteFirstN                  = -1,
                                                       PreserveAncestors            = True,
                                                       PreserveGeneratorDescendants = False,
                                                       SimBarcodeOffset             = DerivationFrameworkSimBarcodeOffset)
  ToolSvc += SUSY1TruthThinningTool
  thinningTools.append(SUSY1TruthThinningTool)

if globalflags.DataSource()=='geant4':
        from DerivationFrameworkMCTruth.DerivationFrameworkMCTruthConf import DerivationFramework__TruthCollectionMaker
        from DerivationFrameworkMCTruth.DerivationFrameworkMCTruthConf import DerivationFramework__TruthCollectionMakerTau

        SUSY1TruthPhotonTool = DerivationFramework__TruthCollectionMaker(
                name                 = "SUSY1TruthPhotonTool",
                NewCollectionName       = "SUSY1TruthPhotons",
                ParticleSelectionString = "(abs(TruthParticles.pdgId) == 22) && (TruthParticles.status == 1) && (TruthParticles.barcode < %d) && (TruthParticles.pt > 20.0*GeV)" % DerivationFrameworkSimBarcodeOffset)
        ToolSvc += SUSY1TruthPhotonTool
        AugmentationTools.append(SUSY1TruthPhotonTool)        

        SUSY1TruthElectronTool = DerivationFramework__TruthCollectionMaker(
                name                 = "SUSY1TruthElectronTool",
                NewCollectionName       = "SUSY1TruthElectrons",
                ParticleSelectionString = "(abs(TruthParticles.pdgId) == 11) && (TruthParticles.status == 1) && (TruthParticles.barcode < %d)" % DerivationFrameworkSimBarcodeOffset)
        ToolSvc += SUSY1TruthElectronTool
        AugmentationTools.append(SUSY1TruthElectronTool)        

        SUSY1TruthMuonTool = DerivationFramework__TruthCollectionMaker(
                name                 = "SUSY1TruthMuonTool",
                NewCollectionName       = "SUSY1TruthMuons",
                ParticleSelectionString = "(abs(TruthParticles.pdgId) == 13) && (TruthParticles.status == 1) && (TruthParticles.barcode < %d)" % DerivationFrameworkSimBarcodeOffset)
        ToolSvc += SUSY1TruthMuonTool
        AugmentationTools.append(SUSY1TruthMuonTool)        

        #SUSY1TruthTauTool = DerivationFramework__TruthCollectionMakerTau(
                #name                 = "SUSY1TruthTauTool",
                #NewCollectionName       = "SUSY1TruthTaus")
        #ToolSvc += SUSY1TruthTauTool
        #AugmentationTools.append(SUSY1TruthTauTool)        

        SUSY1TruthNeutrinoTool = DerivationFramework__TruthCollectionMaker(
                name                 = "SUSY1TruthNeutrinoTool",
                NewCollectionName       = "SUSY1TruthNeutrinos",
                ParticleSelectionString = "(abs(TruthParticles.pdgId) == 12 || abs(TruthParticles.pdgId) == 14 || abs(TruthParticles.pdgId) == 16) && (TruthParticles.status == 1) && (TruthParticles.barcode < %d)" % DerivationFrameworkSimBarcodeOffset)
        ToolSvc += SUSY1TruthNeutrinoTool
        AugmentationTools.append(SUSY1TruthNeutrinoTool)        

#====================================================================
# SKIMMING
#====================================================================
expression_cut_HT      = 'sum(AntiKt4EMTopoJets.DFCommonJets_Calib_pt * ((AntiKt4EMTopoJets.DFCommonJets_Calib_pt > 40*GeV) && (abs(AntiKt4EMTopoJets.DFCommonJets_Calib_eta) < 2.8)) ) > 150*GeV'
expression_cut_elecs   = 'count((Electrons.pt > 100*GeV) && (abs(Electrons.eta) < 2.6) && ((Electrons.Loose) || (Electrons.DFCommonElectronsLHLoose))) > 0 || count((Electrons.pt > 20*GeV) && (abs(Electrons.eta) < 2.6) && ((Electrons.Loose) || (Electrons.DFCommonElectronsLHLoose))) > 1'
expression_cut_muons   = 'count((Muons.pt >= 100*GeV) && (abs(Muons.eta) < 2.6) && (Muons.DFCommonGoodMuon)) > 0 || count((Muons.pt >= 20*GeV) && (abs(Muons.eta) < 2.6) && (Muons.DFCommonGoodMuon)) > 1'
expression_cut_photons = 'count((DFCommonPhotons_et > 100*GeV) && (abs(DFCommonPhotons_eta) < 2.6)) > 0 || count((DFCommonPhotons_et > 50*GeV) && (abs(DFCommonPhotons_eta) < 2.6)) > 1'

expression             = '(%s) || (%s) || (%s) || (%s)' % (expression_cut_HT, expression_cut_elecs, expression_cut_muons, expression_cut_photons)

from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__xAODStringSkimmingTool
SUSY1SkimmingTool = DerivationFramework__xAODStringSkimmingTool( name = "SUSY1SkimmingTool",
                                                                expression = expression)
ToolSvc += SUSY1SkimmingTool

#=======================================
# CREATE THE DERIVATION KERNEL ALGORITHM   
#=======================================
applyJetCalibration_xAODColl("AntiKt4EMTopo", SeqSUSY1) # default: sequence=DerivationFrameworkJob
# updateJVT_xAODColl("AntiKt4EMTopo") # TODO: for next cache?

from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__DerivationKernel
SeqSUSY1 += CfgMgr.DerivationFramework__DerivationKernel(
	"SUSY1KernelSkim",
	SkimmingTools = [SUSY1SkimmingTool],
)

#==============================================================================
# SUSY signal augmentation
#==============================================================================
from DerivationFrameworkSUSY.DecorateSUSYProcess import DecorateSUSYProcess
AugmentationTools += DecorateSUSYProcess("SUSY1")

#==============================================================================
# SUSY background generator filters
#==============================================================================
if globalflags.DataSource() == 'geant4':
  replaceBuggyAntiKt4TruthWZJets(SeqSUSY1)
  ToolSvc += CfgMgr.DerivationFramework__SUSYGenFilterTool(
    "SUSY1GenFilt",
    SimBarcodeOffset = DerivationFrameworkSimBarcodeOffset
  )
  AugmentationTools.append(ToolSvc.SUSY1GenFilt)

#==============================================================================
# Augment after skim
#==============================================================================
SeqSUSY1 += CfgMgr.DerivationFramework__DerivationKernel(
	"SUSY1KernelAug",
	AugmentationTools = AugmentationTools,
	ThinningTools = thinningTools,
)

#==============================================================================
# Jet building
#==============================================================================
# AntiKt10*PtFrac5Rclus20
addDefaultTrimmedJets(SeqSUSY1, "SUSY1")


#====================================================================
# CONTENT LIST  
#====================================================================
# About SmartCollections and AllVariables:
#   If you want to use CP tools on a container, you MUST include that container as a SmartCollection. 
#   You are then free to include it as AllVariables in addition, to pick up variables not needed by the CP tools but needed by your analysis.
from DerivationFrameworkCore.SlimmingHelper import SlimmingHelper
SUSY1SlimmingHelper = SlimmingHelper("SUSY1SlimmingHelper")
SUSY1SlimmingHelper.SmartCollections = ["Electrons","Photons", "MET_Reference_AntiKt4EMTopo", "Muons", "TauJets", "BTagging_AntiKt4EMTopo", "InDetTrackParticles", "PrimaryVertices", "BTagging_AntiKt4Track"]
SUSY1SlimmingHelper.AllVariables = ["TruthParticles", "TruthEvents", "TruthVertices", "MET_Truth", "AntiKt4PV0TrackJets", "MET_Track"] 
SUSY1SlimmingHelper.ExtraVariables = ["BTagging_AntiKt4EMTopo.MV1_discriminant.MV1c_discriminant",
				      "Muons.etcone30.ptcone30.ptcone20.charge.quality.InnerDetectorPt.MuonSpectrometerPt.CaloLRLikelihood.CaloMuonIDTag",
				      "Photons.author.Loose.Tight",
				      "AntiKt4EMTopoJets.NumTrkPt1000.TrackWidthPt1000.NumTrkPt500.DFCommonJets_Calib_pt.DFCommonJets_Calib_eta.DFCommonJets_Calib_phi", # TODO: .DFCommonJets_Jvt",
				      "GSFTrackParticles.z0.d0.vz.definingParametersCovMatrix",
				      "CombinedMuonTrackParticles.d0.z0.vz.definingParametersCovMatrix.truthOrigin.truthType",
				      "ExtrapolatedMuonTrackParticles.d0.z0.vz.definingParametersCovMatrix.truthOrigin.truthType",
				      "TauJets.TruthCharge.TruthProng.IsTruthMatched.TruthPtVis.truthOrigin.truthType.truthParticleLink.truthJetLink",
				      "MuonTruthParticles.barcode.decayVtxLink.e.m.pdgId.prodVtxLink.px.py.pz.recoMuonLink.status.truthOrigin.truthType",
				      "AntiKt4TruthJets.eta.m.phi.pt.TruthLabelDeltaR_B.TruthLabelDeltaR_C.TruthLabelDeltaR_T.TruthLabelID.ConeTruthLabelID.PartonTruthLabelID",
				      ]
if globalflags.DataSource()=='geant4':
  SUSY1SlimmingHelper.StaticContent += [
    "xAOD::TruthParticleContainer#SUSY1TruthMuons",
    "xAOD::TruthParticleAuxContainer#SUSY1TruthMuonsAux.",
    "xAOD::TruthParticleContainer#SUSY1TruthElectrons",
    "xAOD::TruthParticleAuxContainer#SUSY1TruthElectronsAux.",
    "xAOD::TruthParticleContainer#SUSY1TruthPhotons",
    "xAOD::TruthParticleAuxContainer#SUSY1TruthPhotonsAux.",
    "xAOD::TruthParticleContainer#SUSY1TruthNeutrinos",
    "xAOD::TruthParticleAuxContainer#SUSY1TruthNeutrinosAux.",
    #"xAOD::TruthParticleContainer#SUSY1TruthTaus",
    #"xAOD::TruthParticleAuxContainer#SUSY1TruthTausAux." 
    "xAOD::TruthParticleContainer#TruthTaus",
    "xAOD::TruthParticleAuxContainer#TruthTausAux.",
  ]

SUSY1SlimmingHelper.IncludeMuonTriggerContent   = True
SUSY1SlimmingHelper.IncludeEGammaTriggerContent = True
SUSY1SlimmingHelper.IncludeBPhysTriggerContent  = False
SUSY1SlimmingHelper.IncludeJetTriggerContent    = True
SUSY1SlimmingHelper.IncludeTauTriggerContent    = True
SUSY1SlimmingHelper.IncludeEtMissTriggerContent = True
SUSY1SlimmingHelper.IncludeBJetTriggerContent   = True

addJetOutputs(SUSY1SlimmingHelper, ["SmallR", "SUSY1"], ["AntiKt4EMTopoJets"], ["AntiKt3PV0TrackJets", "AntiKt2PV0TrackJets", "AntiKt4EMPFlowJets", "AntiKt4LCTopoJets", "AntiKt4TruthJets"])

SUSY1SlimmingHelper.AppendContentToStream(SUSY1Stream)
