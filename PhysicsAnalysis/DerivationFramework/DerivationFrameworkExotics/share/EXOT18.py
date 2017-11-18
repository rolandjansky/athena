#********************************************************************
# EXOT18.py 
# reductionConf flag EXOT18 in Reco_tf.py   
#********************************************************************
from DerivationFrameworkCore.DerivationFrameworkMaster import *
from DerivationFrameworkInDet.InDetCommon import *
from DerivationFrameworkJetEtMiss.JetCommon import *
from DerivationFrameworkJetEtMiss.ExtendedJetCommon import *
from DerivationFrameworkJetEtMiss.METCommon import *
from DerivationFrameworkEGamma.EGammaCommon import *
from DerivationFrameworkMuons.MuonsCommon import *
from DerivationFrameworkCore.WeightMetadata import *

from JetRec.JetRecStandard import jtm

exot18Seq = CfgMgr.AthSequencer("EXOT18Sequence")

from AthenaCommon.GlobalFlags import globalflags
isMC = False
if globalflags.DataSource()=='geant4':
    isMC = True

#====================================================================
# THINNING TOOL 
#====================================================================
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__TrackParticleThinning
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__MuonTrackParticleThinning
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__EgammaTrackParticleThinning
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__JetTrackParticleThinning
from DerivationFrameworkCalo.DerivationFrameworkCaloConf import DerivationFramework__CaloClusterThinning
from DerivationFrameworkCalo.DerivationFrameworkCaloConf import DerivationFramework__JetCaloClusterThinning
from DerivationFrameworkMCTruth.DerivationFrameworkMCTruthConf import DerivationFramework__MenuTruthThinning

thinningTools = []

# Track particle thinning
thinExpression = '(InDetTrackParticles.d0 < 1.5) && ((DFCommonInDetTrackZ0AtPV * sin(InDetTrackParticles.theta )) <= 1.5)'
EXOT18TPThinningTool = DerivationFramework__TrackParticleThinning(name = "EXOT18TPThinningTool",
                                                                  ThinningService         = "EXOT18ThinningSvc",
                                                                  SelectionString         = thinExpression,
                                                                  InDetTrackParticlesKey  = "InDetTrackParticles")
ToolSvc += EXOT18TPThinningTool
thinningTools += EXOT18TPThinningTool

# Tracks associated with Muons
EXOT18MuonTPThinningTool = DerivationFramework__MuonTrackParticleThinning(name                    = "EXOT18MuonTPThinningTool",
                                                                          ThinningService         = "EXOT18ThinningSvc",
                                                                          MuonKey                 = "Muons",
                                                                          InDetTrackParticlesKey  = "InDetTrackParticles")
ToolSvc += EXOT18MuonTPThinningTool
thinningTools += EXOT18MuonTPThinningTool

# Tracks associated with Electrons
EXOT18ElectronTPThinningTool = DerivationFramework__EgammaTrackParticleThinning(name                    = "EXOT18ElectronTPThinningTool",
                                                                                ThinningService         = "EXOT18ThinningSvc",
                                                                                SGKey            	    = "Electrons",
                                                                                InDetTrackParticlesKey  = "InDetTrackParticles")
ToolSvc += EXOT18ElectronTPThinningTool
thinningTools += EXOT18ElectronTPThinningTool

# Tracks associated with Photons
EXOT18PhotonTPThinningTool = DerivationFramework__EgammaTrackParticleThinning(name                    = "EXOT18PhotonTPThinningTool",
                                                                              ThinningService         = "EXOT18ThinningSvc",
                                                                              SGKey                   = "Photons",
                                                                              InDetTrackParticlesKey  = "InDetTrackParticles")
ToolSvc += EXOT18PhotonTPThinningTool
thinningTools += EXOT18PhotonTPThinningTool

# Tracks associated with small-R jets
EXOT18AKt4JetTPThinningTool = DerivationFramework__JetTrackParticleThinning(name                    = "EXOT18AKt4JetTPThinningTool",
                                                                            ThinningService         = "EXOT18ThinningSvc",
                                                                            JetKey                  = "AntiKt4LCTopoJets",
                                                                            SelectionString         = "AntiKt4LCTopoJets.pt > 15*GeV && abs(AntiKt4LCTopoJets.eta) < 2.8",
                                                                            InDetTrackParticlesKey  = "InDetTrackParticles")
ToolSvc += EXOT18AKt4JetTPThinningTool
thinningTools.append(EXOT18AKt4JetTPThinningTool)

# Tracks associated with large-R jets
EXOT18AKt10JetTPThinningTool = DerivationFramework__JetTrackParticleThinning(name                   = "EXOT18AKt10JetTPThinningTool",
                                                                             ThinningService        = "EXOT18ThinningSvc",
                                                                             JetKey                 = "AntiKt10LCTopoJets",
                                                                             SelectionString        = "AntiKt10LCTopoJets.pt > 150*GeV && abs(AntiKt10LCTopoJets.eta) < 2.8",
                                                                             InDetTrackParticlesKey = "InDetTrackParticles")
