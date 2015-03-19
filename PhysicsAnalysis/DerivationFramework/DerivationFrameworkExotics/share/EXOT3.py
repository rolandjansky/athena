#********************************************************************
# EXOT3.py 
# reductionConf flag EXOT3 in Reco_tf.py   
#********************************************************************

from DerivationFrameworkCore.DerivationFrameworkMaster import *
from DerivationFrameworkMuons.MuonsCommon import *
from DerivationFrameworkJetEtMiss.JetCommon import *
from DerivationFrameworkJetEtMiss.ExtendedJetCommon import *
from DerivationFrameworkEGamma.EGammaCommon import *
from DerivationFrameworkJetEtMiss.METCommon import *

from DerivationFrameworkExotics.JetDefinitions import *

from JetRec.JetRecStandard import jtm

#=======================================
# Augmentation: Jet Collections
#=======================================

# Custom Jet Collections
from JetRec.JetRecConf import JetAlgorithm
if not "CamKt08LCTopoJets" in jtm.tools:
  cam08_lctopo_finder = jtm.addJetFinder("CamKt08LCTopoJets", "CamKt", 0.8, "lctopo", "largeR_notruth",
                                         ghostArea=0.01 , ptmin=2000, ptminFilter=20000, calibOpt="none")
  jetalg_cam08_lctopo = JetAlgorithm("exot3_cam08_lctopo_finder", Tools = [ cam08_lctopo_finder] )
  DerivationFrameworkJob += jetalg_cam08_lctopo


if not "CamKt12LCTopoSplitFilteredMu100SmallR30YCut15Jets" in jtm.tools:
  #Temp jets to find split filtered jets
  cam12_lctopo_finder = jtm.addJetFinder("tmp_exot3_CamKt12LCTopoJets", "CamKt", 1.2, "lctopo", "largeR_notruth", 
                                         ghostArea=0.01, ptmin=2000, ptminFilter=50000, calibOpt="none")

  #Split filtered jets
  cam12_splitfilter = jtm.addJetSplitter("CamKt12LCTopoSplitFilteredMu100SmallR30YCut15Jets",
                                      mumax=1.0, ymin=0.15, input="tmp_exot3_CamKt12LCTopoJets", 
                                      modifiersin="groomed")

  jetalg_cam12_splitfiltered = JetAlgorithm("exot3_cam12_splitfilter", Tools=[
                                                                      cam12_lctopo_finder,
                                                                      cam12_splitfilter])

  DerivationFrameworkJob += jetalg_cam12_splitfiltered


if not "CamKt10LCTopoJets" in jtm.tools:
  cam10_lctopo_finder = jtm.addJetFinder("CamKt10LCTopoJets", "CamKt", 1.0, "lctopo", "largeR_notruth",
                                         ghostArea=0.01 , ptmin=2000, ptminFilter=30000, calibOpt="none")

  jetalg_cam10_lctopo = JetAlgorithm("exot3_cam10_lctopo_finder", Tools = [ cam10_lctopo_finder] )

  DerivationFrameworkJob += jetalg_cam10_lctopo

DerivationFrameworkJob += addTrimmedJets("AntiKt", 1.0, "LCTopo", rclus=0.2, ptfrac=0.05)
#====================================================================
# THINNING TOOLS
#====================================================================

#Tracks associated with Electrons
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__EgammaTrackParticleThinning
EXOT3ElectronTPThinningTool = DerivationFramework__EgammaTrackParticleThinning( name            = "EXOT3ElectronTPThinningTool",
                                                                                ThinningService = "EXOT3ThinningSvc",
                                                                                SGKey           = "Electrons",
                                                                                InDetTrackParticlesKey = "InDetTrackParticles")
ToolSvc += EXOT3ElectronTPThinningTool

#Tracks associated with Muons
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__MuonTrackParticleThinning
EXOT3MuonTPThinningTool = DerivationFramework__MuonTrackParticleThinning( name                  = "EXOT3MuonTPThinningTool",
                                                                          ThinningService       = "EXOT3ThinningSvc",
                                                                          MuonKey               = "Muons",
                                                                          InDetTrackParticlesKey = "InDetTrackParticles")
ToolSvc += EXOT3MuonTPThinningTool

#Tracks associated with Jets
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__JetTrackParticleThinning
EXOT3AKt4JetTPThinningTool = DerivationFramework__JetTrackParticleThinning( name                = "EXOT3AKt4JetTPThinningTool",
                                                                        ThinningService         = "EXOT3ThinningSvc",
                                                                        JetKey                  = "AntiKt4LCTopoJets",
                                                                        SelectionString         = "AntiKt4LCTopoJets.pt > 15*GeV && abs(AntiKt4LCTopoJets.eta) < 2.8",
                                                                        InDetTrackParticlesKey  = "InDetTrackParticles")
