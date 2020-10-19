#********************************************************************
# SUSY1.py
# reductionConf flag SUSY1 in Reco_tf.py
#********************************************************************

from DerivationFrameworkCore.DerivationFrameworkMaster import *
from DerivationFrameworkJetEtMiss.JetCommon import *
from DerivationFrameworkJetEtMiss.ExtendedJetCommon import *
from DerivationFrameworkEGamma.EGammaCommon import *
from DerivationFrameworkMuons.MuonsCommon import *
from DerivationFrameworkInDet.InDetCommon import *
from DerivationFrameworkJetEtMiss.METCommon import *
from DerivationFrameworkFlavourTag.FlavourTagCommon import *
from DerivationFrameworkFlavourTag.HbbCommon import *
from BTagging.BTaggingFlags import BTaggingFlags
from DerivationFrameworkSUSY.SUSYCommonFlags import SUSYFlags


#BTaggingFlags.Do2019Retraining = False 

if DerivationFrameworkHasTruth:
  from DerivationFrameworkMCTruth.MCTruthCommon import addStandardTruthContents
  addStandardTruthContents()

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
from DerivationFrameworkSUSY.SUSY1TriggerList import SUSY1ThinTriggers

# Note here using truth information as a proxy for running different triggers (not obvious!)
if SUSYFlags.TurnOffThinning == True and not DerivationFrameworkHasTruth:
  SUSY1ThinningHelper.TriggerChains = ""
else:
  SUSY1ThinningHelper.TriggerChains = '|'.join(SUSY1ThinTriggers)

SUSY1ThinningHelper.AppendToStream( SUSY1Stream )
SUSY1ThinningHelper.Actions = ['DropFeatures', 'Reload', 'SyncThinning', 'Save', 'Restore']




#====================================================================
# THINNING TOOL
#====================================================================\

# TrackParticles associated with Vertices from soft tagging x3 OPs
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__VertexParticleThinning

SUSY1VertexTPThinningToolTight = DerivationFramework__VertexParticleThinning(name		 = "SUSY1VertexTPThinningToolTight",
									 ThinningService	 = SUSY1ThinningHelper.ThinningSvc(),
									 VertexKey		 = "SoftBVrtClusterTool_Tight_Vertices",
									 InDetTrackParticlesKey  = "InDetTrackParticles")
ToolSvc += SUSY1VertexTPThinningToolTight
thinningTools.append(SUSY1VertexTPThinningToolTight)

SUSY1VertexTPThinningToolMedium = DerivationFramework__VertexParticleThinning(name		 = "SUSY1VertexTPThinningToolMedium",
									 ThinningService	 = SUSY1ThinningHelper.ThinningSvc(),
									 VertexKey		 = "SoftBVrtClusterTool_Medium_Vertices",
									 InDetTrackParticlesKey  = "InDetTrackParticles")
ToolSvc += SUSY1VertexTPThinningToolMedium
thinningTools.append(SUSY1VertexTPThinningToolMedium)

SUSY1VertexTPThinningToolLoose = DerivationFramework__VertexParticleThinning(name		 = "SUSY1VertexTPThinningToolLoose",
									 ThinningService	 = SUSY1ThinningHelper.ThinningSvc(),
									 VertexKey		 = "SoftBVrtClusterTool_Loose_Vertices",
									 InDetTrackParticlesKey  = "InDetTrackParticles")
ToolSvc += SUSY1VertexTPThinningToolLoose
thinningTools.append(SUSY1VertexTPThinningToolLoose)

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

from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__GenericObjectThinning
SUSY1PhotonThinningTool = DerivationFramework__GenericObjectThinning( name             = "SUSY1PhotonThinningTool",
                                                                      ThinningService  = SUSY1ThinningHelper.ThinningSvc(),
                                                                      ContainerName    = "Photons",
                                                                      ApplyAnd         = False,
                                                                      SelectionString  = "Photons.pt > 10*GeV")
ToolSvc += SUSY1PhotonThinningTool
thinningTools.append(SUSY1PhotonThinningTool)

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
#                                                                      SelectionString         = "AntiKt4LCTopoJets.pt > 20*GeV")
#ToolSvc += SUSY1aKt4CCThinningTool
#thinningTools.append(SUSY1aKt4CCThinningTool)

#====================================================================
# TRUTH THINNING
#====================================================================
if DerivationFrameworkHasTruth:

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


