#====================================================================
# JETM11.py 
# reductionConf flag JETM11 in Reco_tf.py   
#====================================================================
from DerivationFrameworkCore.DerivationFrameworkMaster import *
from DerivationFrameworkJetEtMiss.JetCommon import *
from DerivationFrameworkJetEtMiss.ExtendedJetCommon import *
from DerivationFrameworkJetEtMiss.METCommon import *
from DerivationFrameworkEGamma.EGammaCommon import*
from DerivationFrameworkMuons.MuonsCommon import*

#======================================================================================================================
# SKIMMING TOOL
#======================================================================================================================
cutExpression = ("(count(Electrons.DFCommonElectronsLHLoose && Electrons.pt > (10*GeV) && abs(Electrons.eta) < 2.47) + " +
                 "count(Muons.DFCommonMuonsPreselection && Muons.pt > (10*GeV) && abs(Muons.eta) < 2.47)) >=1")
from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__xAODStringSkimmingTool
JETM11StringSkimmingTool = DerivationFramework__xAODStringSkimmingTool(name = "JETM11StringSkimmingTool",
                                                                       expression = cutExpression)
ToolSvc += JETM11StringSkimmingTool
print JETM11StringSkimmingTool

#======================================================================================================================
# AUGMENTATION TOOL
#======================================================================================================================
from DerivationFrameworkJetEtMiss.DerivationFrameworkJetEtMissConf import DerivationFramework__METTriggerAugmentationTool
JETM11KFData15AugmentationTool = DerivationFramework__METTriggerAugmentationTool(name = "JETM11KFData15AugmentationTool", #NB: data15 refers to the dataset used to form the look up table, not the intended target
                                                                                 OutputName = "LVL1EnergySumRoI_KFMETData15",
                                                                                 LUTFile = "LUT_data15.root")
ToolSvc += JETM11KFData15AugmentationTool
JETM11KFmc12AugmentationTool = DerivationFramework__METTriggerAugmentationTool(name = "JETM11KFmc12AugmentationTool", #NB: mc12 refers to the dataset used to form the look up table, not the intended target
                                                                                 OutputName = "LVL1EnergySumRoI_KFMETmc12",
                                                                                 LUTFile = "LUT_mc12.root")
ToolSvc += JETM11KFmc12AugmentationTool

#======================================================================================================================
# SET UP STREAM
#======================================================================================================================
streamName = derivationFlags.WriteDAOD_JETM11Stream.StreamName
fileName = buildFileName( derivationFlags.WriteDAOD_JETM11Stream )
JETM11Stream = MSMgr.NewPoolRootStream( streamName, fileName )
JETM11Stream.AcceptAlgs(['JETM11Kernel'])


#=======================================
# ESTABLISH THE THINNING HELPER
#=======================================
from DerivationFrameworkCore.ThinningHelper import ThinningHelper
JETM11ThinningHelper = ThinningHelper( "JETM11ThinningHelper" )
# JETM11ThinningHelper.TriggerChains = ('L1_XE.*|L1_XS.*|L1_TE.*|HLT_xe.*|HLT_xs.*|HLT_te.*|'
#                                       'HLT_e26_lhvloose_L1EM20VH.*|HLT_e\\d\\d_(lhvloose|vloose)|'
#                                       'HLT_mu20_iloose.*|HLT_mu50|HLT_e24_lhmedium_iloose_L1_EM20VH|'
#                                       'HLT_e60_lhmedium|L1_J.*XE.*|HLT_j.*xe.*')
JETM11ThinningHelper.AppendToStream( JETM11Stream )

#======================================================================================================================
# THINNING TOOLS
#======================================================================================================================
thinningTools = []

# TrackParticles directly
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__TrackParticleThinning
JETM11TPThinningTool = DerivationFramework__TrackParticleThinning(name = "JETM11TPThinningTool",
                                                                  ThinningService         = JETM11ThinningHelper.ThinningSvc(),
                                                                  SelectionString         = "InDetTrackParticles.pt > 10*GeV",
                                                                  InDetTrackParticlesKey  = "InDetTrackParticles") 
ToolSvc += JETM11TPThinningTool
thinningTools.append(JETM11TPThinningTool)
  
# TrackParticles associated with Muons
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__MuonTrackParticleThinning
JETM11MuonTPThinningTool = DerivationFramework__MuonTrackParticleThinning(name                    = "JETM11MuonTPThinningTool",
                                                                          ThinningService         = JETM11ThinningHelper.ThinningSvc(),
                                                                          MuonKey                 = "Muons",
                                                                          InDetTrackParticlesKey  = "InDetTrackParticles")
