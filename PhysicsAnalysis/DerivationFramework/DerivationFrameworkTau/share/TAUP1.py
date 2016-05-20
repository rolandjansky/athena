# **************************************************************************************************************************
# TAUP1.py 
# reductionConf flag TAUP1 in Reco_tf.py   
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
streamName                                       = derivationFlags.WriteDAOD_TAUP1Stream.StreamName
fileName                                         = buildFileName( derivationFlags.WriteDAOD_TAUP1Stream )
TAUP1Stream                                      = MSMgr.NewPoolRootStream( streamName, fileName )
TAUP1Stream.AcceptAlgs(["TAUP1Kernel"])

# ==========================================================================================================================
# Thinning tool
# ==========================================================================================================================
from DerivationFrameworkCore.ThinningHelper import ThinningHelper
TAUP1ThinningHelper                              = ThinningHelper( "TAUP1ThinningHelper" )
TAUP1ThinningHelper.TriggerChains                = "HLT_e24.*|HLT_e60.*"
TAUP1ThinningHelper.AppendToStream( TAUP1Stream )

# MET/Jet tracks
thinning_expression                              = "(InDetTrackParticles.pt > 0.5*GeV) && (InDetTrackParticles.numberOfPixelHits > 0) && (InDetTrackParticles.numberOfSCTHits > 5) && (abs(DFCommonInDetTrackZ0AtPV) < 1.5)"
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__TrackParticleThinning
TAUP1MetTPThinningTool                           = DerivationFramework__TrackParticleThinning(
                                                     name                      = "TAUP1MetTPThinningTool",
                                                     ThinningService           = TAUP1ThinningHelper.ThinningSvc(),
                                                     SelectionString           = thinning_expression,
                                                     InDetTrackParticlesKey    = "InDetTrackParticles",
                                                     ApplyAnd                  = True)
ToolSvc                                         += TAUP1MetTPThinningTool

from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__JetTrackParticleThinning
TAUP1JetTPThinningTool                           = DerivationFramework__JetTrackParticleThinning(
                                                     name                      = "TAUP1JetTPThinningTool",
                                                     ThinningService           = TAUP1ThinningHelper.ThinningSvc(),
                                                     JetKey                    = "AntiKt4LCTopoJets",
                                                     InDetTrackParticlesKey    = "InDetTrackParticles",
                                                     ApplyAnd                  = True)
ToolSvc                                         += TAUP1JetTPThinningTool

from DerivationFrameworkCalo.DerivationFrameworkCaloConf import DerivationFramework__CaloClusterThinning
TAUP1CaloClusterThinning                         = DerivationFramework__CaloClusterThinning(
                                                     name                      = "TAUP1ClusterThinning",
                                                     ThinningService           = TAUP1ThinningHelper.ThinningSvc(),
                                                     SGKey                     = "TauJets",
                                                     TopoClCollectionSGKey     = "CaloCalTopoClusters")
ToolSvc                                         += TAUP1CaloClusterThinning

# Tracks associated with electrons
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__EgammaTrackParticleThinning
TAUP1ElectronTPThinningTool                      = DerivationFramework__EgammaTrackParticleThinning(
                                                     name                      = "TAUP1ElectronTPThinningTool",
                                                     ThinningService           = TAUP1ThinningHelper.ThinningSvc(),
                                                     SGKey                     = "Electrons",
                                                     InDetTrackParticlesKey    = "InDetTrackParticles")
ToolSvc                                         += TAUP1ElectronTPThinningTool

# Tracks associated with muons
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__MuonTrackParticleThinning
TAUP1MuonTPThinningTool                          = DerivationFramework__MuonTrackParticleThinning(
                                                     name                      = "TAUP1MuonTPThinningTool",
                                                     ThinningService           = TAUP1ThinningHelper.ThinningSvc(),
                                                     MuonKey                   = "Muons",
                                                     InDetTrackParticlesKey    = "InDetTrackParticles")
ToolSvc                                         += TAUP1MuonTPThinningTool

# Tracks associated with taus
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__TauTrackParticleThinning
TAUP1TauTPThinningTool                           = DerivationFramework__TauTrackParticleThinning(
                                                     name                      = "TAUP1TauTPThinningTool",
                                                     ThinningService           = TAUP1ThinningHelper.ThinningSvc(),
                                                     TauKey                    = "TauJets",
                                                     InDetTrackParticlesKey    = "InDetTrackParticles",
                                                     ConeSize                  = 0.6)
ToolSvc                                         += TAUP1TauTPThinningTool

