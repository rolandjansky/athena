#*****************************************************************
# SUSY11.py
# reductionConf flag SUSY11 in Reco_tf.py (jet smearing)
#********************************************************************

from DerivationFrameworkCore.DerivationFrameworkMaster import *
from DerivationFrameworkJetEtMiss.JetCommon import *
from DerivationFrameworkJetEtMiss.ExtendedJetCommon import *
from DerivationFrameworkEGamma.EGammaCommon import *
from DerivationFrameworkMuons.MuonsCommon import *
from DerivationFrameworkTau.TauCommon import *
if DerivationFrameworkIsMonteCarlo:
  from DerivationFrameworkMCTruth.MCTruthCommon import addStandardTruthContents,addMiniTruthCollectionLinks,addPVCollection
  addStandardTruthContents()
  addMiniTruthCollectionLinks()
  addPVCollection()
from DerivationFrameworkInDet.InDetCommon import *
from DerivationFrameworkJetEtMiss.METCommon import *
from DerivationFrameworkFlavourTag.HbbCommon import *
from DerivationFrameworkFlavourTag.FlavourTagCommon import *

### Set up stream
streamName = derivationFlags.WriteDAOD_SUSY11Stream.StreamName
fileName   = buildFileName( derivationFlags.WriteDAOD_SUSY11Stream )
SUSY11Stream = MSMgr.NewPoolRootStream( streamName, fileName )
SUSY11Stream.AcceptAlgs(["SUSY11KernelSkim"])

### Init
from DerivationFrameworkCore.ThinningHelper import ThinningHelper
SUSY11ThinningHelper = ThinningHelper( "SUSY11ThinningHelper" )
thinningTools       = []
AugmentationTools   = []

# stream-specific sequence for on-the-fly jet building
SeqSUSY11 = CfgMgr.AthSequencer("SeqSUSY11")
DerivationFrameworkJob += SeqSUSY11


#====================================================================
# Trigger navigation thinning
#====================================================================
from DerivationFrameworkSUSY.SUSY11TriggerList import SUSY11TriggerThinningList
SUSY11ThinningHelper.TriggerChains = '|'.join(SUSY11TriggerThinningList)

SUSY11ThinningHelper.AppendToStream( SUSY11Stream )


#====================================================================
# THINNING TOOL
#====================================================================\

# TrackParticles associated with Muons
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__MuonTrackParticleThinning
SUSY11MuonTPThinningTool = DerivationFramework__MuonTrackParticleThinning(name			 = "SUSY11MuonTPThinningTool",
									 ThinningService	 = SUSY11ThinningHelper.ThinningSvc(),
									 MuonKey		 = "Muons",
									 InDetTrackParticlesKey  = "InDetTrackParticles")
ToolSvc += SUSY11MuonTPThinningTool
thinningTools.append(SUSY11MuonTPThinningTool)

# TrackParticles associated with electrons
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__EgammaTrackParticleThinning
SUSY11ElectronTPThinningTool = DerivationFramework__EgammaTrackParticleThinning(name			 = "SUSY11ElectronTPThinningTool",
										 ThinningService	 = SUSY11ThinningHelper.ThinningSvc(),
										 SGKey			 = "Electrons",
										 InDetTrackParticlesKey  = "InDetTrackParticles")
ToolSvc += SUSY11ElectronTPThinningTool
thinningTools.append(SUSY11ElectronTPThinningTool)

# TrackParticles associated with taus, only keep charged tau tracks
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__TauTrackParticleThinning
SUSY11TauTPThinningTool = DerivationFramework__TauTrackParticleThinning(name                   = "SUSY11TauTPThinningTool",
                                                                        ThinningService        = SUSY11ThinningHelper.ThinningSvc(),
                                                                        TauKey                 = "TauJets",
                                                                        InDetTrackParticlesKey = "InDetTrackParticles",
                                                                        SelectionString        = "(TauJets.ptFinalCalib >= 15.*GeV) && (TauJets.nTracks==1 || TauJets.nTracks==3)",
                                                                        ApplyAnd               = False,
                                                                        DoTauTracksThinning    = True,
                                                                        TauTracksKey           = "TauTracks")