ToolSvc += JETM11MuonTPThinningTool
thinningTools.append(JETM11MuonTPThinningTool)

# TrackParticles associated with electrons
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__EgammaTrackParticleThinning
JETM11ElectronTPThinningTool = DerivationFramework__EgammaTrackParticleThinning(name                     = "JETM11ElectronTPThinningTool",
                                                                                ThinningService          = JETM11ThinningHelper.ThinningSvc(),
                                                                                SGKey                    = "Electrons",
                                                                                InDetTrackParticlesKey   = "InDetTrackParticles")
ToolSvc += JETM11ElectronTPThinningTool
thinningTools.append(JETM11ElectronTPThinningTool)

# TrackParticles associated with photons
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__EgammaTrackParticleThinning
JETM11PhotonTPThinningTool = DerivationFramework__EgammaTrackParticleThinning(name                        = "JETM11PhotonTPThinningTool",
                                                                              ThinningService             = JETM11ThinningHelper.ThinningSvc(),
                                                                              SGKey                       = "Photons",
                                                                              InDetTrackParticlesKey      = "InDetTrackParticles")
ToolSvc += JETM11PhotonTPThinningTool
thinningTools.append(JETM11PhotonTPThinningTool)

# TrackParticles associated with taus
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__TauTrackParticleThinning
JETM11TauTPThinningTool = DerivationFramework__TauTrackParticleThinning( name                    = "JETM11TauTPThinningTool",
                                                                         ThinningService         = JETM11ThinningHelper.ThinningSvc(),
                                                                         TauKey                  = "TauJets",
                                                                         InDetTrackParticlesKey  = "InDetTrackParticles")
ToolSvc += JETM11TauTPThinningTool
thinningTools.append(JETM11TauTPThinningTool)

#======================================================================================================================
# CREATE PRIVATE SEQUENCE
#======================================================================================================================
jetm11Seq = CfgMgr.AthSequencer("jetm11Seq")
DerivationFrameworkJob += jetm11Seq

#=======================================
# CREATE THE DERIVATION KERNEL ALGORITHM   
#=======================================
from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__DerivationKernel
jetm11Seq += CfgMgr.DerivationFramework__DerivationKernel('JETM11Kernel',
                                                          SkimmingTools = [JETM11StringSkimmingTool],
                                                          AugmentationTools = [JETM11KFData15AugmentationTool, JETM11KFmc12AugmentationTool],
                                                          ThinningTools = thinningTools)

#======================================================================================================================
# Content list for slimming
#======================================================================================================================
from DerivationFrameworkCore.SlimmingHelper import SlimmingHelper
JETM11SlimmingHelper = SlimmingHelper('JETM11SlimmingHelper')
JETM11SlimmingHelper.SmartCollections = ["Electrons", "Muons", "Photons", "TauJets",
                                         "AntiKt4EMTopoJets", "PrimaryVertices", "BTagging_AntiKt4EMTopo"]
JETM11SlimmingHelper.AllVariables =     ["HLT_xAOD__TrigMissingETContainer_TrigEFMissingET_topocl_PS",
                                         "HLT_xAOD__TrigMissingETContainer_TrigEFMissingET_mht",
                                         "HLT_xAOD__TrigMissingETContainer_TrigEFMissingET_topocl_PUC",
                                         "HLT_xAOD__TrigMissingETContainer_TrigEFMissingET_topocl",
                                         "HLT_xAOD__TrigMissingETContainer_TrigEFMissingET",
                                         "HLT_xAOD__JetContainer_a4tclcwsubjesFS",
                                         "LVL1JetRoIs",
                                         "LVL1JetEtRoI",
                                         "LVL1EnergySumRoI_KFMETData15", 
                                         "LVL1EnergySumRoI_KFMETmc12",
                                         "MET_Core_AntiKt4EMTopo",
                                         "METAssoc_AntiKt4EMTopo"]
JETM11SlimmingHelper.IncludeJetTriggerContent = True
JETM11SlimmingHelper.IncludeEtMissTriggerContent = True
JETM11SlimmingHelper.AppendContentToStream(JETM11Stream)


#======================================================================================================================
# Content Definition
#======================================================================================================================
# JETM11Stream.AddItem("std::vector<float>#JETM11KFEx");
# JETM11Stream.AddItem("std::vector<float>#JETM11KFEy");
