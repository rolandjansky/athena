# **************************************************************************************************************************
# TAUP2.py 
# reductionConf flag TAUP2 in Reco_tf.py   
# **************************************************************************************************************************
from DerivationFrameworkCore.DerivationFrameworkMaster import *
from DerivationFrameworkInDet.InDetCommon import *
from DerivationFrameworkJetEtMiss.JetCommon import *
from DerivationFrameworkJetEtMiss.ExtendedJetCommon import *
from DerivationFrameworkJetEtMiss.METCommon import *
from DerivationFrameworkEGamma.EGammaCommon import *
from DerivationFrameworkMuons.MuonsCommon import *
if DerivationFrameworkIsMonteCarlo:
  from DerivationFrameworkMCTruth.MCTruthCommon import *
  from DerivationFrameworkTau.TauTruthCommon import *
from DerivationFrameworkTau.TauCommon import *

# ==========================================================================================================================
# Set up stream
# ==========================================================================================================================
streamName                                       = derivationFlags.WriteDAOD_TAUP2Stream.StreamName
fileName                                         = buildFileName( derivationFlags.WriteDAOD_TAUP2Stream )
TAUP2Stream                                      = MSMgr.NewPoolRootStream( streamName, fileName )
TAUP2Stream.AcceptAlgs(["TAUP2Kernel"])

# ==========================================================================================================================
# Thinning tool
# ==========================================================================================================================
# from DerivationFrameworkCore.ThinningHelper import ThinningHelper
# TAUP2ThinningHelper                              = ThinningHelper( "TAUP2ThinningHelper" )
# TAUP2ThinningHelper.TriggerChains                = "HLT_e24.*|HLT_e60.*"
# TAUP2ThinningHelper.AppendToStream( TAUP2Stream )
# 
# # MET/Jet tracks
# thinning_expression                              = "(InDetTrackParticles.pt > 0.5*GeV) && (InDetTrackParticles.numberOfPixelHits > 0) && (InDetTrackParticles.numberOfSCTHits > 5) && (abs(DFCommonInDetTrackZ0AtPV) < 1.5)"
# from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__TrackParticleThinning
# TAUP2MetTPThinningTool                           = DerivationFramework__TrackParticleThinning(
#                                                      name                      = "TAUP2MetTPThinningTool",
#                                                      ThinningService           = TAUP2ThinningHelper.ThinningSvc(),
#                                                      SelectionString           = thinning_expression,
#                                                      InDetTrackParticlesKey    = "InDetTrackParticles",
#                                                      ApplyAnd                  = True)
# ToolSvc                                         += TAUP2MetTPThinningTool
# 
# from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__JetTrackParticleThinning
# TAUP2JetTPThinningTool                           = DerivationFramework__JetTrackParticleThinning(
#                                                      name                      = "TAUP2JetTPThinningTool",
#                                                      ThinningService           = TAUP2ThinningHelper.ThinningSvc(),
#                                                      JetKey                    = "AntiKt4LCTopoJets",
#                                                      InDetTrackParticlesKey    = "InDetTrackParticles",
#                                                      ApplyAnd                  = True)
# ToolSvc                                         += TAUP2JetTPThinningTool
# 
# # Tracks associated with electrons
# from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__EgammaTrackParticleThinning
# TAUP2ElectronTPThinningTool                      = DerivationFramework__EgammaTrackParticleThinning(
#                                                      name                      = "TAUP2ElectronTPThinningTool",
#                                                      ThinningService           = TAUP2ThinningHelper.ThinningSvc(),
#                                                      SGKey                     = "Electrons",
#                                                      InDetTrackParticlesKey    = "InDetTrackParticles")
# ToolSvc                                         += TAUP2ElectronTPThinningTool
# 
# # Tracks associated with muons
# from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__MuonTrackParticleThinning
# TAUP2MuonTPThinningTool                          = DerivationFramework__MuonTrackParticleThinning(
#                                                      name                      = "TAUP2MuonTPThinningTool",
#                                                      ThinningService           = TAUP2ThinningHelper.ThinningSvc(),
#                                                      MuonKey                   = "Muons",
#                                                      InDetTrackParticlesKey    = "InDetTrackParticles")
# ToolSvc                                         += TAUP2MuonTPThinningTool
# 
# # Tracks associated with taus
# from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__TauTrackParticleThinning
# TAUP2TauTPThinningTool                           = DerivationFramework__TauTrackParticleThinning(
#                                                      name                      = "TAUP2TauTPThinningTool",
#                                                      ThinningService           = TAUP2ThinningHelper.ThinningSvc(),
#                                                      TauKey                    = "TauJets",
#                                                      InDetTrackParticlesKey    = "InDetTrackParticles",
#                                                      ConeSize                  = 0.6)
# ToolSvc                                         += TAUP2TauTPThinningTool