ToolSvc += SUSY11TauTPThinningTool
thinningTools.append(SUSY11TauTPThinningTool)

# TauJets thinning
from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__GenericObjectThinning
SUSY11TauJetsThinningTool = DerivationFramework__GenericObjectThinning(name            = "SUSY11TauJetsThinningTool",
                                                                       ThinningService = SUSY11ThinningHelper.ThinningSvc(),
                                                                       ContainerName   = "TauJets",
                                                                       SelectionString = "(TauJets.ptFinalCalib >= 15.*GeV) && (TauJets.nTracks==1 || TauJets.nTracks==3)")
ToolSvc += SUSY11TauJetsThinningTool
thinningTools.append(SUSY11TauJetsThinningTool)


#====================================================================
# TRUTH THINNING
#====================================================================

if DerivationFrameworkIsMonteCarlo:

  # Decorate Electron with bkg electron type/origin
  from MCTruthClassifier.MCTruthClassifierBase import MCTruthClassifier as BkgElectronMCTruthClassifier
  from DerivationFrameworkEGamma.DerivationFrameworkEGammaConf import DerivationFramework__BkgElectronClassification
  BkgElectronClassificationTool = DerivationFramework__BkgElectronClassification (name = "BkgElectronClassificationTool",MCTruthClassifierTool = BkgElectronMCTruthClassifier)
  ToolSvc += BkgElectronClassificationTool
  AugmentationTools.append(BkgElectronClassificationTool)


#====================================================================
# TRIGGER SKIMMING
#====================================================================

from DerivationFrameworkSUSY.SUSY11TriggerList import SUSY11TriggerSkimmingList
from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__TriggerSkimmingTool
SUSY11SkimmingTool = DerivationFramework__TriggerSkimmingTool(
    name          = "SUSY11SkimmingTool",
    TriggerListOR = SUSY11TriggerSkimmingList )

ToolSvc += SUSY11SkimmingTool


#=======================================
# CREATE THE DERIVATION KERNEL ALGORITHM
#=======================================
from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__DerivationKernel

# Add sumOfWeights metadata for LHE3 multiweights =======
from DerivationFrameworkCore.LHE3WeightMetadata import *

#==============================================================================
# SUSY signal augmentation (before skimming!)
#==============================================================================
from DerivationFrameworkSUSY.DecorateSUSYProcess import IsSUSYSignal
if IsSUSYSignal():

   from DerivationFrameworkSUSY.DecorateSUSYProcess import DecorateSUSYProcess
   SeqSUSY11 += CfgMgr.DerivationFramework__DerivationKernel("SUSY11KernelSigAug",
                                                            AugmentationTools = DecorateSUSYProcess("SUSY11")
                                                            )

   from DerivationFrameworkSUSY.SUSYWeightMetadata import *


#==============================================================================
# SUSY skimming selection
#==============================================================================
SeqSUSY11 += CfgMgr.DerivationFramework__DerivationKernel(
  "SUSY11KernelSkim",
  SkimmingTools = [SUSY11SkimmingTool],
)


#==============================================================================
# Jet building
#==============================================================================
#re-tag PFlow jets so they have b-tagging info.
FlavorTagInit(JetCollections = ['AntiKt4EMPFlowJets'], Sequencer = SeqSUSY11)

## Adding decorations for fJVT PFlow jets                                                                                                                                                                   
getPFlowfJVT(jetalg='AntiKt4EMPFlow',sequence=SeqSUSY11, algname='JetForwardPFlowJvtToolAlg')
applyMVfJvtAugmentation(jetalg='AntiKt4EMTopo',sequence=SeqSUSY11, algname='JetForwardJvtToolBDTAlg')

