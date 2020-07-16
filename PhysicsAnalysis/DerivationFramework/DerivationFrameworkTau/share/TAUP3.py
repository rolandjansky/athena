# **************************************************************************************************************************
# TAUP3.py 
# reductionConf flag TAUP3 in Reco_tf.py   
# **************************************************************************************************************************
from DerivationFrameworkCore.DerivationFrameworkMaster import *
from DerivationFrameworkInDet.InDetCommon import *
from DerivationFrameworkJetEtMiss.JetCommon import *
from DerivationFrameworkJetEtMiss.ExtendedJetCommon import *
from DerivationFrameworkJetEtMiss.METCommon import *
from DerivationFrameworkMuons.MuonsCommon import *

# ==========================================================================================================================
# Set up stream
# ==========================================================================================================================
streamName  = derivationFlags.WriteDAOD_TAUP3Stream.StreamName
fileName    = buildFileName( derivationFlags.WriteDAOD_TAUP3Stream )
TAUP3Stream = MSMgr.NewPoolRootStream( streamName, fileName )
TAUP3Stream.AcceptAlgs(["TAUP3Kernel"])

augStream = MSMgr.GetStream( streamName )
evtStream = augStream.GetEventStream()

SeqTAUP3 = CfgMgr.AthSequencer("SeqTAUP3")

# ==========================================================================================================================
# MC truth
# ==========================================================================================================================
if DerivationFrameworkIsMonteCarlo:
  from DerivationFrameworkMCTruth.MCTruthCommon import addStandardTruthContents
  addStandardTruthContents(SeqTAUP3,prefix='TAUP3_')

# ==========================================================================================================================
# Thinning tool
# ==========================================================================================================================
from DerivationFrameworkCalo.DerivationFrameworkCaloConf import DerivationFramework__CaloClusterThinning
TAUP3CaloClusterThinning = DerivationFramework__CaloClusterThinning(name                  = "TAUP3ClusterThinning",
                                                                    StreamName            = streamName,       
                                                                    SGKey                 = "TauJets",
                                                                    TopoClCollectionSGKey = "CaloCalTopoClusters")
ToolSvc += TAUP3CaloClusterThinning

# Tracks associated with electrons
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__EgammaTrackParticleThinning
TAUP3ElectronTPThinningTool = DerivationFramework__EgammaTrackParticleThinning(name                   = "TAUP3ElectronTPThinningTool",
                                                                               StreamName             = streamName,
                                                                               SGKey                  = "Electrons",
                                                                               InDetTrackParticlesKey = "InDetTrackParticles")
ToolSvc += TAUP3ElectronTPThinningTool

# Tracks associated with muons
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__MuonTrackParticleThinning
TAUP3MuonTPThinningTool = DerivationFramework__MuonTrackParticleThinning(name                   = "TAUP3MuonTPThinningTool",
                                                                         StreamName             = streamName,
                                                                         MuonKey                = "Muons",
                                                                         InDetTrackParticlesKey = "InDetTrackParticles")
ToolSvc += TAUP3MuonTPThinningTool

# Tracks associated with taus
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__TauTrackParticleThinning
TAUP3TauTPThinningTool = DerivationFramework__TauTrackParticleThinning(name                   = "TAUP3TauTPThinningTool",
                                                                       StreamName             = streamName,
                                                                       TauKey                 = "TauJets",
                                                                       InDetTrackParticlesKey = "InDetTrackParticles",
                                                                       ConeSize               = 0.6)
ToolSvc += TAUP3TauTPThinningTool

# ==========================================================================================================================
# Skimming tool
# ==========================================================================================================================
muRequirement  = "( count( (Muons.pt > 22.0*GeV) && Muons.DFCommonGoodMuon ) >= 1 )"
tauRequirement = "( count( (TauJets.pt > 15.0*GeV) && (abs(TauJets.charge)==1.0) && ((TauJets.nTracks == 1) || (TauJets.nTracks == 3)) ) >= 1 )"

expression = muRequirement + " && " + tauRequirement

from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__xAODStringSkimmingTool
TAUP3SkimmingTool = DerivationFramework__xAODStringSkimmingTool(name       = "TAUP3SkimmingTool",
                                                                expression = expression)

ToolSvc += TAUP3SkimmingTool

# ==========================================================================================================================
# Kernel algorithm
# ==========================================================================================================================
from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__DerivationKernel
SeqTAUP3 += CfgMgr.DerivationFramework__DerivationKernel("TAUP3Kernel",
                                                         SkimmingTools = [TAUP3SkimmingTool],
                                                         ThinningTools = [TAUP3CaloClusterThinning,
                                                                          TAUP3ElectronTPThinningTool,
                                                                          TAUP3MuonTPThinningTool,
                                                                          TAUP3TauTPThinningTool])

DerivationFrameworkJob += SeqTAUP3

# ==========================================================================================================================
# Add the containers to the output stream (slimming done here)
# ==========================================================================================================================
from DerivationFrameworkCore.SlimmingHelper import SlimmingHelper
from DerivationFrameworkTau.TAUPExtraContent import *

TAUP3SlimmingHelper = SlimmingHelper("TAUP3SlimmingHelper")
TAUP3SlimmingHelper.SmartCollections = ["Electrons",
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

TAUP3SlimmingHelper.IncludeMuonTriggerContent    = True
TAUP3SlimmingHelper.IncludeEGammaTriggerContent  = True
TAUP3SlimmingHelper.IncludeEtMissTriggerContent  = True
TAUP3SlimmingHelper.IncludeJetTriggerContent     = True
TAUP3SlimmingHelper.IncludeBJetTriggerContent    = True

TAUP3SlimmingHelper.ExtraVariables = ExtraContentTAUP3
TAUP3SlimmingHelper.AllVariables   = ExtraContainersTAUP3

if DerivationFrameworkIsMonteCarlo:
  TAUP3SlimmingHelper.ExtraVariables += ExtraContentTruthTAUP3
  TAUP3SlimmingHelper.AllVariables   += ExtraContainersTruthTAUP3
  TAUP3SlimmingHelper.AllVariables   += ["TruthElectrons", 
                                         "TruthMuons", 
                                         "TruthTaus", 
                                         "TruthPhotons",
                                         "TruthNeutrinos"]

TAUP3SlimmingHelper.AppendContentToStream(TAUP3Stream)

TAUP3Stream.AddItem("xAOD::EventShape#*")
TAUP3Stream.AddItem("xAOD::EventShapeAuxInfo#*")
