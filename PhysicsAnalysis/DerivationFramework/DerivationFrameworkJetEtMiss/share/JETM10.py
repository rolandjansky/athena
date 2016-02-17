#====================================================================
# JETM10.py 
# reductionConf flag JETM10 in Reco_tf.py   
#====================================================================
from DerivationFrameworkCore.DerivationFrameworkMaster import *
from DerivationFrameworkJetEtMiss.JetCommon import *
from DerivationFrameworkJetEtMiss.ExtendedJetCommon import *
from DerivationFrameworkJetEtMiss.METCommon import *

#======================================================================================================================
# SKIMMING TOOL
#======================================================================================================================
# pre scale
from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__PrescaleTool
JETM10PrescaleSkimmingTool = DerivationFramework__PrescaleTool( name = "JETM10PrescaleSkimmingTool",
                                                                Prescale = 5)
ToolSvc += JETM10PrescaleSkimmingTool


#======================================================================================================================
# AUGMENTATION  TOOL
#======================================================================================================================
# from DerivationFrameworkJetEtMiss.DerivationFrameworkJetEtMissConf import DerivationFramework__METTriggerAugmentationTool
# JETM10LVL1AugmentationTool = DerivationFramework__METTriggerAugmentationTool(name = "JETM10LVL1AugmentationTool",
#                                                                              SGPrefix = "JETM10")
# ToolSvc += JETM10LVL1AugmentationTool

#======================================================================================================================
# SET UP STREAM
#======================================================================================================================
streamName = derivationFlags.WriteDAOD_JETM10Stream.StreamName
fileName = buildFileName( derivationFlags.WriteDAOD_JETM10Stream )
JETM10Stream = MSMgr.NewPoolRootStream( streamName, fileName )
JETM10Stream.AcceptAlgs(['JETM10Kernel'])

#=======================================
# ESTABLISH THE THINNING HELPER
#=======================================
from DerivationFrameworkCore.ThinningHelper import ThinningHelper
JETM10ThinningHelper = ThinningHelper( "JETM10ThinningHelper" )
JETM10ThinningHelper.TriggerChains = ('L1_XE.*|L1_XS.*|L1_TE.*|HLT_xe.*|HLT_xs.*|HLT_te.*|'
                                      'HLT_e26_lhvloose_L1EM20VH.*|HLT_e\\d\\d_(lhvloose|vloose)|'
                                      'HLT_mu20_iloose.*|HLT_mu50|HLT_e24_lhmedium_iloose_L1_EM20VH|'
                                      'HLT_e60_lhmedium|L1_J.*XE.*|HLT_j.*xe.*')
JETM10ThinningHelper.AppendToStream( JETM10Stream )

#======================================================================================================================
# THINNING TOOLS
#======================================================================================================================
thinningTools = []

# TrackParticles directly
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__TrackParticleThinning
JETM10TPThinningTool = DerivationFramework__TrackParticleThinning(name = "JETM10TPThinningTool",
                                                                       ThinningService         = JETM10ThinningHelper.ThinningSvc(),
                                                                       SelectionString         = "InDetTrackParticles.pt > 10*GeV",
                                                                       InDetTrackParticlesKey  = "InDetTrackParticles") 
ToolSvc += JETM10TPThinningTool
thinningTools.append(JETM10TPThinningTool)
  
# TrackParticles associated with Muons
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__MuonTrackParticleThinning
JETM10MuonTPThinningTool = DerivationFramework__MuonTrackParticleThinning(name                    = "JETM10MuonTPThinningTool",
                                                                               ThinningService         = JETM10ThinningHelper.ThinningSvc(),
                                                                               MuonKey                 = "Muons",
                                                                               InDetTrackParticlesKey  = "InDetTrackParticles")
ToolSvc += JETM10MuonTPThinningTool
thinningTools.append(JETM10MuonTPThinningTool)

# TrackParticles associated with electrons
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__EgammaTrackParticleThinning
JETM10ElectronTPThinningTool = DerivationFramework__EgammaTrackParticleThinning(name                     = "JETM10ElectronTPThinningTool",
                                                                                     ThinningService          = JETM10ThinningHelper.ThinningSvc(),
                                                                                     SGKey                    = "Electrons",
                                                                                     InDetTrackParticlesKey   = "InDetTrackParticles")