# ==========================================================================================================================
# Skimming tool
# ==========================================================================================================================
# elRequirement                                    = "( count( (Electrons.DFCommonElectronsIsEMLoose || Electrons.DFCommonElectronsLHLoose) && (Electrons.pt > 20.0*GeV) && (abs(Electrons.eta) < 2.6) ) >= 1 )"
# muRequirement                                    = "( count( (Muons.pt > 10.0*GeV) && (abs(Muons.eta) < 2.0) && Muons.DFCommonGoodMuon ) < 1 )"
# tauRequirement                                   = "( count( (TauJets.pt > 12.0*GeV) && (abs(TauJets.eta) < 2.6) && (abs(TauJets.charge) == 1.0) ) >= 1 )"
# 
# expression                                       = elRequirement + " && " + muRequirement + " && " + tauRequirement
# 
# from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__xAODStringSkimmingTool
# TAUP2SkimmingTool                                = DerivationFramework__xAODStringSkimmingTool(
#                                                      name                      = "TAUP2SkimmingTool",
#                                                      expression                = expression)
# 
# ToolSvc                                         += TAUP2SkimmingTool

# ==========================================================================================================================
# Kernel algorithm
# ==========================================================================================================================
augmentationTools                                = []
# JRC: This is now done in TauCommon.py
# if DerivationFrameworkIsMonteCarlo:
#   augmentationTools                             += [DFCommonTauTruthMatchingWrapper]

from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__DerivationKernel
DerivationFrameworkJob                          += CfgMgr.DerivationFramework__DerivationKernel(
                                                     "TAUP2Kernel",
#                                                     SkimmingTools             = [TAUP2SkimmingTool],
#                                                     ThinningTools             = [TAUP2MetTPThinningTool,
#                                                                                  TAUP2JetTPThinningTool,
#                                                                                  TAUP2ElectronTPThinningTool,
#                                                                                  TAUP2MuonTPThinningTool,
#                                                                                  TAUP2TauTPThinningTool],
                                                     AugmentationTools         = augmentationTools)

# ==========================================================================================================================
# Add the containers to the output stream (slimming done here)
# ==========================================================================================================================
from DerivationFrameworkCore.SlimmingHelper import SlimmingHelper
from DerivationFrameworkTau.TAUPExtraContent import *

TAUP2SlimmingHelper                              = SlimmingHelper("TAUP2SlimmingHelper")
TAUP2SlimmingHelper.SmartCollections             = [
#                                                    "Electrons",
#                                                    "Photons",
#                                                    "Muons",
                                                    "TauJets"]
#                                                    "MET_Reference_AntiKt4EMTopo",
#                                                    "MET_Reference_AntiKt4LCTopo",
#                                                    "AntiKt4EMTopoJets",
#                                                    "AntiKt4LCTopoJets",
#                                                    "BTagging_AntiKt4EMTopo",
#                                                    "BTagging_AntiKt4LCTopo",
#                                                    "InDetTrackParticles",
#                                                    "PrimaryVertices"]

if DerivationFrameworkIsMonteCarlo:
  TAUP2SlimmingHelper.StaticContent              = ["xAOD::TruthParticleContainer#TruthElectrons",
                                                    "xAOD::TruthParticleAuxContainer#TruthElectronsAux.",
                                                    "xAOD::TruthParticleContainer#TruthMuons",
                                                    "xAOD::TruthParticleAuxContainer#TruthMuonsAux.",
                                                    "xAOD::TruthParticleContainer#TruthTaus",
                                                    "xAOD::TruthParticleAuxContainer#TruthTausAux."]
#                                                    "xAOD::TruthParticleContainer#TruthPhotons",
#                                                    "xAOD::TruthParticleAuxContainer#TruthPhotonsAux.",
#                                                    "xAOD::TruthParticleContainer#TruthNeutrinos",
#                                                    "xAOD::TruthParticleAuxContainer#TruthNeutrinosAux."]

# TAUP2SlimmingHelper.IncludeMuonTriggerContent    = True
# TAUP2SlimmingHelper.IncludeTauTriggerContent     = True
# TAUP2SlimmingHelper.IncludeEGammaTriggerContent  = True
# TAUP2SlimmingHelper.IncludeEtMissTriggerContent  = True
# TAUP2SlimmingHelper.IncludeJetTriggerContent     = True
# TAUP2SlimmingHelper.IncludeBJetTriggerContent    = True

TAUP2SlimmingHelper.ExtraVariables               = ExtraContentTAUP2
TAUP2SlimmingHelper.AllVariables                 = ExtraContainersTAUP2

if globalflags.DataSource() == "geant4":
  TAUP2SlimmingHelper.ExtraVariables            += ExtraContentTruthTAUP2
  TAUP2SlimmingHelper.AllVariables              += ExtraContainersTruthTAUP2

TAUP2SlimmingHelper.AppendContentToStream(TAUP2Stream)

TAUP2Stream.AddItem("xAOD::EventShape#*")
TAUP2Stream.AddItem("xAOD::EventShapeAuxInfo#*")

# Add MET_RefFinalFix
# JRC TEMPORARILY COMMENTED
# addMETOutputs(TAUP2Stream)
