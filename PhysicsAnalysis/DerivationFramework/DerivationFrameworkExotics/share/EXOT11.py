#********************************************************************
# EXOT11.py 
# reductionConf flag EXOT11 in Reco_tf.py   
#********************************************************************

from DerivationFrameworkCore.DerivationFrameworkMaster import *
from DerivationFrameworkInDet.InDetCommon import *
from DerivationFrameworkMuons.MuonsCommon import *
from DerivationFrameworkEGamma.EGammaCommon import *
from DerivationFrameworkJetEtMiss.METCommon import *
from DerivationFrameworkJetEtMiss.JetCommon import *
from DerivationFrameworkJetEtMiss.ExtendedJetCommon import *

from JetRec.JetRecStandard import jtm

#=======================================
# Augmentation: Jet Collections
#=======================================

from DerivationFrameworkExotics.JetDefinitions import *

# Custom Jet Collections
from JetRec.JetRecConf import JetAlgorithm
if not "CamKt08LCTopoJets" in jtm.tools:
  cam08_lctopo_finder = jtm.addJetFinder("CamKt08LCTopoJets", "CamKt", 0.8, "lctopo", "largeR_notruth",
                                         ghostArea=0.01 , ptmin=2000, ptminFilter=20000, calibOpt="none")
  jetalg_cam08_lctopo        = JetAlgorithm("exot11_cam08_lctopo_finder", Tools = [ cam08_lctopo_finder] )
  DerivationFrameworkJob += jetalg_cam08_lctopo

if not "CamKt10LCTopoJets" in jtm.tools:
  cam10_lctopo_finder = jtm.addJetFinder("CamKt10LCTopoJets", "CamKt", 1.0, "lctopo", "largeR_notruth",
                                         ghostArea=0.01 , ptmin=2000, ptminFilter=30000, calibOpt="none")
  jetalg_cam10_lctopo        = JetAlgorithm("exot11_cam10_lctopo_finder", Tools = [ cam10_lctopo_finder] )
  DerivationFrameworkJob += jetalg_cam10_lctopo

#Temp jets to find split filtered jets
if not "CamKt12LCTopoSplitFilteredMu100SmallR30YCut15Jets" in jtm.tools:
  cam12_lctopo_finder = jtm.addJetFinder("tmp_exot11_CamKt12LCTopoJets", "CamKt", 1.2, "lctopo", "largeR_notruth",
                                         ghostArea=0.01, ptmin=2000, ptminFilter=50000, calibOpt="none")

  #Split filtered jets
  cam12_splitfilter = jtm.addJetSplitter("CamKt12LCTopoSplitFilteredMu100SmallR30YCut15Jets",
                                      mumax=1.0, ymin=0.15, input="tmp_exot11_CamKt12LCTopoJets",
                                      modifiersin="groomed")

  jetalg_cam12_splitfiltered = JetAlgorithm("exot11_cam12_splitfilter",   Tools = [ cam12_lctopo_finder,
                                                                                    cam12_splitfilter] )
  DerivationFrameworkJob += jetalg_cam12_splitfiltered

# Add trimmed jet R20
DerivationFrameworkJob += addTrimmedJets("AntiKt", 1.0, "LCTopo", rclus=0.2, ptfrac=0.05)
DerivationFrameworkJob += addTrimmedJets("AntiKt", 1.0, "Truth", rclus=0.2, ptfrac=0.05)
#====================================================================
# THINNING TOOLS
#====================================================================

#Tracks associated with Electrons
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__EgammaTrackParticleThinning
EXOT11ElectronTPThinningTool = DerivationFramework__EgammaTrackParticleThinning( name                   = "EXOT11ElectronTPThinningTool",
                                                                                 ThinningService        = "EXOT11ThinningSvc",
                                                                                 SGKey                  = "Electrons",
                                                                                 InDetTrackParticlesKey = "InDetTrackParticles")
ToolSvc += EXOT11ElectronTPThinningTool

#Tracks associated with Muons
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__MuonTrackParticleThinning
EXOT11MuonTPThinningTool = DerivationFramework__MuonTrackParticleThinning( name                  = "EXOT11MuonTPThinningTool",
                                                                           ThinningService       = "EXOT11ThinningSvc",
                                                                           MuonKey               = "Muons",
                                                                           InDetTrackParticlesKey = "InDetTrackParticles")
ToolSvc += EXOT11MuonTPThinningTool

#Tracks associated with Jets
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__JetTrackParticleThinning
EXOT11AKt4JetTPThinningTool = DerivationFramework__JetTrackParticleThinning( name                    = "EXOT11AKt4JetTPThinningTool",
                                                                             ThinningService         = "EXOT11ThinningSvc",
                                                                             JetKey                  = "AntiKt4LCTopoJets",
                                                                             SelectionString         = "AntiKt4LCTopoJets.pt > 15*GeV && abs(AntiKt4LCTopoJets.eta) < 2.8",
                                                                             InDetTrackParticlesKey  = "InDetTrackParticles")
ToolSvc += EXOT11AKt4JetTPThinningTool

EXOT11AKt10JetTPThinningTool = DerivationFramework__JetTrackParticleThinning( name                    = "EXOT11AKt10JetTPThinningTool",
                                                                              ThinningService         = "EXOT11ThinningSvc",
                                                                              JetKey                  = "AntiKt10LCTopoJets",
                                                                              SelectionString         = "AntiKt10LCTopoJets.pt > 15*GeV && abs(AntiKt10LCTopoJets.eta) < 2.8",
                                                                              InDetTrackParticlesKey  = "InDetTrackParticles")