#====================================================================
# SKIMMING
#====================================================================
expression_cut_HT      = 'sum(AntiKt4EMTopoJets.DFCommonJets_Calib_pt * ((AntiKt4EMTopoJets.DFCommonJets_Calib_pt > 40*GeV) && (abs(AntiKt4EMTopoJets.DFCommonJets_Calib_eta) < 2.8)) ) > 150*GeV'

expression_cut_elecs   = 'count((Electrons.pt > 100*GeV) && (abs(Electrons.eta) < 2.6) && (Electrons.DFCommonElectronsLHLoose)) > 0 || count((Electrons.pt > 20*GeV) && (abs(Electrons.eta) < 2.6) && (Electrons.DFCommonElectronsLHLoose)) > 1'
expression_cut_muons   = 'count((Muons.pt >= 100*GeV) && (abs(Muons.eta) < 2.6) && (Muons.DFCommonMuonsPreselection)) > 0 || count((Muons.pt >= 20*GeV) && (abs(Muons.eta) < 2.6) && (Muons.DFCommonMuonsPreselection)) > 1'
expression_cut_photons = 'count((DFCommonPhotons_et > 100*GeV) && (abs(DFCommonPhotons_eta) < 2.6)) > 0 || count((DFCommonPhotons_et > 50*GeV) && (abs(DFCommonPhotons_eta) < 2.6)) > 1'

expression             = '(%s) || (%s) || (%s) || (%s)' % (expression_cut_HT, expression_cut_elecs, expression_cut_muons, expression_cut_photons)


##NEW SCHEME
from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__xAODStringSkimmingTool
SUSY1SkimmingORTool = DerivationFramework__xAODStringSkimmingTool( name = "SUSY1SkimmingORTool",
                                                                   expression = expression)
ToolSvc += SUSY1SkimmingORTool

#add AND with Trigger skimming criteria
from DerivationFrameworkSUSY.SUSY1TriggerList import SUSY1AllTriggers
trigExpr = '('+' || '.join(SUSY1AllTriggers)+')'

from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__TriggerSkimmingTool
SUSY1trigSkimmingTool = DerivationFramework__TriggerSkimmingTool( name = "SUSY1trigSkimmingTool",
                                                                  TriggerListOR = SUSY1AllTriggers)

ToolSvc += SUSY1trigSkimmingTool

from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__FilterCombinationAND
SUSY1SkimmingTool = DerivationFramework__FilterCombinationAND(name = "SUSY1SkimmingTool",
                                                              FilterList = [SUSY1SkimmingORTool, SUSY1trigSkimmingTool])
ToolSvc += SUSY1SkimmingTool





from EventShapeTools.EventDensityConfig import configEventDensityTool, EventDensityAlg

#====================================================================
# ISOLATION TOOL
#====================================================================
#Track selection
from IsolationTool.IsolationToolConf import xAOD__TrackIsolationTool
TrackIsoTool = xAOD__TrackIsolationTool("TrackIsoTool")
TrackIsoTool.TrackSelectionTool.maxZ0SinTheta= 3.
TrackIsoTool.TrackSelectionTool.minPt= 1000.
TrackIsoTool.TrackSelectionTool.CutLevel= "Loose"
ToolSvc += TrackIsoTool

TrackIsoTool500 = xAOD__TrackIsolationTool("TrackIsoTool500")
TrackIsoTool500.TrackSelectionTool.maxZ0SinTheta= 3.
TrackIsoTool500.TrackSelectionTool.minPt= 500.
TrackIsoTool500.TrackSelectionTool.CutLevel= "Loose"
ToolSvc += TrackIsoTool500

from DerivationFrameworkSUSY.DerivationFrameworkSUSYConf import DerivationFramework__trackIsolationDecorator
import ROOT, PyCintex
PyCintex.loadDictionary('xAODCoreRflxDict')
PyCintex.loadDictionary('xAODPrimitivesDict')
isoPar = ROOT.xAOD.Iso
Pt1000IsoTrackDecorator = DerivationFramework__trackIsolationDecorator(name = "Pt1000IsoTrackDecorator",
                                                                TrackIsolationTool = TrackIsoTool,
                                                                TargetContainer = "InDetTrackParticles",
                                                                ptcones = [isoPar.ptcone40,isoPar.ptcone30,isoPar.ptcone20],
                                                                Prefix = 'TrkIsoPt1000_'
                                                               )
Pt500IsoTrackDecorator = DerivationFramework__trackIsolationDecorator(name = "Pt500IsoTrackDecorator",
                                                                TrackIsolationTool = TrackIsoTool500,
                                                                TargetContainer = "InDetTrackParticles",
                                                                ptcones = [isoPar.ptcone40,isoPar.ptcone30,isoPar.ptcone20],
                                                                Prefix = 'TrkIsoPt500_'
                                                               )