reducedJetList = ["AntiKt2PV0TrackJets", "AntiKt4PV0TrackJets"]
replaceAODReducedJets(reducedJetList, SeqSUSY11, "SUSY11")
addDefaultTrimmedJets(SeqSUSY11, "SUSY11")
addVRJets(SeqSUSY11)
addVRJets(SeqSUSY11, training='201903')
addVRJets(SeqSUSY11, do_ghost=True)

BTaggingFlags.CalibrationChannelAliases += ["AntiKtVR30Rmax4Rmin02Track->AntiKtVR30Rmax4Rmin02Track,AntiKt4EMTopo"]


#==============================================================================
# Augment after skim
#==============================================================================
SeqSUSY11 += CfgMgr.DerivationFramework__DerivationKernel(
	"SUSY11KernelAug",
	AugmentationTools = AugmentationTools,
	ThinningTools = thinningTools,
)


#====================================================================
# CONTENT LIST
#====================================================================

from DerivationFrameworkCore.SlimmingHelper import SlimmingHelper
SUSY11SlimmingHelper = SlimmingHelper("SUSY11SlimmingHelper")
SUSY11SlimmingHelper.SmartCollections = ["Electrons", "Photons", "MET_Reference_AntiKt4EMTopo", "MET_Reference_AntiKt4EMPFlow",
                                         "Muons", "AntiKt4EMTopoJets", "AntiKt4EMPFlowJets", 
                                         "AntiKt4EMPFlowJets_BTagging201810",
                                         "AntiKt4EMPFlowJets_BTagging201903",
                                         "BTagging_AntiKt4EMPFlow_201810",
                                         "BTagging_AntiKt4EMPFlow_201903",
                                         "AntiKt4EMTopoJets_BTagging201810",
                                         "BTagging_AntiKt4EMTopo_201810","PrimaryVertices", "TauJets",
                                         "AntiKtVR30Rmax4Rmin02TrackJets_BTagging201810",
                                         "AntiKtVR30Rmax4Rmin02TrackJets_BTagging201903",
                                         "AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets"]
