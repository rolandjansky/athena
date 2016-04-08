#********************************************************************
# SUSY7.py 
# reductionConf flag SUSY7 in Reco_tf.py   
#********************************************************************

from DerivationFrameworkCore.DerivationFrameworkMaster import *
from DerivationFrameworkJetEtMiss.JetCommon import *
from DerivationFrameworkJetEtMiss.ExtendedJetCommon import *
from DerivationFrameworkEGamma.EGammaCommon import *
from DerivationFrameworkMuons.MuonsCommon import *
from DerivationFrameworkTau.TauTruthCommon import *
from DerivationFrameworkInDet.InDetCommon import *
from DerivationFrameworkJetEtMiss.METCommon import *
from DerivationFrameworkFlavourTag.FlavourTagCommon import *


### Set up stream
streamName = derivationFlags.WriteDAOD_SUSY7Stream.StreamName
fileName   = buildFileName( derivationFlags.WriteDAOD_SUSY7Stream )
SUSY7Stream = MSMgr.NewPoolRootStream( streamName, fileName )
SUSY7Stream.AcceptAlgs(["SUSY7KernelSkim"])

### Init
from DerivationFrameworkCore.ThinningHelper import ThinningHelper
SUSY7ThinningHelper = ThinningHelper( "SUSY7ThinningHelper" )
thinningTools       = []
AugmentationTools   = []

# stream-specific sequence for on-the-fly jet building
SeqSUSY7 = CfgMgr.AthSequencer("SeqSUSY7")
DerivationFrameworkJob += SeqSUSY7


#====================================================================
# Trigger navigation thinning
#====================================================================
SUSY7ThinningHelper.TriggerChains = 'HLT_e.*|HLT_g.*|HLT_mu.*'
SUSY7ThinningHelper.AppendToStream( SUSY7Stream )


#====================================================================
# THINNING TOOL 
#====================================================================\

# MET/Jet tracks -> no longer needed, 11.05.2015

from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__TrackParticleThinning

SUSY7TPThinningTool = DerivationFramework__TrackParticleThinning(name = "SUSY7TPThinningTool",
								 ThinningService	 = SUSY7ThinningHelper.ThinningSvc(),
								 SelectionString	 = "InDetTrackParticles.pt > 10*GeV",
								 InDetTrackParticlesKey  = "InDetTrackParticles")
ToolSvc += SUSY7TPThinningTool
thinningTools.append(SUSY7TPThinningTool)

# TrackParticles associated with Muons
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__MuonTrackParticleThinning
SUSY7MuonTPThinningTool = DerivationFramework__MuonTrackParticleThinning(name			 = "SUSY7MuonTPThinningTool",
									 ThinningService	 = SUSY7ThinningHelper.ThinningSvc(),
									 MuonKey		 = "Muons",
									 InDetTrackParticlesKey  = "InDetTrackParticles")
ToolSvc += SUSY7MuonTPThinningTool
thinningTools.append(SUSY7MuonTPThinningTool)

# TrackParticles associated with electrons
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__EgammaTrackParticleThinning
SUSY7ElectronTPThinningTool = DerivationFramework__EgammaTrackParticleThinning(name			 = "SUSY7ElectronTPThinningTool",
										 ThinningService	 = SUSY7ThinningHelper.ThinningSvc(),
										 SGKey			 = "Electrons",
										 InDetTrackParticlesKey  = "InDetTrackParticles")
ToolSvc += SUSY7ElectronTPThinningTool
thinningTools.append(SUSY7ElectronTPThinningTool)

# TrackParticles associated with photons
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__EgammaTrackParticleThinning
SUSY7PhotonTPThinningTool = DerivationFramework__EgammaTrackParticleThinning(name			 = "SUSY7PhotonTPThinningTool",
										 ThinningService	 = SUSY7ThinningHelper.ThinningSvc(),
										 SGKey			 = "Photons",
										 InDetTrackParticlesKey  = "InDetTrackParticles")
ToolSvc += SUSY7PhotonTPThinningTool
thinningTools.append(SUSY7PhotonTPThinningTool)

# TrackParticles associated with taus
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__TauTrackParticleThinning
SUSY7TauTPThinningTool = DerivationFramework__TauTrackParticleThinning( name		= "SUSY7TauTPThinningTool",
									ThinningService = SUSY7ThinningHelper.ThinningSvc(),
									TauKey  	= "TauJets",
									InDetTrackParticlesKey  = "InDetTrackParticles")
ToolSvc += SUSY7TauTPThinningTool
thinningTools.append(SUSY7TauTPThinningTool)