ToolSvc += Pt1000IsoTrackDecorator
ToolSvc += Pt500IsoTrackDecorator

AugmentationTools.append(Pt1000IsoTrackDecorator)
AugmentationTools.append(Pt500IsoTrackDecorator)

#=======================================
# CREATE THE DERIVATION KERNEL ALGORITHM
#=======================================

from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__DerivationKernel

# Add sumOfWeights metadata for LHE3 multiweights =======
from DerivationFrameworkCore.LHE3WeightMetadata import *


#==============================================================================
# SUSY skimming selection
#==============================================================================
SeqSUSY1 += CfgMgr.DerivationFramework__DerivationKernel(
	"SUSY1KernelSkim",
	SkimmingTools = [SUSY1SkimmingTool],
        )


#==============================================================================
# Jet building
#==============================================================================
#re-tag PFlow jets so they have b-tagging info.
FlavorTagInit(JetCollections = ['AntiKt4EMPFlowJets'], Sequencer = SeqSUSY1)

## Adding decorations for fJVT PFlow jets
getPFlowfJVT(jetalg='AntiKt4EMPFlow',sequence=SeqSUSY1, algname='JetForwardPFlowJvtToolAlg')
applyMVfJvtAugmentation(jetalg='AntiKt4EMTopo',sequence=SeqSUSY1, algname='JetForwardJvtToolBDTAlg')

#==============================================================================
OutputJets["SUSY1"] = []

#reducedJetList = [ "AntiKt2PV0TrackJets", "AntiKt4PV0TrackJets", "AntiKt10LCTopoJets"]
reducedJetList = ["AntiKt2PV0TrackJets", "AntiKt4PV0TrackJets","AntiKt4TruthDressedWZJets"]

# AntiKt2PV0TrackJets is flavour-tagged automatically (AntiKt4PV0TrackJets flavour tagging not supported in R21)
replaceAODReducedJets(reducedJetList, SeqSUSY1, "SUSY1")

# AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets
addDefaultTrimmedJets(SeqSUSY1, "SUSY1")

#==============================================================================
# Soft Tagging
#==============================================================================
from InDetVKalVxInJetTool.InDetVKalVxInJetFinder import InDetVKalVxInJetFinder

SeqSUSY1 += CfgMgr.BTagVertexAugmenter()

# used by Loose and Medium
SoftBJetSVFinderTool      = InDetVKalVxInJetFinder("SoftBJetSVFinder")
ToolSvc += SoftBJetSVFinderTool
SoftBJetSVFinderTool.ConeForTag = 0.75

softTagAlgLoose = CfgMgr.SoftBVrt__SoftBVrtClusterTool(  "SoftBVrtClusterToolLoose",
                           OutputLevel=INFO, #DEBUG                                                                                          
                           )

softTagAlgLoose.SVFinderName = 'SoftBJetSVFinder'
softTagAlgLoose.TrackJetCollectionName = 'AntiKt4PV0TrackJets'
softTagAlgLoose.TrackSelectionTool.CutLevel = "LoosePrimary"
softTagAlgLoose.OperatingPoint = 'Loose'

SeqSUSY1 += softTagAlgLoose

softTagAlgMedium = CfgMgr.SoftBVrt__SoftBVrtClusterTool(  "SoftBVrtClusterToolMedium",
                           OutputLevel=INFO, #DEBUG                                                                                          
                           )

softTagAlgMedium.SVFinderName = 'SoftBJetSVFinder'
softTagAlgMedium.TrackJetCollectionName = 'AntiKt4PV0TrackJets'
softTagAlgMedium.TrackSelectionTool.CutLevel = "LoosePrimary"
softTagAlgMedium.OperatingPoint = 'Medium'

SeqSUSY1 += softTagAlgMedium

# used by Tight
SoftBJetSVFinderToolTight      = InDetVKalVxInJetFinder("SoftBJetSVFinderTight")
ToolSvc += SoftBJetSVFinderToolTight
SoftBJetSVFinderToolTight.ConeForTag = 0.6

softTagAlgTight = CfgMgr.SoftBVrt__SoftBVrtClusterTool(  "SoftBVrtClusterToolTight",
                           OutputLevel=INFO, #DEBUG                                                                                          
                           )

softTagAlgTight.SVFinderName = 'SoftBJetSVFinderTight'
softTagAlgTight.TrackJetCollectionName = 'AntiKt4PV0TrackJets'
softTagAlgTight.TrackSelectionTool.CutLevel = "LoosePrimary"
softTagAlgTight.OperatingPoint = 'Tight'

