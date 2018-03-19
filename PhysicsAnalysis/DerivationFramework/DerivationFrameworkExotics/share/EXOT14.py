#********************************************************************
# EXOT14.py
# reductionConf flag EXOT14 in Reco_tf.py
#********************************************************************
from DerivationFrameworkCore.DerivationFrameworkMaster import *
from DerivationFrameworkJetEtMiss.JetCommon import *
from DerivationFrameworkJetEtMiss.ExtendedJetCommon import *
from DerivationFrameworkJetEtMiss.METCommon import *
from DerivationFrameworkEGamma.EGammaCommon import *
from DerivationFrameworkMuons.MuonsCommon import *
from DerivationFrameworkCore.WeightMetadata import *

#====================================================================
# SET UP STREAM
#====================================================================
stream_name = derivationFlags.WriteDAOD_EXOT14Stream.StreamName
file_name = buildFileName(derivationFlags.WriteDAOD_EXOT14Stream)
EXOT14Stream = MSMgr.NewPoolRootStream(stream_name, file_name)
EXOT14Stream.AcceptAlgs(['EXOT14Kernel'])

from DerivationFrameworkCore.ThinningHelper import ThinningHelper
EXOT14ThinningHelper = ThinningHelper('EXOT14ThinningHelper')

# stream-specific sequence
exot14Seq = CfgMgr.AthSequencer('EXOT14Sequence')
DerivationFrameworkJob += exot14Seq

#====================================================================
# Trigger navigation thinning
#====================================================================
# Thinning: what information is kept regarding the trigger navigation
EXOT14ThinningHelper.TriggerChains = "HLT_xe.*|HLT_e.*|HLT_2e.*|HLT_mu.*|HLT_2mu.*"
EXOT14ThinningHelper.AppendToStream(EXOT14Stream)

#====================================================================
# THINNING TOOLS
#====================================================================

# Tracks associated with Muons
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__MuonTrackParticleThinning
EXOT14MuonTPThinningTool = DerivationFramework__MuonTrackParticleThinning( name                    = "EXOT14MuonTPThinningTool",
                                                                           ThinningService         = EXOT14ThinningHelper.ThinningSvc(),
                                                                           MuonKey                 = "Muons",
                                                                           InDetTrackParticlesKey  = "InDetTrackParticles",
                                                                           SelectionString         = "Muons.pt > 5*GeV",
                                                                           ConeSize                = 0.4)
ToolSvc += EXOT14MuonTPThinningTool

# Tracks associated with Electrons
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__EgammaTrackParticleThinning
EXOT14ElectronTPThinningTool = DerivationFramework__EgammaTrackParticleThinning( name                    = "EXOT14ElectronTPThinningTool",
                                                                                 ThinningService         = EXOT14ThinningHelper.ThinningSvc(),
                                                                                 SGKey                	 = "Electrons",
                                                                                 GSFTrackParticlesKey    = "GSFTrackParticles",
                                                                                 InDetTrackParticlesKey  = "InDetTrackParticles",
                                                                                 SelectionString         = "Electrons.pt > 4*GeV",
                                                                                 BestMatchOnly           = True,
                                                                                 ConeSize                = 0.4)
ToolSvc += EXOT14ElectronTPThinningTool

# Tracks associated with Photons
EXOT14PhotonTPThinningTool = DerivationFramework__EgammaTrackParticleThinning( name                    = "EXOT14PhotonTPThinningTool",
                                                                               ThinningService         = EXOT14ThinningHelper.ThinningSvc(),
                                                                               SGKey                   = "Photons",
                                                                               InDetTrackParticlesKey  = "InDetTrackParticles",
                                                                               SelectionString         = "Photons.pt > 18*GeV",
                                                                               ConeSize                = 0.6)
ToolSvc += EXOT14PhotonTPThinningTool

# Calo Clusters associated with Photons
from DerivationFrameworkCalo.DerivationFrameworkCaloConf import DerivationFramework__CaloClusterThinning
EXOT14PhotonCCThinningTool = DerivationFramework__CaloClusterThinning( name                    = "EXOT14PhotonCCThinningTool",
                                                                                     ThinningService         = EXOT14ThinningHelper.ThinningSvc(),
                                                                                     SGKey             	     = "Photons",
                                                                                     CaloClCollectionSGKey   = "egammaClusters",
                                                                                     TopoClCollectionSGKey   = "CaloCalTopoClusters",
                                                                                     SelectionString         = "Photons.pt > 5*GeV",
                                                                                     #FrwdClCollectionSGKey   = "LArClusterEMFrwd",
                                                                                     ConeSize                = 0.6)
ToolSvc += EXOT14PhotonCCThinningTool

# Calo Clusters associated with Electrons
EXOT14ElectronCCThinningTool = DerivationFramework__CaloClusterThinning( name                  = "EXOT14ElectronCCThinningTool",
                                                                                     ThinningService         = EXOT14ThinningHelper.ThinningSvc(),
                                                                                     SGKey             	     = "Electrons",
                                                                                     CaloClCollectionSGKey   = "egammaClusters",
                                                                                     TopoClCollectionSGKey   = "CaloCalTopoClusters",
                                                                                     SelectionString         = "Electrons.pt > 18*GeV",
                                                                                     #FrwdClCollectionSGKey   = "LArClusterEMFrwd",
                                                                                     ConeSize                = 0.4)