ToolSvc += EXOT3AKt4JetTPThinningTool

EXOT3AKt10JetTPThinningTool = DerivationFramework__JetTrackParticleThinning( name               = "EXOT3AKt10JetTPThinningTool",
                                                                        ThinningService         = "EXOT3ThinningSvc",
                                                                        JetKey                  = "AntiKt10LCTopoJets",
                                                                        SelectionString         = "AntiKt10LCTopoJets.pt > 200*GeV && abs(AntiKt10LCTopoJets.eta) < 2.8",
                                                                        InDetTrackParticlesKey  = "InDetTrackParticles")
ToolSvc += EXOT3AKt10JetTPThinningTool


#=======================================
# SKIMMING   
#=======================================

trigger_selection =  "(HLT_j460_a10_L1J100 || HLT_j400 || HLT_j200_ht1000 || HLT_j330)"
topology_selection = "(count (abs(AntiKt10LCTopoJets.eta) < 2.8 && AntiKt10LCTopoJets.pt > 200)  > 1)"
lepton_veto = "( (count (Electrons.Tight && Electrons.pt > 20*GeV) == 0) && (count (Muons.DFCommonGoodMuon && Muons.pt > 25*GeV) == 0) )" 
#met_veto = "(count((MET_RefFinal.mpx * MET_RefFinal.mpx + MET_RefFinal.mpy * MET_RefFinal.mpy) > 2500*GeV*GeV) == 0)"

beamEnergy = jobproperties.Beam.energy()
expression = ''
if (beamEnergy < 4.1e+06):
    expression = "(EventInfo.eventTypeBitmask==1) || EF_j360_at10tcem" + "||" + trigger_selection
if (beamEnergy > 6.0e+06):
    expression = trigger_selection + "&&" + topology_selection + "&&" + lepton_veto # + "&&" + met_veto

from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__xAODStringSkimmingTool
EXOT3StringSkimmingTool = DerivationFramework__xAODStringSkimmingTool(name = "EXOT3StringSkimmingTool",
                                                                      expression = expression)
 
ToolSvc += EXOT3StringSkimmingTool

#=======================================
# CREATE THE DERIVATION KERNEL ALGORITHM   
#=======================================

from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__DerivationKernel
DerivationFrameworkJob += CfgMgr.DerivationFramework__DerivationKernel(	"EXOT3Kernel",
									SkimmingTools = [EXOT3StringSkimmingTool],
									ThinningTools = [EXOT3ElectronTPThinningTool,
                                                     EXOT3MuonTPThinningTool,
                                                     EXOT3AKt4JetTPThinningTool, 
                                                     EXOT3AKt10JetTPThinningTool])

#====================================================================
# SET UP STREAM   
#====================================================================
streamName = derivationFlags.WriteDAOD_EXOT3Stream.StreamName
fileName   = buildFileName( derivationFlags.WriteDAOD_EXOT3Stream )
EXOT3Stream = MSMgr.NewPoolRootStream( streamName, fileName )
EXOT3Stream.AcceptAlgs(["EXOT3Kernel"])
# Thinning
from AthenaServices.Configurables import ThinningSvc, createThinningSvc
augStream = MSMgr.GetStream( streamName )
evtStream = augStream.GetEventStream()
svcMgr += createThinningSvc( svcName="EXOT3ThinningSvc", outStreams=[evtStream] )

#====================================================================
# Add the containers to the output stream - slimming done here
#====================================================================
from DerivationFrameworkCore.SlimmingHelper import SlimmingHelper
from DerivationFrameworkExotics.EXOT3ContentList import EXOT3SmartContent, EXOT3AllVariablesContent, EXOT3UnslimmedContent

EXOT3SlimmingHelper = SlimmingHelper("EXOT3SlimmingHelper")

# Containers to be smart slimmed, see https://svnweb.cern.ch/trac/atlasoff/browser/PhysicsAnalysis
# /DerivationFramework/DerivationFrameworkExamples/trunk/share/SlimmingExample.py#L38
EXOT3SlimmingHelper.SmartCollections = EXOT3SmartContent

# Keep all variables for containers which we don't want to smart slim and were
# not created in the derivation
EXOT3SlimmingHelper.AllVariables = EXOT3AllVariablesContent

# Add jet collections created by derivation job
EXOT3SlimmingHelper.UserContent = EXOT3UnslimmedContent


EXOT3SlimmingHelper.AppendContentToStream(EXOT3Stream)