SeqSUSY1 += softTagAlgTight

#==============================================================================
# Augment after skim
#==============================================================================
SeqSUSY1 += CfgMgr.DerivationFramework__DerivationKernel(
	"SUSY1KernelAug",
	AugmentationTools = AugmentationTools,
	ThinningTools = thinningTools,
)

# Add VR jets
addVRJets(SeqSUSY1)
addVRJets(SeqSUSY1, training='201903')
addVRJets(SeqSUSY1, do_ghost=True)
addRecommendedXbbTaggers(SeqSUSY1, ToolSvc)

BTaggingFlags.CalibrationChannelAliases += ["AntiKtVR30Rmax4Rmin02Track->AntiKtVR30Rmax4Rmin02Track,AntiKt4EMTopo"]

#====================================================================
# CONTENT LIST
#====================================================================
# About SmartCollections and AllVariables:
#   If you want to use CP tools on a container, you MUST include that container as a SmartCollection.
#   You are then free to include it as AllVariables in addition, to pick up variables not needed by the CP tools but needed by your analysis.
from DerivationFrameworkCore.SlimmingHelper import SlimmingHelper
SUSY1SlimmingHelper = SlimmingHelper("SUSY1SlimmingHelper")
# BTagging_AntiKt4Track changed to BTagging_AntiKt2Track, as the former is no longer supported
SUSY1SlimmingHelper.SmartCollections = ["Electrons","Photons",
                                        "AntiKt4EMTopoJets",
                                        "AntiKt4EMPFlowJets",
                                        "MET_Reference_AntiKt4EMTopo",
                                        "MET_Reference_AntiKt4EMPFlow",
                                        "Muons",
                                        "TauJets",
                                        "InDetTrackParticles",
                                        "PrimaryVertices",
                                        "AntiKt4EMPFlowJets_BTagging201810",
                                        "AntiKt4EMPFlowJets_BTagging201903",
                                        "BTagging_AntiKt4EMPFlow_201810",
                                        "BTagging_AntiKt4EMPFlow_201903",
                                        "AntiKt4EMTopoJets_BTagging201810",
                                        "BTagging_AntiKt4EMTopo_201810",
                                        #"BTagging_AntiKt4EMTopo",
                                        #"BTagging_AntiKt4EMPFlow",
                                        # "BTagging_AntiKt2Track",
                                        "AntiKtVR30Rmax4Rmin02TrackJets_BTagging201810",
                                        "AntiKtVR30Rmax4Rmin02TrackJets_BTagging201903",
                                        "AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets"]
SUSY1SlimmingHelper.AllVariables = [
  "TruthParticles", "TruthEvents", "TruthVertices", "MET_Truth", "AntiKt4TruthDressedWZJets", "AntiKtVR30Rmax4Rmin02TrackJets_BTagging201810","AntiKtVR30Rmax4Rmin02TrackJets_BTagging201903","BTagging_AntiKtVR30Rmax4Rmin02Track_201810","BTagging_AntiKtVR30Rmax4Rmin02Track_201903",
  #"AntiKt2PV0TrackJets", "AntiKt4PV0TrackJets",
  "MET_Track"
]
SUSY1SlimmingHelper.ExtraVariables = ["Muons.etcone30.ptcone30.ptcone20.charge.quality.InnerDetectorPt.MuonSpectrometerPt.CaloLRLikelihood.CaloMuonIDTag",
				                              "Photons.author.Loose.Tight",
				                              "AntiKt4EMTopoJets.NumTrkPt1000.TrackWidthPt1000.NumTrkPt500.DFCommonJets_Calib_pt.DFCommonJets_Calib_eta.DFCommonJets_Calib_phi.Timing.DFCommonJets_jetClean_VeryLooseBadLLP",
                                      "AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets.KtDR.ZCut12.Angularity.Aplanarity.PlanarFlow.FoxWolfram2.FoxWolfram0.Dip12.Sphericity.ThrustMin.ThrustMaj",
                                      "AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets.XbbScoreHiggs.XbbScoreTop.XbbScoreQCD", #xbb variables
                                      "BTagging_AntiKtVR30Rmax4Rmin02Track_201810.MV2c10_discriminant.MV2c10mu_discriminant.MV2c10rnn_discriminant.DL1_pu.DL1_pc.DL1_pb.DL1rmu_pu.DL1rmu_pc.DL1rmu_pb.DL1r_pu.DL1r_pc.DL1r_pb",
                                      "AntiKt10TruthTrimmedPtFrac5SmallR20Jets.pt.eta.phi.m.GhostBHadronsFinalCount",
                                      # TODO: .DFCommonJets_Jvt",
				                              "GSFTrackParticles.z0.d0.vz.definingParametersCovMatrix",
				                              "CombinedMuonTrackParticles.d0.z0.vz.definingParametersCovMatrix.truthOrigin.truthType",
				                              "ExtrapolatedMuonTrackParticles.d0.z0.vz.definingParametersCovMatrix.truthOrigin.truthType",
				                              "TauJets.IsTruthMatched.truthOrigin.truthType.truthParticleLink.truthJetLink",
				                              "MuonTruthParticles.barcode.decayVtxLink.e.m.pdgId.prodVtxLink.px.py.pz.recoMuonLink.status.truthOrigin.truthType",
				                              "AntiKt4TruthJets.eta.m.phi.pt.TruthLabelDeltaR_B.TruthLabelDeltaR_C.TruthLabelDeltaR_T.TruthLabelID.ConeTruthLabelID.PartonTruthLabelID",
                                      "InDetTrackParticles.TrkIsoPt1000_ptcone20.TrkIsoPt1000_ptcone30.TrkIsoPt1000_ptcone40.TrkIsoPt500_ptcone20.TrkIsoPt500_ptcone30.TrkIsoPt500_ptcone40",
                                      "HLT_xAOD__BTaggingContainer_HLTBjetFex.MV2c20_discriminant.MV2c10_discriminant.COMB.BTagBtagToJetAssociator",
#                                      "HLT_xAOD__BTaggingContainer_HLTBjetFex.MV2c20_discriminant.MV2c10_discriminant.COMB",
                                      "HLT_xAOD__JetContainer_SplitJet.pt.eta.phi.m",
                                    ]

