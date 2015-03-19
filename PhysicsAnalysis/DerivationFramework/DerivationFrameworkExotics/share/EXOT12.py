#********************************************************************
# EXOT12.py 
# reductionConf flag EXOT12 in Reco_tf.py   
#********************************************************************

from DerivationFrameworkCore.DerivationFrameworkMaster import *
from DerivationFrameworkInDet.InDetCommon import *
from DerivationFrameworkMuons.MuonsCommon import *
from DerivationFrameworkEGamma.EGammaCommon import *
from DerivationFrameworkJetEtMiss.METCommon import *
from DerivationFrameworkJetEtMiss.JetCommon import *
from DerivationFrameworkJetEtMiss.ExtendedJetCommon import *

from JetRec.JetRecStandard import jtm

from DerivationFrameworkExotics.JetDefinitions import *

#=======================================
# Augmentation: Jet Collections
#=======================================

from JetRec.JetRecConf import JetAlgorithm
# Custom Jet Collections
if not "CamKt10LCTopoJets" in jtm.tools:
  cam10_lctopo_finder = jtm.addJetFinder("CamKt10LCTopoJets", "CamKt", 1.0, "lctopo", "largeR_notruth",
                                         ghostArea=0.01 , ptmin=2000, ptminFilter=30000, calibOpt="none")
  jetalg_cam10_lctopo        = JetAlgorithm("exot12_cam10_lctopo_finder", Tools = [ cam10_lctopo_finder] )
  DerivationFrameworkJob += jetalg_cam10_lctopo

#Temp jets to find split filtered jets
if not "CamKt10LCTopoJets" in jtm.tools:
  cam12_lctopo_finder = jtm.addJetFinder("tmp_exot12_CamKt12LCTopoJets", "CamKt", 1.2, "lctopo", "largeR_notruth",
                                         ghostArea=0.01, ptmin=2000, ptminFilter=50000, calibOpt="none")

  #Split filtered jets
  cam12_splitfilter = jtm.addJetSplitter("CamKt12LCTopoSplitFilteredMu100SmallR30YCut15Jets",
                                      mumax=1.0, ymin=0.15, input="tmp_exot12_CamKt12LCTopoJets",
                                      modifiersin="groomed")
  
  jetalg_cam12_splitfiltered = JetAlgorithm("exot12_cam12_splitfilter",   Tools = [ cam12_lctopo_finder,
                                                                             cam12_splitfilter])
  DerivationFrameworkJob += jetalg_cam12_splitfiltered

# Add trimmed jet R20
DerivationFrameworkJob += addTrimmedJets("AntiKt", 1.0, "LCTopo", rclus=0.2, ptfrac=0.05)
DerivationFrameworkJob += addTrimmedJets("AntiKt", 1.0, "Truth", rclus=0.2, ptfrac=0.05)
#====================================================================
# THINNING TOOLS
#====================================================================

#Tracks associated with Electrons
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__EgammaTrackParticleThinning
EXOT12ElectronTPThinningTool = DerivationFramework__EgammaTrackParticleThinning( name                   = "EXOT12ElectronTPThinningTool",
                                                                                 ThinningService        = "EXOT12ThinningSvc",
                                                                                 SGKey                  = "Electrons",
                                                                                 InDetTrackParticlesKey = "InDetTrackParticles")
ToolSvc += EXOT12ElectronTPThinningTool

#Tracks associated with Muons
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__MuonTrackParticleThinning
EXOT12MuonTPThinningTool = DerivationFramework__MuonTrackParticleThinning( name                  = "EXOT12MuonTPThinningTool",
                                                                           ThinningService       = "EXOT12ThinningSvc",
                                                                           MuonKey               = "Muons",
                                                                           InDetTrackParticlesKey = "InDetTrackParticles")
ToolSvc += EXOT12MuonTPThinningTool

#Tracks associated with Jets
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__JetTrackParticleThinning
EXOT12AKt4JetTPThinningTool = DerivationFramework__JetTrackParticleThinning( name                    = "EXOT12AKt4JetTPThinningTool",
                                                                             ThinningService         = "EXOT12ThinningSvc",
                                                                             JetKey                  = "AntiKt4LCTopoJets",
                                                                             SelectionString         = "AntiKt4LCTopoJets.pt > 15*GeV && abs(AntiKt4LCTopoJets.eta) < 2.8",
                                                                             InDetTrackParticlesKey  = "InDetTrackParticles")
ToolSvc += EXOT12AKt4JetTPThinningTool

