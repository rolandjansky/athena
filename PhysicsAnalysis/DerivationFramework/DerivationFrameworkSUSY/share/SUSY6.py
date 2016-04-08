#********************************************************************
# SUSY6.py
# Derivation for pixel dE/dx based search for CHAMPs
#********************************************************************

from DerivationFrameworkCore.DerivationFrameworkMaster import *
from DerivationFrameworkJetEtMiss.JetCommon import *
from DerivationFrameworkJetEtMiss.ExtendedJetCommon import *
from DerivationFrameworkEGamma.EGammaCommon import *
from DerivationFrameworkMuons.MuonsCommon import *
from DerivationFrameworkInDet.InDetCommon import *
from DerivationFrameworkJetEtMiss.METCommon import *
     
### Set up stream
streamName = derivationFlags.WriteDAOD_SUSY6Stream.StreamName
fileName   = buildFileName( derivationFlags.WriteDAOD_SUSY6Stream )
SUSY6Stream = MSMgr.NewPoolRootStream( streamName, fileName )
SUSY6Stream.AcceptAlgs(["SUSY6KernelSkim"])

### Init
from DerivationFrameworkCore.ThinningHelper import ThinningHelper
SUSY6ThinningHelper = ThinningHelper( "SUSY6ThinningHelper" )
thinningTools       = []
AugmentationTools   = []

# stream-specific sequence for on-the-fly jet building
SeqSUSY6 = CfgMgr.AthSequencer("SeqSUSY6")
DerivationFrameworkJob += SeqSUSY6


#====================================================================
# Trigger navigation thinning
#====================================================================
SUSY6ThinningHelper.TriggerChains = 'HLT_xe.*|HLT_2mu14|HLT_mu50'
SUSY6ThinningHelper.AppendToStream( SUSY6Stream )


#====================================================================
# THINNING TOOLS 
#====================================================================

thinning_expression = "(InDetTrackParticles.pt > 0.5*GeV) && (InDetTrackParticles.numberOfPixelHits > 0) && (abs(DFCommonInDetTrackZ0AtPV) < 3.0)"
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__TrackParticleThinning
SUSY6MetTPThinningTool = DerivationFramework__TrackParticleThinning( name               = "SUSY6MetTPThinningTool",
                                                                ThinningService         = SUSY6ThinningHelper.ThinningSvc(),
                                                                SelectionString         = thinning_expression,
                                                                InDetTrackParticlesKey  = "InDetTrackParticles",
                                                                ApplyAnd                = True)
ToolSvc += SUSY6MetTPThinningTool
thinningTools.append(SUSY6MetTPThinningTool)


# TrackParticles directly
SUSY6TPThinningTool = DerivationFramework__TrackParticleThinning(name = "SUSY6TPThinningTool",
                                                                 ThinningService         = SUSY6ThinningHelper.ThinningSvc(),
                                                                 SelectionString         = "InDetTrackParticles.pt > 1*GeV",
                                                                 InDetTrackParticlesKey  = "InDetTrackParticles")
ToolSvc += SUSY6TPThinningTool
thinningTools.append(SUSY6TPThinningTool)

# TrackParticles associated with Muons
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__MuonTrackParticleThinning
SUSY6MuonTPThinningTool = DerivationFramework__MuonTrackParticleThinning(name                    = "SUSY6MuonTPThinningTool",
                                                                         ThinningService         = SUSY6ThinningHelper.ThinningSvc(),
                                                                         MuonKey                 = "Muons",
                                                                         InDetTrackParticlesKey  = "InDetTrackParticles")
ToolSvc += SUSY6MuonTPThinningTool
thinningTools.append(SUSY6MuonTPThinningTool)

# TrackParticles associated with electrons
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__EgammaTrackParticleThinning
SUSY6ElectronTPThinningTool = DerivationFramework__EgammaTrackParticleThinning(name                    	= "SUSY6ElectronTPThinningTool",
                                                                                 ThinningService        = SUSY6ThinningHelper.ThinningSvc(),
                                                                                 SGKey             	= "Electrons",
                                                                                 InDetTrackParticlesKey = "InDetTrackParticles")
ToolSvc += SUSY6ElectronTPThinningTool
thinningTools.append(SUSY6ElectronTPThinningTool)


