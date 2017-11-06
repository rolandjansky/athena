#*****************************************************************
# SUSY11.py
# reductionConf flag SUSY11 in Reco_tf.py (jet smearing)
#********************************************************************

from DerivationFrameworkCore.DerivationFrameworkMaster import *
from DerivationFrameworkJetEtMiss.JetCommon import *
from DerivationFrameworkJetEtMiss.ExtendedJetCommon import *
from DerivationFrameworkEGamma.EGammaCommon import *
from DerivationFrameworkMuons.MuonsCommon import *
if DerivationFrameworkIsMonteCarlo:
  from DerivationFrameworkMCTruth.MCTruthCommon import addStandardTruthContents
  addStandardTruthContents()
from DerivationFrameworkInDet.InDetCommon import *
from DerivationFrameworkJetEtMiss.METCommon import *

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
# To be added.
SUSY11ThinningHelper.AppendToStream( SUSY11Stream )


#====================================================================
# THINNING TOOL
#====================================================================\

# B.M.: likely not used
#from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__TrackParticleThinning

#SUSY11TPThinningTool = DerivationFramework__TrackParticleThinning(name = "SUSY11TPThinningTool",
#								 ThinningService	 = SUSY11ThinningHelper.ThinningSvc(),
#								 SelectionString	 = "InDetTrackParticles.pt > 10*GeV",
#								 InDetTrackParticlesKey  = "InDetTrackParticles")
#ToolSvc += SUSY11TPThinningTool
#thinningTools.append(SUSY11TPThinningTool)

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

# TrackParticles associated with taus
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__TauTrackParticleThinning
SUSY11TauTPThinningTool = DerivationFramework__TauTrackParticleThinning( name            = "SUSY11TauTPThinningTool",
                                                                        ThinningService	 = SUSY11ThinningHelper.ThinningSvc(),
                                                                        TauKey          = "TauJets",
                                                                        InDetTrackParticlesKey  = "InDetTrackParticles")
ToolSvc += SUSY11TauTPThinningTool
thinningTools.append(SUSY11TauTPThinningTool)


#====================================================================
# TRUTH THINNING
#====================================================================

if DerivationFrameworkIsMonteCarlo:
  truth_cond_Lepton = "((abs(TruthParticles.pdgId) >= 11) && (abs(TruthParticles.pdgId) <= 16))"		# Leptons

  from DerivationFrameworkMCTruth.DerivationFrameworkMCTruthConf import DerivationFramework__GenericTruthThinning
  SUSY11TruthThinningTool = DerivationFramework__GenericTruthThinning( 
                      name                         = "SUSY11TruthThinningTool",
                      ThinningService	             = SUSY11ThinningHelper.ThinningSvc(),
                      ParticleSelectionString      = truth_cond_Lepton,
                      PreserveDescendants          = True,
                      PreserveGeneratorDescendants = False,
                      SimBarcodeOffset             = DerivationFrameworkSimBarcodeOffset
                      )
  ToolSvc += SUSY11TruthThinningTool
  thinningTools.append(SUSY11TruthThinningTool)

#====================================================================
# TRIGGER SKIMMING
#====================================================================

jettrig = '( HLT_j15 || HLT_j25 || HLT_j35 || HLT_j55 || HLT_j60 || HLT_j85 || HLT_j110 || HLT_j150 || HLT_j175 || HLT_j200 || HLT_j260 || HLT_j300 || HLT_j320 || HLT_j360 || HLT_j380 || HLT_j400 || HLT_j420 || HLT_j440 || HLT_j460)'


from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__xAODStringSkimmingTool
SUSY11SkimmingTool = DerivationFramework__xAODStringSkimmingTool( name = "SUSY11SkimmingTool",
                                                                expression = jettrig)

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
# now part of MCTruthCommon
#if DerivationFrameworkIsMonteCarlo:
#
#  OutputJets["SUSY11"] = []
#  reducedJetList = [ "AntiKt4TruthJets", "AntiKt4TruthWZJets" ]
#
#  replaceAODReducedJets(reducedJetList, SeqSUSY11, "SUSY11")