EXOT12AKt10JetTPThinningTool = DerivationFramework__JetTrackParticleThinning( name                    = "EXOT12AKt10JetTPThinningTool",
                                                                              ThinningService         = "EXOT12ThinningSvc",
                                                                              JetKey                  = "AntiKt10LCTopoJets",
                                                                              SelectionString         = "AntiKt10LCTopoJets.pt > 15*GeV && abs(AntiKt10LCTopoJets.eta) < 2.8",
                                                                              InDetTrackParticlesKey  = "InDetTrackParticles")
ToolSvc += EXOT12AKt10JetTPThinningTool

#Truth Thinning (!!! NOT WORKING FOR athena 19)
#from DerivationFrameworkMCTruth.DerivationFrameworkMCTruthConf import DerivationFramework__MenuTruthThinning
#EXOT12MenuThinningTool = DerivationFramework__MenuTruthThinning( name = "EXOT12MenuThinningTool",
#                                                                 ThinningService = "EXOT12ThinningSvc",
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

#ToolSvc += EXOT12MenuThinningTool

#from DerivationFrameworkMCTruth.DerivationFrameworkMCTruthConf import DerivationFramework__GenericTruthThinning
#EXOT12GenThinningTool = DerivationFramework__GenericTruthThinning( name = "EXOT12GenThinningTool",
#                                                                   ThinningService = "EXOT12ThinningSvc",
#                                                                   ParticleSelectionString = "abs(TruthParticle.pdgId) ==34 || abs(TruthParticle.pdgId)==5100039", 
#                                                                   PreserveDescendants = False)

#ToolSvc += EXOT12GenThinningTool

#=======================================
# SKIMMING TOOL   
#=======================================

#trig = " (HLT_e28_tight_iloose || HLT_e60_medium1)|| (HLT_mu26_imedium || HLT_mu50) "
selection_e = "(count((Electrons.Medium) && (Electrons.pt > 15*GeV)) > 1)"
selection_mu = "(count(Muons.pt > 15*GeV) > 1)"
#selection_j = "( (count(CamKt12LCTopoJets.pt > 200*GeV) > 0) || (count(AntiKt10LCTopoJets.pt > 200*GeV) > 0) || (count(AntiKt4LCTopoJets.pt > 200*GeV) > 0) )"

beamEnergy = jobproperties.Beam.energy()
expression = ''
if (beamEnergy < 4.1e+06):
    expression = "(" + selection_e + "||" + selection_mu + ")" 
if (beamEnergy > 6.0e+06):
    expression = "(" + selection_e + "||" + selection_mu + ")" 

from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__xAODStringSkimmingTool
EXOT12SkimmingTool = DerivationFramework__xAODStringSkimmingTool( name = "EXOT12SkimmingTool", 
                                                                  expression = expression)
ToolSvc += EXOT12SkimmingTool
print EXOT12SkimmingTool

#=======================================
# THE DERIVATION KERNEL ALGORITHM   
#=======================================
from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__DerivationKernel
DerivationFrameworkJob += CfgMgr.DerivationFramework__DerivationKernel("EXOT12Kernel",
                                                                       SkimmingTools = [EXOT12SkimmingTool],
                                                                       ThinningTools = [EXOT12ElectronTPThinningTool,
                                                                                        EXOT12MuonTPThinningTool,
                                                                                        EXOT12AKt4JetTPThinningTool,
                                                                                        EXOT12AKt10JetTPThinningTool])
#,EXOT12MenuThinningTool

#====================================================================
# SET UP STREAM   
#====================================================================
streamName   = derivationFlags.WriteDAOD_EXOT12Stream.StreamName
fileName     = buildFileName( derivationFlags.WriteDAOD_EXOT12Stream )
EXOT12Stream = MSMgr.NewPoolRootStream( streamName, fileName )
EXOT12Stream.AcceptAlgs(["EXOT12Kernel"])

# Thinning
from AthenaServices.Configurables import ThinningSvc, createThinningSvc
augStream = MSMgr.GetStream( streamName )
evtStream = augStream.GetEventStream()
svcMgr   += createThinningSvc( svcName="EXOT12ThinningSvc", outStreams=[evtStream] )

#====================================================================
# Add the containers to the output stream - slimming done here
#====================================================================
from DerivationFrameworkCore.SlimmingHelper import SlimmingHelper
from DerivationFrameworkExotics.EXOT12ContentList import *
EXOT12SlimmingHelper = SlimmingHelper("EXOT12SlimmingHelper")
EXOT12SlimmingHelper.SmartCollections = EXOT12SmartContent
EXOT12SlimmingHelper.AllVariables = EXOT12AllVariablesContent
EXOT12SlimmingHelper.StaticContent = EXOT12Content
EXOT12SlimmingHelper.AppendContentToStream(EXOT12Stream)