#====================================================================
# TRUTH THINNING
#====================================================================
if DerivationFrameworkIsMonteCarlo:
  from DerivationFrameworkMCTruth.DerivationFrameworkMCTruthConf import DerivationFramework__MenuTruthThinning
  SUSY6TruthThinningTool = DerivationFramework__MenuTruthThinning(name              = "SUSY6TruthThinningTool",
                                                       ThinningService              = SUSY6ThinningHelper.ThinningSvc(),
                                                       WritePartons                 = False,
                                                       WriteHadrons                 = False,
                                                       WriteBHadrons                = False, 
                                                       WriteGeant                   = False,
                                                       GeantPhotonPtThresh          = 20000,
                                                       WriteTauHad                  = False,
                                                       PartonPtThresh               = -1.0,
                                                       WriteBSM                     = True,
                                                       WriteBosons                  = True,
                                                       WriteBosonProducts           = True, 
                                                       WriteBSMProducts             = True,
                                                       WriteTopAndDecays            = True, 
                                                       WriteEverything              = False,
                                                       WriteAllLeptons              = False,
                                                       WriteLeptonsNotFromHadrons   = False,
                                                       WriteStatus3                 = False,
                                                       WriteFirstN                  = -1,
                                                       PreserveAncestors            = True, 
                                                       PreserveGeneratorDescendants = False,
                                                       SimBarcodeOffset             = DerivationFrameworkSimBarcodeOffset)
  ToolSvc += SUSY6TruthThinningTool
  thinningTools.append(SUSY6TruthThinningTool)


        
#====================================================================
# SKIMMING TOOL 
#====================================================================

muonTriggers = ['HLT_mu50']

dimuonTriggers = ['HLT_2mu14']

xeTriggers = ['HLT_xe35',
              'HLT_xe35_tc_lcw',
              'HLT_xe35_tc_em',
              'HLT_xe35_pueta',
              'HLT_xe35_pufit',
              'HLT_xe35_mht',
              'HLT_xe35_L2FS',
              'HLT_xe35_l2fsperf_wEFMuFEB_wEFMu',
              'HLT_xe35_wEFMu',
              'HLT_xe35_tc_lcw_wEFMu',
              'HLT_xe35_tc_em_wEFMu',
              'HLT_xe35_mht_wEFMu',
              'HLT_xe35_pueta_wEFMu',
              'HLT_xe35_pufit_wEFMu',
              'HLT_xe50',
              'HLT_xe50_tc_lcw',
              'HLT_xe50_tc_em',
              'HLT_xe50_mht',
              'HLT_xe50_pueta',
              'HLT_xe50_pufit',
              'HLT_xe60',
              'HLT_xe60_tc_lcw',
              'HLT_xe60_tc_em',
              'HLT_xe60_mht',
              'HLT_xe60_pueta',
              'HLT_xe60_pufit',
              'HLT_xe60_wEFMu',
              'HLT_xe60_tc_lcw_wEFMu',
              'HLT_xe60_tc_em_wEFMu',
              'HLT_xe60_mht_wEFMu',
              'HLT_xe60_pueta_wEFMu',
              'HLT_xe60_pufit_wEFMu',
              'HLT_xe70_wEFMu',
              'HLT_xe70_tc_lcw_wEFMu',
              'HLT_xe70_tc_em_wEFMu',
              'HLT_xe70_mht_wEFMu',
              'HLT_xe70_pueta_wEFMu',
              'HLT_xe70_pufit_wEFMu',
              'HLT_xe70',
              'HLT_xe70_tc_lcw',
              'HLT_xe70_tc_em',
              'HLT_xe70_mht',
              'HLT_xe70_pueta',
              'HLT_xe70_pufit',
              'HLT_xe80_L1XE50',
              'HLT_xe80_tc_lcw_L1XE50',
              'HLT_xe80_tc_em_L1XE50',
              'HLT_xe80_mht_L1XE50',
              'HLT_xe80_pueta_L1XE50',
              'HLT_xe80_pufit_L1XE50',
              'HLT_xe80_wEFMu_L1XE50',
              'HLT_xe80_tc_lcw_wEFMu_L1XE50',
              'HLT_xe80_tc_em_wEFMu_L1XE50',
              'HLT_xe80_mht_wEFMu_L1XE50',
              'HLT_xe80_pueta_wEFMu_L1XE50',
              'HLT_xe80_pufit_wEFMu_L1XE50',
              'HLT_xe80',
              'HLT_xe80_tc_lcw',
              'HLT_xe80_tc_em',
              'HLT_xe80_mht',
              'HLT_xe80_pueta',
              'HLT_xe80_pufit',
              'HLT_xe80_wEFMu',
              'HLT_xe80_tc_lcw_wEFMu',
              'HLT_xe80_tc_em_wEFMu',
              'HLT_xe80_mht_wEFMu',
              'HLT_xe80_pueta_wEFMu',
              'HLT_xe80_pufit_wEFMu',
              'HLT_xe80_L1XE70',
              'HLT_xe80_tc_lcw_L1XE70',
              'HLT_xe80_tc_em_L1XE70',
              'HLT_xe80_mht_L1XE70',
              'HLT_xe80_pueta_L1XE70',
              'HLT_xe80_pufit_L1XE70',
              'HLT_xe80_wEFMu_L1XE70',
              'HLT_xe80_tc_lcw_wEFMu_L1XE70',
              'HLT_xe80_tc_em_wEFMu_L1XE70',
              'HLT_xe80_mht_wEFMu_L1XE70',
              'HLT_xe80_pueta_wEFMu_L1XE70',
              'HLT_xe80_pufit_wEFMu_L1XE70',
              'HLT_xe100',
              'HLT_xe100_tc_lcw',
              'HLT_xe100_tc_em',
              'HLT_xe100_mht',
              'HLT_xe100_pueta',
              'HLT_xe100_pufit',
              'HLT_xe100_wEFMu',
              'HLT_xe100_tc_lcw_wEFMu',
              'HLT_xe100_tc_em_wEFMu',
              'HLT_xe100_mht_wEFMu',
              'HLT_xe100_pueta_wEFMu',
              'HLT_xe100_pufit_wEFMu']