ToolSvc += EXOT18AKt10JetTPThinningTool
thinningTools += EXOT18AKt10JetTPThinningTool

# Calo Clusters associated with Electrons
EXOT18ElectronCCThinningTool = DerivationFramework__CaloClusterThinning(name                    = "EXOT18ElectronCCThinningTool",
                                                                        ThinningService         = "EXOT18ThinningSvc",
                                                                        SGKey                   = "Electrons",
                                                                        CaloClCollectionSGKey   = "egammaClusters",
                                                                        TopoClCollectionSGKey   = "",
                                                                        ConeSize                = 0)
ToolSvc += EXOT18ElectronCCThinningTool
thinningTools += EXOT18ElectronCCThinningTool

# Calo Clusters associated with Photons
EXOT18PhotonCCThinningTool = DerivationFramework__CaloClusterThinning(name                    = "EXOT18PhotonCCThinningTool",
                                                                      ThinningService         = "EXOT18ThinningSvc",
                                                                      SGKey                   = "Photons",
                                                                      CaloClCollectionSGKey   = "egammaClusters",
                                                                      TopoClCollectionSGKey   = "CaloCalTopoClusters",
                                                                      SelectionString         = "Photons.pt > 100*GeV",
                                                                      ConeSize                = 0)
ToolSvc += EXOT18PhotonCCThinningTool
thinningTools += EXOT18PhotonCCThinningTool

# Calo Clusters associated with small-R jets
EXOT18AKt10CCThinningTool = DerivationFramework__JetCaloClusterThinning(name                  = "EXOT18Ak10CCThinningTool",
                                                                        ThinningService       = "EXOT18ThinningSvc",
                                                                        SGKey                 = "AntiKt10LCTopoJets",
                                                                        TopoClCollectionSGKey = "CaloCalTopoClusters",
                                                                        SelectionString       = "AntiKt10LCTopoJets.pt > 150*GeV && abs(AntiKt10LCTopoJets.eta) < 2.8",
                                                                        AdditionalClustersKey = ["LCOriginTopoClusters"])
ToolSvc += EXOT18AKt10CCThinningTool
thinningTools += EXOT18AKt10CCThinningTool

#Truth Thinning
EXOT18MCThinningTool = DerivationFramework__MenuTruthThinning(name                = "EXOT18MCThinningTool",
                                                              ThinningService     = "EXOT18ThinningSvc",
                                                              WriteEverything     = False,
                                                              WritePartons        = False,
                                                              WriteHadrons        = True,
                                                              WriteBHadrons       = True,
                                                              WriteGeant          = False,
                                                              WriteTauHad         = False,
                                                              WriteBSM            = True,
                                                              WriteBosons         = True,
                                                              WriteBosonProducts  = True,
                                                              WriteBSMProducts    = True,
                                                              WriteTopAndDecays   = True,
                                                              WriteAllLeptons     = False,
                                                              WriteStatus3        = False,
                                                              WriteFirstN         = -1)

if isMC:
    ToolSvc += EXOT18MCThinningTool
    thinningTools += EXOT18MCThinningTool

#====================================================================
# SKIMMING TOOL 
#====================================================================
from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__xAODStringSkimmingTool
from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__TriggerSkimmingTool
from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__FilterCombinationAND
from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__FilterCombinationOR

# Photon trigger
photon_trigger_expression = "(HLT_g140_loose || HLT_g300_etcut) && (count(Photons.pt > 100*GeV) > 0)"
EXOT18PhotonSkimmingTool = DerivationFramework__xAODStringSkimmingTool(name = "EXOT18PhotonSkimmingTool",
                                                                       expression = photon_trigger_expression)
ToolSvc += EXOT18PhotonSkimmingTool

# Jet trigger
triggers = [
    "HLT_j340",
    "HLT_j380",
    "HLT_j360_a10r_L1J100",
    "HLT_j400_a10r_L1J100",
    "HLT_j420_a10r_L1J100",
    "HLT_j360_a10_lcw_L1J100",
    "HLT_j400_a10_lcw_L1J100",
    "HLT_j420_a10_lcw_L1J100",
    "HLT_ht700_L1J75",
    "HLT_ht1000",
    "HLT_ht1000_L1J100",
]
EXOT18JetTriggerSkimmingTool = DerivationFramework__TriggerSkimmingTool(name = "EXOT18JetTriggerSkimmingTool",
                                                                        TriggerListAND = [],
                                                                        TriggerListOR  = triggers)
ToolSvc += EXOT18JetTriggerSkimmingTool