ToolSvc += EXOT14ElectronCCThinningTool

if globalflags.DataSource() == 'geant4':
    # MC Thinning
    from DerivationFrameworkMCTruth.DerivationFrameworkMCTruthConf import \
        DerivationFramework__MenuTruthThinning

    EXOT14MCThinningTool = DerivationFramework__MenuTruthThinning(
        name                       = 'EXOT14MCThinningTool',
        ThinningService            = EXOT14ThinningHelper.ThinningSvc(),
        WriteEverything            = False,
        WritePartons               = False,
        PartonPtThresh             = -1.0,
        WriteHadrons               = False,
        WriteBHadrons              = False,
        WriteGeant                 = False,
        GeantPhotonPtThresh        = -1.0,
        WriteTauHad                = True,
        WriteBSM                   = True,
        WriteBosons                = True,
        WriteBSMProducts           = True,
        WriteTopAndDecays          = True,
        WriteAllLeptons            = True,
        #WriteLeptonsNotFromHadrons = True,
        WriteStatus3               = False,
        WriteFirstN                = -1,
        #PreserveDescendants        = False,
        )
    ToolSvc += EXOT14MCThinningTool

applyJetCalibration_xAODColl("AntiKt4EMTopo", exot14Seq)
applyJetCalibration_xAODColl("AntiKt4EMPFlow", exot14Seq)

# Note that triggers from later years are not duplicated if the triggers exit in previous years
lepton_triggers = [
   # el - y2015
   "HLT_e24_lhmedium_L1EM20VH",
   "HLT_e60_lhmedium",
   "HLT_e120_lhloose",
   "HLT_2e12_lhloose_L12EM10VH",
   # el - y2016
   "HLT_e24_lhtight_nod0_ivarloose",
   "HLT_e24_lhmedium_nod0_L1EM20VH",
   "HLT_e26_lhtight_nod0_ivarloose",
   "HLT_e60_lhmedium_nod0",
   "HLT_e60_medium",
   "HLT_e140_lhloose_nod0",
   "HLT_e300_etcut",
   "HLT_2e15_lhvloose_nod0_L12EM13VH",
   "HLT_2e17_lhvloose_nod0",
   # el - y2017
   "HLT_e26_lhtight_nod0_ivarloose_L1EM22VHIM",
   "HLT_e28_lhtight_nod0_ivarloose",
   "HLT_e28_lhtight_nod0_ivarloose_L1EM24VHIM",
   "HLT_e60_lhmedium_nod0_L1EM24VHI",
   "HLT_e140_lhloose_nod0_L1EM24VHI",
   "HLT_e300_etcut_L1EM24VHI",

   # mu - y2015
   "HLT_mu20_iloose_L1MU15",
   "HLT_mu40",
   "HLT_mu60_0eta105_msonly",
   "HLT_2mu10",
   "HLT_mu18_mu8noL1",
   # mu - y2016
   "HLT_mu24_iloose",
   "HLT_mu24_iloose_L1MU15",
   "HLT_mu24_ivarloose",
   "HLT_mu24_ivarloose_L1MU15",
   "HLT_mu24_ivarmedium",
   "HLT_mu24_imedium",
   "HLT_mu26_ivarmedium",
   "HLT_mu26_imedium",
   "HLT_mu50",
   "HLT_2mu10_nomucomb",
   "HLT_2mu14",
   "HLT_2mu14_nomucomb",
   "HLT_mu20_mu8noL1",
   "HLT_mu20_nomucomb_mu6noL1_nscan03",
   "HLT_mu20_msonly_mu10noL1_msonly_nscan05_noComb",
   "HLT_mu22_mu8noL1",
   # mu - y2017
   "HLT_mu60",
   "HLT_mu22_mu8noL1_calotag_0eta010",
   "HLT_mu24_mu8noL1",
   "HLT_mu24_mu8noL1_calotag_0eta010",
   "HLT_mu26_mu10noL1"
]

met_triggers = [
   # xe - y2015
   "HLT_xe70",
   "HLT_xe70_mht",
   "HLT_xe70_tc_lcw",
   # xe - y2016
   "HLT_xe80_tc_lcw_L1XE50",
   "HLT_xe90_mht_L1XE50",
   "HLT_xe90_tc_lcw_wEFMu_L1XE50",
   "HLT_xe90_mht_wEFMu_L1XE50",
   "HLT_xe100_L1XE50",
   "HLT_xe100_tc_em_L1XE50",
   "HLT_xe100_mht_L1XE50",
   "HLT_xe110_pueta_L1XE50",
   "HLT_xe110_mht_L1XE50",
   "HLT_xe120_tc_lcw_L1XE50",
   "HLT_xe120_pueta",
   "HLT_xe120_pufit",
   # xe - y2017
   "HLT_xe90_pufit_L1XE50",
   "HLT_xe100_pufit_L1XE50",
   "HLT_xe100_pufit_L1XE55",
   "HLT_xe110_pufit_L1XE50",
   "HLT_xe110_pufit_L1XE55",
   "HLT_xe110_pufit_L1XE60",
   "HLT_xe120_pufit_L1XE50",
   "HLT_xe120_pufit_L1XE55",
   "HLT_xe120_pufit_L1XE60",
   "HLT_noalg_L1J400"
]