if DerivationFrameworkIsMonteCarlo:
        xeTriggers = [trig for trig in xeTriggers if 'xe100' in trig]

expression = '(' + ' || '.join(xeTriggers+muonTriggers+dimuonTriggers) + ')' 

from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__xAODStringSkimmingTool
SUSY6SkimmingTool = DerivationFramework__xAODStringSkimmingTool( name = "SUSY6SkimmingTool",
                                                                expression = expression)

ToolSvc += SUSY6SkimmingTool

#=======================================
# CREATE THE DERIVATION KERNEL ALGORITHM   
#=======================================
from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__DerivationKernel
DerivationFrameworkJob += CfgMgr.DerivationFramework__DerivationKernel(
  "SUSY6KernelSkim",
  SkimmingTools = [SUSY6SkimmingTool]
)

#==============================================================================
# SUSY signal augmentation
#==============================================================================
from DerivationFrameworkSUSY.DecorateSUSYProcess import DecorateSUSYProcess
AugmentationTools += DecorateSUSYProcess("SUSY6")

#==============================================================================
# SUSY background generator filters
#==============================================================================
if globalflags.DataSource() == 'geant4':
  replaceBuggyAntiKt4TruthWZJets(SeqSUSY6)
  ToolSvc += CfgMgr.DerivationFramework__SUSYGenFilterTool(
    "SUSY6GenFilt",
    SimBarcodeOffset = DerivationFrameworkSimBarcodeOffset
  )
  AugmentationTools.append(ToolSvc.SUSY6GenFilt)

#==============================================================================
# Augment after skim
#==============================================================================
SeqSUSY6 += CfgMgr.DerivationFramework__DerivationKernel(
	"SUSY6KernelAug",
	AugmentationTools = AugmentationTools,
	ThinningTools = thinningTools,
)


#====================================================================
# CONTENT LIST  
#====================================================================

from DerivationFrameworkCore.SlimmingHelper import SlimmingHelper
SUSY6SlimmingHelper = SlimmingHelper("SUSY6SlimmingHelper")
SUSY6SlimmingHelper.SmartCollections = ["Electrons", "Photons", "Muons", "AntiKt4EMTopoJets", "MET_Reference_AntiKt4EMTopo", "PrimaryVertices"]
SUSY6SlimmingHelper.AllVariables = ["TruthParticles", "TruthEvents", "TruthVertices", "MET_Truth", "InDetTrackParticles", "MET_Reference_AntiKt4EMTopo", "MET_Core_AntiKt4EMTopo","METAssoc_AntiKt4EMTopo", "InDetPixelPrdAssociationTrackParticles"]
SUSY6SlimmingHelper.ExtraVariables = [ "AntiKt4EMTopoJets.NumTrkPt1000.TrackWidthPt1000.NumTrkPt500",
          "AntiKt4TruthJets.eta.m.phi.pt.TruthLabelDeltaR_B.TruthLabelDeltaR_C.TruthLabelDeltaR_T.TruthLabelID.ConeTruthLabelID.PartonTruthLabelID"]
SUSY6SlimmingHelper.IncludeMuonTriggerContent = True
#SUSY6SlimmingHelper.IncludeJetTauEtMissTriggerContent = True
SUSY6SlimmingHelper.IncludeJetTriggerContent = True
SUSY6SlimmingHelper.IncludeTauTriggerContent = True
SUSY6SlimmingHelper.IncludeEtMissTriggerContent = True
SUSY6SlimmingHelper.IncludeBJetTriggerContent = True

SUSY6SlimmingHelper.AppendContentToStream(SUSY6Stream)