# Jet selection
jet_expression = "count(abs(AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets.DFCommonJets_Calib_eta) < 2.8 && AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets.DFCommonJets_Calib_pt > 150*GeV && AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets.DFCommonJets_Calib_m > 30*GeV) >= 1"
EXOT18JetSkimmingTool = DerivationFramework__xAODStringSkimmingTool(name = "EXOT18JetSkimmingTool",
                                                                    expression = jet_expression)
ToolSvc += EXOT18JetSkimmingTool

# Photon OR jet trigger
EXOT18ORSkimmingTool = DerivationFramework__FilterCombinationOR(name = "EXOT18ORSkimmingTool",
                                                                FilterList = [EXOT18PhotonSkimmingTool, EXOT18JetTriggerSkimmingTool])
ToolSvc += EXOT18ORSkimmingTool

# Jet selection AND OR tool
EXOT18FinalSkimmingTool = DerivationFramework__FilterCombinationAND(name = "EXOT18FinalSkimmingTool",
                                                                    FilterList = [EXOT18JetSkimmingTool, EXOT18ORSkimmingTool])
ToolSvc += EXOT18FinalSkimmingTool


#=======================================
# JETS
#=======================================

#restore AOD-reduced jet collections
from DerivationFrameworkJetEtMiss.ExtendedJetCommon import replaceAODReducedJets
OutputJets["EXOT18"] = []
reducedJetList = [
                  "AntiKt2PV0TrackJets", #flavour-tagged automatically
                  "AntiKt4PV0TrackJets",
                  "AntiKt4TruthJets",
                  "AntiKt10LCTopoJets"]
replaceAODReducedJets(reducedJetList,exot18Seq,"EXOT18")

#AntiKt10*PtFrac5SmallR20Jets must be scheduled *AFTER* the other collections are replaced
from DerivationFrameworkJetEtMiss.ExtendedJetCommon import addDefaultTrimmedJets
addDefaultTrimmedJets(exot18Seq,"EXOT18")

#jet calibration
applyJetCalibration_CustomColl("AntiKt10LCTopoTrimmedPtFrac5SmallR20", exot18Seq)


#=======================================
# CREATE THE DERIVATION KERNEL ALGORITHM   
#=======================================
from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__DerivationKernel
DerivationFrameworkJob += exot18Seq
exot18Seq += CfgMgr.DerivationFramework__DerivationKernel("EXOT18Kernel_skim", SkimmingTools = [EXOT18FinalSkimmingTool])
exot18Seq += CfgMgr.DerivationFramework__DerivationKernel("EXOT18Kernel", AugmentationTools = [], ThinningTools = thinningTools)

#====================================================================
# SET UP STREAM   
#====================================================================
from AthenaServices.Configurables import ThinningSvc, createThinningSvc

streamName = derivationFlags.WriteDAOD_EXOT18Stream.StreamName
fileName   = buildFileName( derivationFlags.WriteDAOD_EXOT18Stream )
EXOT18Stream = MSMgr.NewPoolRootStream( streamName, fileName )
EXOT18Stream.AcceptAlgs(["EXOT18Kernel"])

# SPECIAL LINES FOR THINNING
# Thinning service name must match the one passed to the thinning tools
augStream = MSMgr.GetStream( streamName )
evtStream = augStream.GetEventStream()
svcMgr += createThinningSvc( svcName="EXOT18ThinningSvc", outStreams=[evtStream] )

#====================================================================
# Add the containers to the output stream - slimming done here
#====================================================================
from DerivationFrameworkCore.SlimmingHelper import SlimmingHelper
from DerivationFrameworkExotics.EXOT18ContentList import EXOT18SmartContent, EXOT18AllVariablesContent, EXOT18ExtraVariables, EXOT18UnslimmedContent

EXOT18SlimmingHelper = SlimmingHelper("EXOT18SlimmingHelper")
EXOT18SlimmingHelper.SmartCollections = EXOT18SmartContent
EXOT18SlimmingHelper.AllVariables = EXOT18AllVariablesContent
EXOT18SlimmingHelper.ExtraVariables = EXOT18ExtraVariables
EXOT18SlimmingHelper.StaticContent = EXOT18UnslimmedContent

addJetOutputs(EXOT18SlimmingHelper, ["EXOT18"])

EXOT18SlimmingHelper.AppendToDictionary = {}
listJets = ['AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets']
if globalflags.DataSource()=='geant4':
    listJets.extend(['AntiKt10TruthTrimmedPtFrac5SmallR20Jets'])
for i in listJets:
    EXOT18SlimmingHelper.AppendToDictionary[i] = 'xAOD::JetContainer'
    EXOT18SlimmingHelper.AppendToDictionary[i+'Aux'] = 'xAOD::JetAuxContainer'
  
EXOT18SlimmingHelper.IncludeJetTriggerContent = True
EXOT18SlimmingHelper.IncludeEGammaTriggerContent = True

EXOT18SlimmingHelper.AppendContentToStream(EXOT18Stream)
