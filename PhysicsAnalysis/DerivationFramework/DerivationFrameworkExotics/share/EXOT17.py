#********************************************************************
# EXOT17.py 
# reductionConf flag EXOT17 in Reco_tf.py   
#********************************************************************
from DerivationFrameworkCore.DerivationFrameworkMaster import *
from DerivationFrameworkJetEtMiss.JetCommon import *
from DerivationFrameworkJetEtMiss.METCommon import *
from DerivationFrameworkEGamma.EGammaCommon import *
from DerivationFrameworkEGamma.ElectronsCPDetailedContent import *
from DerivationFrameworkMuons.MuonsCommon import *
from DerivationFrameworkCore.WeightMetadata import *

from AthenaCommon.GlobalFlags import globalflags
isMC = False
if globalflags.DataSource()=='geant4':
  isMC = True

exot17Seq = CfgMgr.AthSequencer("EXOT17Sequence")

#====================================================================
# SET UP STREAM   
#====================================================================
streamName = derivationFlags.WriteDAOD_EXOT17Stream.StreamName
fileName   = buildFileName( derivationFlags.WriteDAOD_EXOT17Stream )
EXOT17Stream = MSMgr.NewPoolRootStream( streamName, fileName )
EXOT17Stream.AcceptAlgs(["EXOT17Kernel"])

#====================================================================
# THINNING TOOLS
#====================================================================

#thinning helper
from DerivationFrameworkCore.ThinningHelper import ThinningHelper
EXOT17ThinningHelper = ThinningHelper( "EXOT17ThinningHelper" )
EXOT17ThinningHelper.TriggerChains = '^(?!.*_[0-9]*(e|j|xe|tau|ht|xs|te))(?!HLT_mu.*_[0-9]*mu.*)HLT_mu.*'
EXOT17ThinningHelper.AppendToStream( EXOT17Stream )

thinningTools = []

# Tracks associated with Muons
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__MuonTrackParticleThinning
EXOT17MuonTPThinningTool = DerivationFramework__MuonTrackParticleThinning(name                    = "EXOT17MuonTPThinningTool",
                                                                          ThinningService         = EXOT17ThinningHelper.ThinningSvc(),
                                                                          MuonKey                 = "Muons",
                                                                          InDetTrackParticlesKey  = "InDetTrackParticles",
                                                                          ConeSize                =  0.05)
ToolSvc += EXOT17MuonTPThinningTool
thinningTools.append(EXOT17MuonTPThinningTool)

# Tracks associated with Electrons
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__EgammaTrackParticleThinning
EXOT17ElectronTPThinningTool = DerivationFramework__EgammaTrackParticleThinning(name                    = "EXOT17ElectronTPThinningTool",
                                                                                ThinningService         = EXOT17ThinningHelper.ThinningSvc(),
                                                                                SGKey                   = "Electrons",
                                                                                InDetTrackParticlesKey  = "InDetTrackParticles",
                                                                                ConeSize                =  0.0)
ToolSvc += EXOT17ElectronTPThinningTool
thinningTools.append(EXOT17ElectronTPThinningTool)

# truth thinning
from DerivationFrameworkMCTruth.DerivationFrameworkMCTruthConf import DerivationFramework__MenuTruthThinning
EXOT17TruthTool = DerivationFramework__MenuTruthThinning(name                  = "EXOT17TruthTool",
                                                         ThinningService       = EXOT17ThinningHelper.ThinningSvc(),
                                                         WritePartons          = False,
                                                         WriteHadrons          = False,
                                                         WriteBHadrons         = False,
                                                         WriteGeant            = False,
                                                         GeantPhotonPtThresh   = -1.0,
                                                         WriteTauHad           = False,
                                                         PartonPtThresh        = -1.0,
                                                         WriteBSM              = True,
                                                         WriteBosons           = True,
                                                         WriteBSMProducts      = True,
                                                         WriteBosonProducts    = True,
                                                         WriteTopAndDecays     = True,
                                                         WriteEverything       = False,
                                                         WriteAllLeptons       = False,
                                                         WriteStatus3          = False,
                                                         PreserveGeneratorDescendants  = False,
                                                         PreserveAncestors     = True,
                                                         WriteFirstN           = -1)