SUSY1SlimmingHelper.IncludeMuonTriggerContent   = True
SUSY1SlimmingHelper.IncludeEGammaTriggerContent = True
SUSY1SlimmingHelper.IncludeBPhysTriggerContent  = False
SUSY1SlimmingHelper.IncludeJetTriggerContent    = True
SUSY1SlimmingHelper.IncludeTauTriggerContent    = True
SUSY1SlimmingHelper.IncludeEtMissTriggerContent = True
SUSY1SlimmingHelper.IncludeBJetTriggerContent   = False

excludedVertexAuxData = "-vxTrackAtVertex.-MvfFitInfo.-isInitialized.-VTAV"

StaticContent = []
StaticContent += ["xAOD::VertexContainer#SoftBVrtClusterTool_Tight_Vertices"]
StaticContent += ["xAOD::VertexAuxContainer#SoftBVrtClusterTool_Tight_VerticesAux." + excludedVertexAuxData]
StaticContent += ["xAOD::VertexContainer#SoftBVrtClusterTool_Medium_Vertices"]
StaticContent += ["xAOD::VertexAuxContainer#SoftBVrtClusterTool_Medium_VerticesAux." + excludedVertexAuxData]
StaticContent += ["xAOD::VertexContainer#SoftBVrtClusterTool_Loose_Vertices"]
StaticContent += ["xAOD::VertexAuxContainer#SoftBVrtClusterTool_Loose_VerticesAux." + excludedVertexAuxData]

SUSY1SlimmingHelper.StaticContent = StaticContent

appendToDictDict = {
  }

if DerivationFrameworkHasTruth:
  # Most of the new containers are centrally added to SlimmingHelper via DerivationFrameworkCore ContainersOnTheFly.py
  appendToDictDict['TruthTop']='xAOD::TruthParticleContainer';
  appendToDictDict['TruthTopAux']='xAOD::TruthParticleAuxContainer';

  appendToDictDict['TruthBSM']='xAOD::TruthParticleContainer';
  appendToDictDict['TruthBSMAux']='xAOD::TruthParticleAuxContainer';

  appendToDictDict['TruthBoson']='xAOD::TruthParticleContainer';
  appendToDictDict['TruthBosonAux']='xAOD::TruthParticleAuxContainer'

  # All standard truth particle collections are provided by DerivationFrameworkMCTruth (TruthDerivationTools.py)
  SUSY1SlimmingHelper.AllVariables += ["TruthElectrons", "TruthMuons", "TruthTaus", "TruthPhotons", "TruthNeutrinos", "TruthTop", "TruthBSM", "TruthBoson"]

SUSY1SlimmingHelper.AppendToDictionary = appendToDictDict

SUSY1SlimmingHelper.AppendContentToStream(SUSY1Stream)