ToolSvc += JETM10ElectronTPThinningTool
thinningTools.append(JETM10ElectronTPThinningTool)

# TrackParticles associated with photons
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__EgammaTrackParticleThinning
JETM10PhotonTPThinningTool = DerivationFramework__EgammaTrackParticleThinning(name                        = "JETM10PhotonTPThinningTool",
                                                                                   ThinningService             = JETM10ThinningHelper.ThinningSvc(),
                                                                                   SGKey                       = "Photons",
                                                                                   InDetTrackParticlesKey      = "InDetTrackParticles")
ToolSvc += JETM10PhotonTPThinningTool
thinningTools.append(JETM10PhotonTPThinningTool)

# TrackParticles associated with taus
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__TauTrackParticleThinning
JETM10TauTPThinningTool = DerivationFramework__TauTrackParticleThinning( name                    = "JETM10TauTPThinningTool",
                                                                              ThinningService         = JETM10ThinningHelper.ThinningSvc(),
                                                                              TauKey                  = "TauJets",
                                                                              InDetTrackParticlesKey  = "InDetTrackParticles")
ToolSvc += JETM10TauTPThinningTool
thinningTools.append(JETM10TauTPThinningTool)

#======================================================================================================================
# CREATE PRIVATE SEQUENCE
#======================================================================================================================
jetm10Seq = CfgMgr.AthSequencer("jetm10Seq")
DerivationFrameworkJob += jetm10Seq

#=======================================
# CREATE THE DERIVATION KERNEL ALGORITHM   
#=======================================
from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__DerivationKernel
jetm10Seq += CfgMgr.DerivationFramework__DerivationKernel('JETM10Kernel',
                                                          SkimmingTools = [JETM10PrescaleSkimmingTool],
#                                                          AugmentationTools = [JETM10LVL1AugmentationTool],
                                                          ThinningTools = thinningTools)

#======================================================================================================================
# Content list for slimming
#======================================================================================================================
from DerivationFrameworkCore.SlimmingHelper import SlimmingHelper
JETM10SlimmingHelper = SlimmingHelper('JETM10SlimmingHelper')
JETM10SlimmingHelper.SmartCollections = ["Electrons", "Muons", "Photons", "TauJets",
                                         "AntiKt4EMTopoJets", "PrimaryVertices", "BTagging_AntiKt4EMTopo"]
JETM10SlimmingHelper.AllVariables =     ["HLT_xAOD__TrigMissingETContainer_EFJetEtSum",
                                         "HLT_xAOD__TrigMissingETContainer_TrigMissingET_FEB",
                                         "HLT_xAOD__TrigMissingETContainer_EFMissingET_Fex_2sidednoiseSupp_PUC",
                                         "HLT_xAOD__TrigMissingETContainer_T2MissingET",
                                         "HLT_xAOD__TrigMissingETContainer_TrigEFMissingET_topocl_PS",
                                         "HLT_xAOD__TrigMissingETContainer_TrigEFMissingET_mht",
                                         "HLT_xAOD__TrigMissingETContainer_TrigEFMissingET_topocl_PUC",
                                         "HLT_xAOD__TrigMissingETContainer_TrigEFMissingET_topocl",
                                         "HLT_xAOD__TrigMissingETContainer_TrigL2MissingET_FEB",
                                         "HLT_xAOD__TrigMissingETContainer_TrigEFMissingET",
                                         "LVL1JetRoIs",
                                         "LVL1JetEtRoI",
                                         "MET_Core_AntiKt4EMTopo",
                                         "METAssoc_AntiKt4EMTopo"]
JETM10SlimmingHelper.IncludeJetTriggerContent = True
JETM10SlimmingHelper.IncludeEtMissTriggerContent = True
JETM10SlimmingHelper.AppendContentToStream(JETM10Stream)

#======================================================================================================================
# Content Definition
#======================================================================================================================
#JETM10Stream.AddItem("std::vector<float>#JETM10KFEx");
#JETM10Stream.AddItem("std::vector<float>#JETM10KFEy");
