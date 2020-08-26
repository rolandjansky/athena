# **************************************************************************************************************************
# TAUP1.py 
# reductionConf flag TAUP1 in Reco_tf.py   
# **************************************************************************************************************************
from DerivationFrameworkCore.DerivationFrameworkMaster import *
from DerivationFrameworkInDet.InDetCommon import *
from DerivationFrameworkJetEtMiss.JetCommon import *
from DerivationFrameworkJetEtMiss.ExtendedJetCommon import *
from DerivationFrameworkJetEtMiss.METCommon import *
#from DerivationFrameworkEGamma.EGammaCommon import *
from DerivationFrameworkMuons.MuonsCommon import *

# ==========================================================================================================================
# Set up stream
# ==========================================================================================================================
streamName  = derivationFlags.WriteDAOD_TAUP1Stream.StreamName
fileName    = buildFileName( derivationFlags.WriteDAOD_TAUP1Stream )
TAUP1Stream = MSMgr.NewPoolRootStream( streamName, fileName )
TAUP1Stream.AcceptAlgs(["TAUP1Kernel"])

SeqTAUP1 = CfgMgr.AthSequencer("SeqTAUP1")

# ==========================================================================================================================
# MC truth
# ==========================================================================================================================
if DerivationFrameworkIsMonteCarlo:
  from DerivationFrameworkMCTruth.MCTruthCommon import addStandardTruthContents
  addStandardTruthContents(SeqTAUP1,prefix='TAUP1_')

# ==========================================================================================================================
# Thinning tool
# ==========================================================================================================================
from DerivationFrameworkCore.ThinningHelper import ThinningHelper
TAUP1ThinningHelper               = ThinningHelper( "TAUP1ThinningHelper" )
TAUP1ThinningHelper.TriggerChains = "HLT_e24.*|HLT_e60.*"
TAUP1ThinningHelper.AppendToStream( TAUP1Stream )

from DerivationFrameworkCalo.DerivationFrameworkCaloConf import DerivationFramework__CaloClusterThinning
TAUP1CaloClusterThinning = DerivationFramework__CaloClusterThinning(name                  = "TAUP1ClusterThinning",
                                                                    StreamName            = streamName,
                                                                    SGKey                 = "TauJets",
                                                                    TopoClCollectionSGKey = "CaloCalTopoClusters")
ToolSvc += TAUP1CaloClusterThinning

# Tracks associated with electrons
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__EgammaTrackParticleThinning
TAUP1ElectronTPThinningTool = DerivationFramework__EgammaTrackParticleThinning(name                   = "TAUP1ElectronTPThinningTool",
                                                                               StreamName             = streamName,
                                                                               SGKey                  = "Electrons",
                                                                               InDetTrackParticlesKey = "InDetTrackParticles")
ToolSvc += TAUP1ElectronTPThinningTool

# Tracks associated with muons
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__MuonTrackParticleThinning
TAUP1MuonTPThinningTool = DerivationFramework__MuonTrackParticleThinning(name                   = "TAUP1MuonTPThinningTool",
                                                                         StreamName             = streamName,
                                                                         MuonKey                = "Muons",
                                                                         InDetTrackParticlesKey = "InDetTrackParticles")
ToolSvc += TAUP1MuonTPThinningTool

# Tracks associated with taus
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__TauTrackParticleThinning
TAUP1TauTPThinningTool = DerivationFramework__TauTrackParticleThinning(name                   = "TAUP1TauTPThinningTool",
                                                                       StreamName             = streamName,
                                                                       TauKey                 = "TauJets",
                                                                       InDetTrackParticlesKey = "InDetTrackParticles",
                                                                       ConeSize               = 0.6)
ToolSvc += TAUP1TauTPThinningTool

# ==========================================================================================================================
# Skimming tool
# ==========================================================================================================================
#elRequirement  = "( count( (Electrons.DFCommonElectronsIsEMLoose || Electrons.DFCommonElectronsLHLoose) && (Electrons.pt > 20.0*GeV) && (abs(Electrons.eta) < 2.6) ) >= 1 )"
muRequirement  = "( count( (Muons.pt > 10.0*GeV) && (abs(Muons.eta) < 2.0) && Muons.DFCommonGoodMuon ) < 1 )"
tauRequirement = "( count( (TauJets.pt > 12.0*GeV) && (abs(TauJets.eta) < 2.6) && (abs(TauJets.charge) == 1.0) ) >= 1 )"

#expression     = elRequirement + " && " + muRequirement + " && " + tauRequirement
expression     = muRequirement + " && " + tauRequirement

from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__xAODStringSkimmingTool
TAUP1SkimmingTool = DerivationFramework__xAODStringSkimmingTool(name       = "TAUP1SkimmingTool",
                                                                expression = expression)

ToolSvc += TAUP1SkimmingTool

# ==========================================================================================================================
# Kernel algorithm
# ==========================================================================================================================
SeqTAUP1 += CfgMgr.DerivationFramework__DerivationKernel("TAUP1Kernel",
                                                         SkimmingTools = [TAUP1SkimmingTool],
                                                         ThinningTools = [TAUP1CaloClusterThinning,
                                                                          TAUP1ElectronTPThinningTool,
                                                                          TAUP1MuonTPThinningTool,
                                                                          TAUP1TauTPThinningTool])

DerivationFrameworkJob += SeqTAUP1

# ==========================================================================================================================
# Add the containers to the output stream (slimming done here)
# ==========================================================================================================================
from DerivationFrameworkCore.SlimmingHelper import SlimmingHelper
from DerivationFrameworkTau.TAUPExtraContent import *

TAUP1SlimmingHelper                  = SlimmingHelper("TAUP1SlimmingHelper")
TAUP1SlimmingHelper.SmartCollections = ["Electrons",
                                        "Photons",
                                        "Muons",
                                        "TauJets",
                                        "MET_Reference_AntiKt4EMTopo",
                                        "MET_Reference_AntiKt4LCTopo",
                                        "AntiKt4EMTopoJets",
                                        "AntiKt4LCTopoJets",
                                        #"BTagging_AntiKt4EMTopo",
                                        #"BTagging_AntiKt4LCTopo",
                                        "InDetTrackParticles",
                                        "PrimaryVertices"]

TAUP1SlimmingHelper.IncludeMuonTriggerContent   = False
TAUP1SlimmingHelper.IncludeTauTriggerContent    = True
TAUP1SlimmingHelper.IncludeEGammaTriggerContent = True
TAUP1SlimmingHelper.IncludeEtMissTriggerContent = False
TAUP1SlimmingHelper.IncludeJetTriggerContent    = False
TAUP1SlimmingHelper.IncludeBJetTriggerContent   = False

TAUP1SlimmingHelper.ExtraVariables = ExtraContentTAUP1
TAUP1SlimmingHelper.AllVariables   = ExtraContainersTAUP1

if DerivationFrameworkIsMonteCarlo:
  TAUP1SlimmingHelper.ExtraVariables += ExtraContentTruthTAUP1
  TAUP1SlimmingHelper.AllVariables   += ExtraContainersTruthTAUP1
  TAUP1SlimmingHelper.AllVariables   += ["TruthElectrons", 
                                         "TruthMuons", 
                                         "TruthTaus", 
                                         "TruthPhotons",
                                         "TruthNeutrinos"]

TAUP1SlimmingHelper.AppendContentToStream(TAUP1Stream)

TAUP1Stream.AddItem("xAOD::EventShape#*")
TAUP1Stream.AddItem("xAOD::EventShapeAuxInfo#*")