#==============================================================================
# Tau truth building/matching
#==============================================================================
# now part of MCTruthCommon
#if DerivationFrameworkIsMonteCarlo:
#  from DerivationFrameworkSUSY.SUSYTruthCommon import addTruthTaus
#  addTruthTaus(AugmentationTools)


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
# This might be the kind of set-up one would have for a muon based analysis
from DerivationFrameworkCore.SlimmingHelper import SlimmingHelper
SUSY11SlimmingHelper = SlimmingHelper("SUSY11SlimmingHelper")
SUSY11SlimmingHelper.SmartCollections = ["Electrons", "Photons", "MET_Reference_AntiKt4EMTopo", "Muons", "AntiKt4EMTopoJets", "BTagging_AntiKt4EMTopo", "PrimaryVertices", "TauJets"]
SUSY11SlimmingHelper.AllVariables = [ "TruthParticles", "TruthEvents", "TruthVertices", "MET_Truth", "MET_Track"]
SUSY11SlimmingHelper.ExtraVariables = ["BTagging_AntiKt4EMTopo.MV1_discriminant.MV1c_discriminant",
                                       "Muons.ptcone30.ptcone20.charge.quality.InnerDetectorPt.MuonSpectrometerPt.CaloLRLikelihood.CaloMuonIDTag",
                                       "AntiKt4EMTopoJets.NumTrkPt1000.TrackWidthPt1000.NumTrkPt500.N90Constituents.Timing.Width",
                                       "GSFTrackParticles.z0.d0.vz.definingParametersCovMatrix",
                                       "CombinedMuonTrackParticles.d0.z0.vz.definingParametersCovMatrix.truthOrigin.truthType",
                                       "ExtrapolatedMuonTrackParticles.d0.z0.vz.definingParametersCovMatrix.truthOrigin.truthType",
                                       "MuonTruthParticles.barcode.decayVtxLink.e.m.pdgId.prodVtxLink.px.py.pz.recoMuonLink.status.truthOrigin.truthType",
                                       "AntiKt4TruthJets.eta.m.phi.pt.TruthLabelDeltaR_B.TruthLabelDeltaR_C.TruthLabelDeltaR_T.TruthLabelID.ConeTruthLabelID.PartonTruthLabelID",
       "TauJets.IsTruthMatched.truthOrigin.truthType.truthParticleLink.truthJetLink"
       # Run2 tau reco - updated for R21 EDM
       + ".PanTau_isPanTauCandidate.ptPanTauCellBased.etaPanTauCellBased.phiPanTauCellBased.mPanTauCellBased"
       + ".PanTau_BDTValue_1p0n_vs_1p1n.PanTau_BDTValue_1p1n_vs_1pXn.PanTau_BDTValue_3p0n_vs_3pXn"
       # TauID variables - check R21 EDM
       + ".centFrac.etOverPtLeadTrk.innerTrkAvgDist.ipSigLeadTrk.SumPtTrkFrac.ptRatioEflowApprox.mEflowApprox.ChPiEMEOverCaloEME.EMPOverTrkSysP.dRmax.trFlightPathSig.massTrkSys"
       + ".centFracCorrected.etOverPtLeadTrkCorrected.innerTrkAvgDistCorrected.ipSigLeadTrkCorrected.SumPtTrkFracCorrected.ptRatioEflowApproxCorrected"
       + ".mEflowApproxCorrected.ChPiEMEOverCaloEMECorrected.EMPOverTrkSysPCorrected.dRmaxCorrected.trFlightPathSigCorrected.massTrkSysCorrected",
       # Neutral tau decay products - check R21 EDM
        "TauNeutralParticleFlowObjects.pt.eta.phi.m.e.rapidity.bdtPi0Score"]
SUSY11SlimmingHelper.IncludeMuonTriggerContent = False
SUSY11SlimmingHelper.IncludeEGammaTriggerContent = False
SUSY11SlimmingHelper.IncludeJetTriggerContent = True
SUSY11SlimmingHelper.IncludeTauTriggerContent = True
SUSY11SlimmingHelper.IncludeEtMissTriggerContent = False
SUSY11SlimmingHelper.IncludeBJetTriggerContent = True

# All standard truth particle collections are provided by DerivationFrameworkMCTruth (TruthDerivationTools.py)
# Most of the new containers are centrally added to SlimmingHelper via DerivationFrameworkCore ContainersOnTheFly.py
if DerivationFrameworkIsMonteCarlo:

  SUSY11SlimmingHelper.AppendToDictionary = {'TruthTop':'xAOD::TruthParticleContainer','TruthTopAux':'xAOD::TruthParticleAuxContainer',
                                             'TruthBSM':'xAOD::TruthParticleContainer','TruthBSMAux':'xAOD::TruthParticleAuxContainer',
                                             'TruthBoson':'xAOD::TruthParticleContainer','TruthBosonAux':'xAOD::TruthParticleAuxContainer'}
  
  SUSY11SlimmingHelper.AllVariables += ["TruthElectrons", "TruthMuons", "TruthTaus", "TruthPhotons", "TruthNeutrinos", "TruthTop", "TruthBSM", "TruthBoson"]

SUSY11Stream.RemoveItem("xAOD::TrigNavigation#*")
SUSY11Stream.RemoveItem("xAOD::TrigNavigationAuxInfo#*")

SUSY11SlimmingHelper.AppendContentToStream(SUSY11Stream)