ToolSvc += EXOT11AKt10JetTPThinningTool

#Truth Thinning (!!! NOT WORKING FOR athena 19)
#from DerivationFrameworkMCTruth.DerivationFrameworkMCTruthConf import DerivationFramework__MenuTruthThinning
#EXOT11MenuThinningTool = DerivationFramework__MenuTruthThinning( name = "EXOT11MenuThinningTool",
#                                                                 ThinningService = "EXOT11ThinningSvc",
#                                                                 WritePartons               = True,
#                                                                 WriteHadrons               = True,
#                                                                 WriteBHadrons              = True,
#                                                                 WriteGeant                 = False,
#                                                                 GeantPhotonPtThresh        = -1.0,
#                                                                 WriteTauHad                = False,
#                                                                 PartonPtThresh             = -1.0,
#                                                                 WriteBSM                   = True,
#                                                                 WriteBosons                = True,
#                                                                 WriteBSMProducts           = True,
#                                                                 WriteTopAndDecays          = True,
#                                                                 WriteEverything            = False,
#                                                                 WriteAllLeptons            = False,
#                                                                 WriteLeptonsNotFromHadrons = True,
#                                                                 WriteStatus3               = False,
#                                                                 WriteFirstN                = -1,
#                                                                 PreserveDescendants        = False)

#ToolSvc += EXOT11MenuThinningTool

#from DerivationFrameworkMCTruth.DerivationFrameworkMCTruthConf import DerivationFramework__GenericTruthThinning
#EXOT11GenThinningTool = DerivationFramework__GenericTruthThinning( name = "EXOT11GenThinningTool",
#                                                                   ThinningService = "EXOT11ThinningSvc",
#                                                                   ParticleSelectionString = "abs(TruthParticle.pdgId) ==34 || abs(TruthParticle.pdgId)==5100039", 
#                                                                   PreserveDescendants = False)

#ToolSvc += EXOT11GenThinningTool

#=======================================
# SKIMMING TOOL   
#=======================================

#trig = " (HLT_e28_tight_iloose || HLT_e60_medium1)|| (HLT_mu26_imedium || HLT_mu50) "
selection_e = "(count((Electrons.Medium) && (Electrons.pt > 20*GeV)) > 0)"
selection_mu = "(count(Muons.pt > 20*GeV) > 0)"
selection_j = "( (count(CamKt12LCTopoJets.pt > 200*GeV) > 0) || (count(AntiKt10LCTopoJets.pt > 200*GeV) > 0) || (count(AntiKt4LCTopoJets.pt > 200*GeV) > 0) )"

beamEnergy = jobproperties.Beam.energy()
expression = ''
if (beamEnergy < 4.1e+06):
    expression = "(" + selection_e + "||" + selection_mu + ")" + "&&" + selection_j
if (beamEnergy > 6.0e+06):
    expression = "(" + selection_e + "||" + selection_mu + ")" + "&&" + selection_j

from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__xAODStringSkimmingTool
EXOT11SkimmingTool = DerivationFramework__xAODStringSkimmingTool( name = "EXOT11SkimmingTool", 
                                                                  expression = expression)
ToolSvc += EXOT11SkimmingTool
print EXOT11SkimmingTool

#=======================================
# THE DERIVATION KERNEL ALGORITHM   
#=======================================
from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__DerivationKernel
DerivationFrameworkJob += CfgMgr.DerivationFramework__DerivationKernel("EXOT11Kernel",
                                                                       SkimmingTools = [EXOT11SkimmingTool],
                                                                       ThinningTools = [EXOT11ElectronTPThinningTool,
                                                                                        EXOT11MuonTPThinningTool,
                                                                                        EXOT11AKt4JetTPThinningTool,
                                                                                        EXOT11AKt10JetTPThinningTool])
#,EXOT11MenuThinningTool

#====================================================================
# SET UP STREAM   
#====================================================================
streamName   = derivationFlags.WriteDAOD_EXOT11Stream.StreamName
fileName     = buildFileName( derivationFlags.WriteDAOD_EXOT11Stream )
EXOT11Stream = MSMgr.NewPoolRootStream( streamName, fileName )
EXOT11Stream.AcceptAlgs(["EXOT11Kernel"])

# Thinning
from AthenaServices.Configurables import ThinningSvc, createThinningSvc
augStream = MSMgr.GetStream( streamName )
evtStream = augStream.GetEventStream()
svcMgr   += createThinningSvc( svcName="EXOT11ThinningSvc", outStreams=[evtStream] )

#====================================================================
# Add the containers to the output stream - slimming done here
#====================================================================
from DerivationFrameworkCore.SlimmingHelper import SlimmingHelper
from DerivationFrameworkExotics.EXOT11ContentList import *
EXOT11SlimmingHelper = SlimmingHelper("EXOT11SlimmingHelper")
EXOT11SlimmingHelper.SmartCollections = EXOT11SmartContent
EXOT11SlimmingHelper.AllVariables = EXOT11AllVariablesContent
EXOT11SlimmingHelper.StaticContent = EXOT11Content
EXOT11SlimmingHelper.AppendContentToStream(EXOT11Stream)