SUSY11SlimmingHelper.AllVariables = [ "MET_Truth", "MET_Track", "AntiKtVR30Rmax4Rmin02TrackJets_BTagging201810","AntiKtVR30Rmax4Rmin02TrackJets_BTagging201903","BTagging_AntiKtVR30Rmax4Rmin02Track_201810","BTagging_AntiKtVR30Rmax4Rmin02Track_201903" ]
SUSY11SlimmingHelper.ExtraVariables = ["BTagging_AntiKt4EMTopo_201810.MV1_discriminant.MV1c_discriminant",
                                       "Electrons.truthOrigin.truthType.bkgMotherPdgId.bkgTruthOrigin.bkgTruthType.firstEgMotherTruthType.firstEgMotherTruthOrigin.firstEgMotherPdgId.TruthLink",
                                       "Muons.ptcone30.ptcone20.charge.quality.InnerDetectorPt.MuonSpectrometerPt.CaloLRLikelihood.CaloMuonIDTag.TruthLink",
                                       "Photons.TruthLink",                                       
                                       "AntiKt4EMTopoJets.NumTrkPt1000.TrackWidthPt1000.NumTrkPt500.N90Constituents.Timing.Width.DFCommonJets_jetClean_VeryLooseBadLLP",
                                       "GSFTrackParticles.z0.d0.vz.definingParametersCovMatrix",
                                       "CombinedMuonTrackParticles.d0.z0.vz.definingParametersCovMatrix.truthOrigin.truthType",
                                       "ExtrapolatedMuonTrackParticles.d0.z0.vz.definingParametersCovMatrix.truthOrigin.truthType",
                                       "MuonTruthParticles.barcode.decayVtxLink.e.m.pdgId.prodVtxLink.px.py.pz.recoMuonLink.status.truthOrigin.truthType",
                                       "AntiKt4TruthJets.eta.m.phi.pt.TruthLabelDeltaR_B.TruthLabelDeltaR_C.TruthLabelDeltaR_T.TruthLabelID.ConeTruthLabelID.PartonTruthLabelID",
                                       "BTagging_AntiKtVR30Rmax4Rmin02Track_201810.MV2c10_discriminant.DL1_pu.DL1_pc.DL1_pb",
                                       "AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets.KtDR.ZCut12.Angularity.Aplanarity.PlanarFlow.FoxWolfram2.FoxWolfram0.Dip12.Sphericity.ThrustMin.ThrustMaj",
                                       "AntiKt10TruthTrimmedPtFrac5SmallR20Jets.pt.eta.phi.m",
                                       "TauJets.IsTruthMatched.truthOrigin.truthType.truthParticleLink.truthJetLink.seedTrackWidthPt500.seedTrackWidthPt1000"
                                       # Run2 tau reco - updated for R21 EDM
                                       + ".PanTau_isPanTauCandidate.ptPanTauCellBased.etaPanTauCellBased.phiPanTauCellBased.mPanTauCellBased"
                                       + ".PanTau_BDTValue_1p0n_vs_1p1n.PanTau_BDTValue_1p1n_vs_1pXn.PanTau_BDTValue_3p0n_vs_3pXn"
                                       # TauID variables - check R21 EDM
                                       + ".centFrac.etOverPtLeadTrk.innerTrkAvgDist.ipSigLeadTrk.SumPtTrkFrac.ptRatioEflowApprox.mEflowApprox.ChPiEMEOverCaloEME.EMPOverTrkSysP.dRmax.trFlightPathSig.massTrkSys",
                                       # Neutral tau decay products - check R21 EDM
                                       "TauNeutralParticleFlowObjects.pt.eta.phi.m.e.rapidity.bdtPi0Score"]
SUSY11SlimmingHelper.IncludeMuonTriggerContent = False
SUSY11SlimmingHelper.IncludeEGammaTriggerContent = False
SUSY11SlimmingHelper.IncludeJetTriggerContent = True
SUSY11SlimmingHelper.IncludeTauTriggerContent = True
SUSY11SlimmingHelper.IncludeEtMissTriggerContent = False
SUSY11SlimmingHelper.IncludeBJetTriggerContent = False

# All standard truth particle collections are provided by DerivationFrameworkMCTruth (TruthDerivationTools.py)
# Most of the new containers are centrally added to SlimmingHelper via DerivationFrameworkCore ContainersOnTheFly.py

if DerivationFrameworkIsMonteCarlo:

  SUSY11SlimmingHelper.AppendToDictionary = { 'TruthTop':'xAOD::TruthParticleContainer','TruthTopAux':'xAOD::TruthParticleAuxContainer',
                                              'TruthBSM':'xAOD::TruthParticleContainer','TruthBSMAux':'xAOD::TruthParticleAuxContainer',
                                              'TruthBoson':'xAOD::TruthParticleContainer','TruthBosonAux':'xAOD::TruthParticleAuxContainer',
                                              'TruthPrimaryVertices':'xAOD::TruthVertexContainer','TruthPrimaryVerticesAux':'xAOD::TruthVertexAuxContainer' }

  SUSY11SlimmingHelper.AllVariables += ["TruthElectrons", "TruthMuons", "TruthTaus", "TruthPhotons", "TruthNeutrinos", "TruthTop", "TruthBSM", "TruthBoson", "TruthPrimaryVertices"]

#SUSY11Stream.RemoveItem("xAOD::TrigNavigation#*")
#SUSY11Stream.RemoveItem("xAOD::TrigNavigationAuxInfo#*")

SUSY11SlimmingHelper.AppendContentToStream(SUSY11Stream)