if isMC:
  ToolSvc += EXOT17TruthTool
  thinningTools.append(EXOT17TruthTool)

truth_cond_Lepton = "((abs(TruthParticles.pdgId) >= 11) && (abs(TruthParticles.pdgId) <= 16) && (TruthParticles.pt > 1*GeV) && ((TruthParticles.status ==1) || (TruthParticles.status ==2) || (TruthParticles.status ==3) || (TruthParticles.status ==23)) && (TruthParticles.barcode<200000))"

from DerivationFrameworkMCTruth.DerivationFrameworkMCTruthConf import DerivationFramework__GenericTruthThinning
EXOT17TruthTool2 = DerivationFramework__GenericTruthThinning(name                         = "EXOT17TruthTool2",
                                                             ThinningService              = EXOT17ThinningHelper.ThinningSvc(),
                                                             ParticleSelectionString      = truth_cond_Lepton,
                                                             PreserveDescendants          = False,
                                                             PreserveGeneratorDescendants = True,
                                                             PreserveAncestors            = True)
if isMC:
  ToolSvc += EXOT17TruthTool2
  thinningTools.append(EXOT17TruthTool2)

#=======================================
# CREATE THE SKIMMING TOOL   
#=======================================

expression = '(count(Muons.pt > 50*GeV && (Muons.DFCommonGoodMuon && Muons.muonType == 0)) >= 1)'

from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__xAODStringSkimmingTool
EXOT17SkimmingTool = DerivationFramework__xAODStringSkimmingTool(name = "EXOT17SkimmingTool1", expression = expression)
ToolSvc += EXOT17SkimmingTool
print EXOT17SkimmingTool

#=======================================
# CREATE THE DERIVATION KERNEL ALGORITHM   
#=======================================

from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__DerivationKernel
DerivationFrameworkJob += exot17Seq
exot17Seq += CfgMgr.DerivationFramework__DerivationKernel("EXOT17Kernel_skim", SkimmingTools = [EXOT17SkimmingTool])
exot17Seq += CfgMgr.DerivationFramework__DerivationKernel("EXOT17Kernel", ThinningTools = thinningTools)

#=======================================
# JETS
#=======================================

#restore AOD-reduced jet collections
from DerivationFrameworkJetEtMiss.ExtendedJetCommon import replaceAODReducedJets
OutputJets["EXOT17"] = []
reducedJetList = [
  "AntiKt4TruthWZJets" , "AntiKt4TruthJets"]
replaceAODReducedJets(reducedJetList,exot17Seq,"EXOT17")

#Adding Btagging for PFlowJets
from DerivationFrameworkFlavourTag.FlavourTagCommon import FlavorTagInit
FlavorTagInit(JetCollections  = ['AntiKt4EMPFlowJets'],Sequencer = exot17Seq)

#====================================================================
# Add the containers to the output stream - slimming done here
#====================================================================
from DerivationFrameworkCore.SlimmingHelper import SlimmingHelper
from DerivationFrameworkExotics.EXOT9ContentList import *
EXOT17SlimmingHelper = SlimmingHelper("EXOT17SlimmingHelper")
EXOT17SlimmingHelper.StaticContent = EXOT9Content
EXOT17SlimmingHelper.AllVariables = EXOT9AllVariables
EXOT17SlimmingHelper.SmartCollections = EXOT9SmartCollections
EXOT17SlimmingHelper.ExtraVariables += EXOT9Extravariables
EXOT17SlimmingHelper.ExtraVariables += ElectronsCPDetailedContent
EXOT17SlimmingHelper.IncludeEGammaTriggerContent = False
EXOT17SlimmingHelper.IncludeMuonTriggerContent = True
addMETOutputs(EXOT17SlimmingHelper, ["EXOT17", "Track"], ["AntiKt4EMTopo"])
EXOT17SlimmingHelper.AppendContentToStream(EXOT17Stream)