# ==========================================================================================================================
# Skimming tool
# ==========================================================================================================================
elRequirement                                    = "( count( (Electrons.DFCommonElectronsIsEMLoose || Electrons.DFCommonElectronsLHLoose) && (Electrons.pt > 20.0*GeV) && (abs(Electrons.eta) < 2.6) ) >= 1 )"
muRequirement                                    = "( count( (Muons.pt > 10.0*GeV) && (abs(Muons.eta) < 2.0) && Muons.DFCommonGoodMuon ) < 1 )"
tauRequirement                                   = "( count( (TauJets.pt > 12.0*GeV) && (abs(TauJets.eta) < 2.6) && (abs(TauJets.charge) == 1.0) ) >= 1 )"

expression                                       = elRequirement + " && " + muRequirement + " && " + tauRequirement

from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__xAODStringSkimmingTool
TAUP1SkimmingTool                                = DerivationFramework__xAODStringSkimmingTool(
                                                     name                      = "TAUP1SkimmingTool",
                                                     expression                = expression)

ToolSvc                                         += TAUP1SkimmingTool

# ==========================================================================================================================
# Kernel algorithm
# ==========================================================================================================================
augmentationTools                                = []
# JRC: This is now done in TauCommon.py
# if DerivationFrameworkIsMonteCarlo:
#   augmentationTools                             += [DFCommonTauTruthMatchingWrapper]

from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__DerivationKernel
DerivationFrameworkJob                          += CfgMgr.DerivationFramework__DerivationKernel(
                                                     "TAUP1Kernel",
                                                     SkimmingTools             = [TAUP1SkimmingTool],
                                                     ThinningTools             = [TAUP1MetTPThinningTool,
                                                                                  TAUP1JetTPThinningTool,
                                                                                  TAUP1CaloClusterThinning,
                                                                                  TAUP1ElectronTPThinningTool,
                                                                                  TAUP1MuonTPThinningTool,
                                                                                  TAUP1TauTPThinningTool],
                                                     AugmentationTools         = augmentationTools)

# ==========================================================================================================================
# Add the containers to the output stream (slimming done here)
# ==========================================================================================================================
from DerivationFrameworkCore.SlimmingHelper import SlimmingHelper
from DerivationFrameworkTau.TAUPExtraContent import *

TAUP1SlimmingHelper                              = SlimmingHelper("TAUP1SlimmingHelper")
TAUP1SlimmingHelper.SmartCollections             = ["Electrons",
                                                    "Photons",
                                                    "Muons",
                                                    "TauJets",
                                                    "MET_Reference_AntiKt4EMTopo",
                                                    "MET_Reference_AntiKt4LCTopo",
                                                    "AntiKt4EMTopoJets",
                                                    "AntiKt4LCTopoJets",
                                                    "BTagging_AntiKt4EMTopo",
                                                    "BTagging_AntiKt4LCTopo",
                                                    "InDetTrackParticles",
                                                    "PrimaryVertices"]

if DerivationFrameworkIsMonteCarlo:
  TAUP1SlimmingHelper.StaticContent              = ["xAOD::TruthParticleContainer#TruthElectrons",
                                                    "xAOD::TruthParticleAuxContainer#TruthElectronsAux.",
                                                    "xAOD::TruthParticleContainer#TruthMuons",
                                                    "xAOD::TruthParticleAuxContainer#TruthMuonsAux.",
                                                    "xAOD::TruthParticleContainer#TruthTaus",
                                                    "xAOD::TruthParticleAuxContainer#TruthTausAux.",
                                                    "xAOD::TruthParticleContainer#TruthPhotons",
                                                    "xAOD::TruthParticleAuxContainer#TruthPhotonsAux.",
                                                    "xAOD::TruthParticleContainer#TruthNeutrinos",
                                                    "xAOD::TruthParticleAuxContainer#TruthNeutrinosAux."]

TAUP1SlimmingHelper.IncludeMuonTriggerContent    = True
TAUP1SlimmingHelper.IncludeTauTriggerContent     = True
TAUP1SlimmingHelper.IncludeEGammaTriggerContent  = True
TAUP1SlimmingHelper.IncludeEtMissTriggerContent  = True
TAUP1SlimmingHelper.IncludeJetTriggerContent     = True
TAUP1SlimmingHelper.IncludeBJetTriggerContent    = True

TAUP1SlimmingHelper.ExtraVariables               = ExtraContentTAUP1
TAUP1SlimmingHelper.AllVariables                 = ExtraContainersTAUP1

if globalflags.DataSource() == "geant4":
  TAUP1SlimmingHelper.ExtraVariables            += ExtraContentTruthTAUP1
  TAUP1SlimmingHelper.AllVariables              += ExtraContainersTruthTAUP1

TAUP1SlimmingHelper.AppendContentToStream(TAUP1Stream)

TAUP1Stream.AddItem("xAOD::EventShape#*")
TAUP1Stream.AddItem("xAOD::EventShapeAuxInfo#*")

# Add MET_RefFinalFix
# JRC TEMPORARILY COMMENTED
# addMETOutputs(TAUP1Stream)