# Calo Clusters associated with Photons
#from DerivationFrameworkCalo.DerivationFrameworkCaloConf import DerivationFramework__CaloClusterThinning
#SUSY7PhotonCCThinningTool = DerivationFramework__CaloClusterThinning( name                    = "SUSY7PhotonCCThinningTool",
#                                                                                     ThinningService         = SUSY7ThinningHelper.ThinningSvc(),
#                                                                                     SGKey                   = "Photons",
#                                                                                     CaloClCollectionSGKey   = "egammaClusters",
#                                                                                     TopoClCollectionSGKey   = "CaloCalTopoClusters",
#                                                                                     SelectionString         = "Photons.pt > 10*GeV",
                                                                                     #FrwdClCollectionSGKey   = "ForwardElectronClusters",
#                                                                                     ConeSize                = 0.6)
#ToolSvc += SUSY7PhotonCCThinningTool
#thinningTools.append(SUSY7PhotonCCThinningTool)

# Calo Clusters associated with Electrons
#SUSY7ElectronCCThinningTool = DerivationFramework__CaloClusterThinning( name                  = "SUSY7ElectronCCThinningTool",
#                                                                                     ThinningService         = SUSY7ThinningHelper.ThinningSvc(),
#                                                                                     SGKey                   = "Electrons",
#                                                                                     CaloClCollectionSGKey   = "egammaClusters",
#                                                                                     TopoClCollectionSGKey   = "CaloCalTopoClusters",
#                                                                                     SelectionString         = "Electrons.pt > 7*GeV",
                                                                                     #FrwdClCollectionSGKey   = "ForwardElectronClusters",
#                                                                                     ConeSize                = 0.4)
#ToolSvc += SUSY7ElectronCCThinningTool
#thinningTools.append(SUSY7ElectronCCThinningTool)

# Calo Clusters associated with jets
#from DerivationFrameworkEGamma.DerivationFrameworkEGammaConf import DerivationFramework__JetCaloClusterThinning
#SUSY7aKt4CCThinningTool = DerivationFramework__JetCaloClusterThinning(name                    = "SUSY7aKt4CCThinningTool",
#                                                                     ThinningService         = SUSY7ThinningHelper.ThinningSvc(),
#                                                                      SGKey                   = "AntiKt4LCTopoJets",
#                                                                      TopoClCollectionSGKey   = "CaloCalTopoClusters",
#                                                                      SelectionString         = "AntiKt4LCTopoJets.pt > 20*GeV",
#                                                                      ConeSize                = 0)
#ToolSvc += SUSY7aKt4CCThinningTool
#thinningTools.append(SUSY7aKt4CCThinningTool)