from DerivationFrameworkExotics.DerivationFrameworkExoticsConf import DerivationFramework__SkimmingToolEXOT14
EXOT14SkimmingTool = DerivationFramework__SkimmingToolEXOT14(
                                                name = "EXOT14SkimmingTool",
                                                RequireGRL = False,
                                                ReqireLArError = False,
                                                RequireTrigger = True,
                                                RequireJetPts = True,
                                                RequireJetsDEta = True,
                                                RequireDiJetMass = False,
                                                RequireJetsDPhi = False,
                                                DiJetsMass = 5e5,
                                                EtaSeparation = 2.5,
                                                LeadingJetPtCut = 50e3,
                                                SubleadingJetPtCut = 35e3,
                                                DiJetDPhi = 2.7,
                                                Triggers = met_triggers)

ToolSvc += EXOT14SkimmingTool
print EXOT14SkimmingTool

#  Events we skim on
from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__TriggerSkimmingTool
EXOT14TrigSkimmingTool = DerivationFramework__TriggerSkimmingTool(name = "EXOT14TrigSkimmingTool", TriggerListOR = lepton_triggers)

ToolSvc += EXOT14TrigSkimmingTool

from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__FilterCombinationOR

EXOT14ORTool=DerivationFramework__FilterCombinationOR(name="EXOT14ORTool", FilterList=[EXOT14TrigSkimmingTool, EXOT14SkimmingTool])
ToolSvc+=EXOT14ORTool

#=======================================
# JETS
#=======================================

#restore AOD-reduced jet collections
from DerivationFrameworkJetEtMiss.ExtendedJetCommon import replaceAODReducedJets
OutputJets["EXOT14"] = []
reducedJetList = [
    "AntiKt4PV0TrackJets",
    "AntiKt4TruthJets"]
replaceAODReducedJets(reducedJetList,exot14Seq,"EXOT14")

#=======================================
# CREATE THE DERIVATION KERNEL ALGORITHM
#=======================================

isMC = False
if globalflags.DataSource() == 'geant4':
  isMC = True
from TrigBunchCrossingTool.BunchCrossingTool import BunchCrossingTool
if isMC:
  ToolSvc += BunchCrossingTool( "MC" )
else:
  ToolSvc += BunchCrossingTool( "LHC" )

from DerivationFrameworkExotics.DerivationFrameworkExoticsConf import DerivationFramework__BCDistanceAugmentationTool

EXOT14BCDistanceAugmentationTool = DerivationFramework__BCDistanceAugmentationTool(name="EXOT14BCDistanceAugmentationTool")

if isMC:
  EXOT14BCDistanceAugmentationTool.BCTool = "Trig::MCBunchCrossingTool/BunchCrossingTool"
else:
  EXOT14BCDistanceAugmentationTool.BCTool = "Trig::LHCBunchCrossingTool/BunchCrossingTool"
ToolSvc += EXOT14BCDistanceAugmentationTool

#different thinning tools for data/MC
EXOT14ThinningTools=[EXOT14MuonTPThinningTool, EXOT14ElectronTPThinningTool, EXOT14PhotonTPThinningTool,EXOT14PhotonCCThinningTool, EXOT14ElectronCCThinningTool]
if globalflags.DataSource() == 'geant4':
    EXOT14ThinningTools.append(EXOT14MCThinningTool)

from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__DerivationKernel
exot14Seq += CfgMgr.DerivationFramework__DerivationKernel("EXOT14Kernel_skim", SkimmingTools = [EXOT14ORTool])
exot14Seq += CfgMgr.DerivationFramework__DerivationKernel("EXOT14Kernel", ThinningTools = EXOT14ThinningTools,
                                                                          AugmentationTools = [EXOT14BCDistanceAugmentationTool])

#====================================================================
# Add the containers to the output stream - slimming done here
#====================================================================
from DerivationFrameworkCore.SlimmingHelper import SlimmingHelper
from DerivationFrameworkExotics.EXOT14ContentList import *
EXOT14SlimmingHelper = SlimmingHelper("EXOT14SlimmingHelper")
EXOT14SlimmingHelper.StaticContent = EXOT14Content

EXOT14SlimmingHelper.SmartCollections = EXOT14SmartContent
EXOT14SlimmingHelper.AllVariables = EXOT14AllVariablesContent
EXOT14SlimmingHelper.UserContent = EXOT14Content
EXOT14SlimmingHelper.ExtraVariables = EXOT14ExtraVariablesContent

EXOT14SlimmingHelper.IncludeEGammaTriggerContent = True
EXOT14SlimmingHelper.IncludeMuonTriggerContent = True
EXOT14SlimmingHelper.IncludeEtMissTriggerContent = True

EXOT14SlimmingHelper.AppendContentToStream(EXOT14Stream)