#====================================================================
# TRUTH THINNING
#====================================================================
if DerivationFrameworkIsMonteCarlo:
  from DerivationFrameworkMCTruth.DerivationFrameworkMCTruthConf import DerivationFramework__MenuTruthThinning
  SUSY7TruthThinningTool = DerivationFramework__MenuTruthThinning(name              = "SUSY7TruthThinningTool",
                                                       ThinningService              = SUSY7ThinningHelper.ThinningSvc(),
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
  ToolSvc += SUSY7TruthThinningTool
  thinningTools.append(SUSY7TruthThinningTool)

applyJetCalibration_xAODColl("AntiKt4EMTopo")
updateJVT_xAODColl("AntiKt4EMTopo")
applyBTagging_xAODColl('AntiKt4EMTopo')

### skimming 

muonsRequirements = '(Muons.pt >= 6.*GeV) && (abs(Muons.eta) < 2.6) && (Muons.DFCommonGoodMuon)'
electronsRequirements = '(Electrons.pt > 7.*GeV) && (abs(Electrons.eta) < 2.6) && ((Electrons.Loose) || (Electrons.DFCommonElectronsLHLoose))'

# cut value from https://twiki.cern.ch/twiki/bin/view/AtlasProtected/BTaggingBenchmarks#b_tagging_Benchmarks_for_tag_AN1
btagexpression = "count((AntiKt4EMTopoJets.DFCommonJets_FlatBEff_77 || AntiKt4EMTopoJets.DFCommonJets_FixedCutBEff_80) && (AntiKt4EMTopoJets.DFCommonJets_Calib_pt>50.*GeV))>0"

skimexpr = "(count(%s) + count (%s) > 1) || (%s)" % (muonsRequirements, electronsRequirements, btagexpression)

from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__xAODStringSkimmingTool
SUSY7btagSkimmingTool = DerivationFramework__xAODStringSkimmingTool( name = "SUSY7btagSkimmingTool",
								     expression = skimexpr)

ToolSvc += SUSY7btagSkimmingTool


#=======================================
# CREATE THE DERIVATION KERNEL ALGORITHM   
#=======================================
from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__DerivationKernel
DerivationFrameworkJob += CfgMgr.DerivationFramework__DerivationKernel(
  "SUSY7KernelSkim",
  SkimmingTools = [SUSY7btagSkimmingTool]
)

#==============================================================================
# SUSY signal augmentation
#==============================================================================
from DerivationFrameworkSUSY.DecorateSUSYProcess import DecorateSUSYProcess
AugmentationTools += DecorateSUSYProcess("SUSY7")

#==============================================================================
# SUSY background generator filters
#==============================================================================
if globalflags.DataSource() == 'geant4':
  replaceBuggyAntiKt4TruthWZJets(SeqSUSY7)
  ToolSvc += CfgMgr.DerivationFramework__SUSYGenFilterTool(
    "SUSY7GenFilt",
    SimBarcodeOffset = DerivationFrameworkSimBarcodeOffset
  )
  AugmentationTools.append(ToolSvc.SUSY7GenFilt)

#==============================================================================
# Augment after skim
#==============================================================================
SeqSUSY7 += CfgMgr.DerivationFramework__DerivationKernel(
	"SUSY7KernelAug",
	AugmentationTools = AugmentationTools,
	ThinningTools = thinningTools,
)


#====================================================================
# CONTENT LIST  
#====================================================================
# This might be the kind of set-up one would have for a muon based analysis
from DerivationFrameworkCore.SlimmingHelper import SlimmingHelper
SUSY7SlimmingHelper = SlimmingHelper("SUSY7SlimmingHelper")
SUSY7SlimmingHelper.SmartCollections = ["Electrons","Photons","MET_Reference_AntiKt4EMTopo","Muons","TauJets","AntiKt4EMTopoJets", "BTagging_AntiKt4EMTopo", "InDetTrackParticles", "PrimaryVertices"]
SUSY7SlimmingHelper.AllVariables = ["TruthParticles", "TruthEvents", "TruthVertices", "MET_Truth"]				
SUSY7SlimmingHelper.ExtraVariables = ["BTagging_AntiKt4EMTopo.MV1_discriminant.MV1c_discriminant",
				      "Muons.ptcone30.ptcone20.charge.quality.InnerDetectorPt.MuonSpectrometerPt.CaloLRLikelihood.CaloMuonIDTag",
				      "Photons.author.Loose.Tight",
				      "AntiKt4EMTopoJets.NumTrkPt1000.TrackWidthPt1000.NumTrkPt500.DFCommonJets_Calib_pt.DFCommonJets_FixedCutBEff_30.DFCommonJets_FixedCutBEff_30.DFCommonJets_FixedCutBEff_50.DFCommonJets_FixedCutBEff_60.DFCommonJets_FixedCutBEff_70.DFCommonJets_FixedCutBEff_77.DFCommonJets_FixedCutBEff_80.DFCommonJets_FixedCutBEff_85.DFCommonJets_FixedCutBEff_90.DFCommonJets_FlatBEff_30.DFCommonJets_FlatBEff_40.DFCommonJets_FlatBEff_50.DFCommonJets_FlatBEff_60.DFCommonJets_FlatBEff_70.DFCommonJets_FlatBEff_77.DFCommonJets_FlatBEff_85'",
				      "GSFTrackParticles.z0.d0.vz.definingParametersCovMatrix",
				      "CombinedMuonTrackParticles.d0.z0.vz.definingParametersCovMatrix.truthOrigin.truthType",
				      "ExtrapolatedMuonTrackParticles.d0.z0.vz.definingParametersCovMatrix.truthOrigin.truthType",
				      "TauJets.TruthCharge.TruthProng.IsTruthMatched.TruthPtVis.truthOrigin.truthType.truthParticleLink.truthJetLink",
				      "MuonTruthParticles.barcode.decayVtxLink.e.m.pdgId.prodVtxLink.px.py.pz.recoMuonLink.status.truthOrigin.truthType",
				      "AntiKt4TruthJets.eta.m.phi.pt.TruthLabelDeltaR_B.TruthLabelDeltaR_C.TruthLabelDeltaR_T.TruthLabelID.ConeTruthLabelID.PartonTruthLabelID.HadronConeExclTruthLabelID"]
SUSY7SlimmingHelper.IncludeMuonTriggerContent = True
SUSY7SlimmingHelper.IncludeEGammaTriggerContent = True
SUSY7SlimmingHelper.IncludeBPhysTriggerContent = False
SUSY7SlimmingHelper.IncludeJetTriggerContent = True
SUSY7SlimmingHelper.IncludeTauTriggerContent = False
SUSY7SlimmingHelper.IncludeEtMissTriggerContent = True
#addJetOutputs(SUSY7SlimmingHelper,["LargeR"])

SUSY7SlimmingHelper.AppendContentToStream(SUSY7Stream)
