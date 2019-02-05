#********************************************************************
# EXOT9.py 
# reductionConf flag EXOT9 in Reco_tf.py   
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

exot9Seq = CfgMgr.AthSequencer("EXOT9Sequence")

#====================================================================
# SET UP STREAM   
#====================================================================
streamName = derivationFlags.WriteDAOD_EXOT9Stream.StreamName
fileName   = buildFileName( derivationFlags.WriteDAOD_EXOT9Stream )
EXOT9Stream = MSMgr.NewPoolRootStream( streamName, fileName )
EXOT9Stream.AcceptAlgs(["EXOT9Kernel"])

#====================================================================
# THINNING TOOLS
#====================================================================

#thinning helper
from DerivationFrameworkCore.ThinningHelper import ThinningHelper
EXOT9ThinningHelper = ThinningHelper( "EXOT9ThinningHelper" )
EXOT9ThinningHelper.TriggerChains = '^(?!.*_[0-9]*(mu|j|xe|tau|ht|xs|te))(?!HLT_e.*_[0-9]*e.*)HLT_e.*'
EXOT9ThinningHelper.AppendToStream( EXOT9Stream )

thinningTools = []

# Tracks associated with Muons
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__MuonTrackParticleThinning
EXOT9MuonTPThinningTool = DerivationFramework__MuonTrackParticleThinning(name                    = "EXOT9MuonTPThinningTool",
                                                                         ThinningService         = EXOT9ThinningHelper.ThinningSvc(),
                                                                         MuonKey                 = "Muons",
                                                                         InDetTrackParticlesKey  = "InDetTrackParticles",
                                                                         ConeSize                =  0.0)
ToolSvc += EXOT9MuonTPThinningTool
thinningTools.append(EXOT9MuonTPThinningTool)

# Tracks associated with Electrons
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__EgammaTrackParticleThinning
EXOT9ElectronTPThinningTool = DerivationFramework__EgammaTrackParticleThinning(name                    = "EXOT9ElectronTPThinningTool",
                                                                               ThinningService         = EXOT9ThinningHelper.ThinningSvc(),
                                                                               SGKey                   = "Electrons",
                                                                               InDetTrackParticlesKey  = "InDetTrackParticles",
                                                                               ConeSize                =  0.0)
ToolSvc += EXOT9ElectronTPThinningTool
thinningTools.append(EXOT9ElectronTPThinningTool)

# Tracks associated with Photons
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__EgammaTrackParticleThinning
EXOT9PhotonTPThinningTool = DerivationFramework__EgammaTrackParticleThinning(name                    = "EXOT9PhotonTPThinningTool",
                                                                             ThinningService         = EXOT9ThinningHelper.ThinningSvc(),
                                                                             SGKey                   = "Photons",
                                                                             InDetTrackParticlesKey  = "InDetTrackParticles",
                                                                             ConeSize                =  0.0)
ToolSvc += EXOT9PhotonTPThinningTool
thinningTools.append(EXOT9PhotonTPThinningTool)

# truth thinning
from DerivationFrameworkMCTruth.DerivationFrameworkMCTruthConf import DerivationFramework__MenuTruthThinning
EXOT9TruthTool = DerivationFramework__MenuTruthThinning(name                  = "EXOT9TruthTool",
                                                        ThinningService       = EXOT9ThinningHelper.ThinningSvc(),
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
  ToolSvc += EXOT9TruthTool
  thinningTools.append(EXOT9TruthTool)

truth_cond_Lepton = "((abs(TruthParticles.pdgId) >= 11) && (abs(TruthParticles.pdgId) <= 16) && (TruthParticles.pt > 1*GeV) && ((TruthParticles.status ==1) || (TruthParticles.status ==2) || (TruthParticles.status ==3) || (TruthParticles.status ==23)) && (TruthParticles.barcode<200000))"

from DerivationFrameworkMCTruth.DerivationFrameworkMCTruthConf import DerivationFramework__GenericTruthThinning
EXOT9TruthTool2 = DerivationFramework__GenericTruthThinning(name                         = "EXOT9TruthTool2",
                                                            ThinningService              = EXOT9ThinningHelper.ThinningSvc(),
                                                            ParticleSelectionString      = truth_cond_Lepton,
                                                            PreserveDescendants          = False,
                                                            PreserveGeneratorDescendants = True,
                                                            PreserveAncestors            = True)
if isMC:
  ToolSvc += EXOT9TruthTool2
  thinningTools.append(EXOT9TruthTool2)

#=======================================
# CREATE THE SKIMMING TOOL   
#=======================================
from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__xAODStringSkimmingTool
triggerStrategy = '(count(Electrons.pt > 60*GeV && (Electrons.DFCommonElectronsLHLoose||Electrons.DFCommonElectronsLHMedium||Electrons.DFCommonElectronsLHTight||Electrons.DFCommonElectronsLHLooseBL)) >= 1)'
EXOT9SkimmingTool = DerivationFramework__xAODStringSkimmingTool(name = "EXOT9SkimmingTool1", expression = triggerStrategy)
ToolSvc += EXOT9SkimmingTool
print EXOT9SkimmingTool

#=======================================
# CREATE THE DERIVATION KERNEL ALGORITHM   
#=======================================

from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__DerivationKernel
DerivationFrameworkJob += exot9Seq
exot9Seq += CfgMgr.DerivationFramework__DerivationKernel("EXOT9Kernel_skim", SkimmingTools = [EXOT9SkimmingTool])
exot9Seq += CfgMgr.DerivationFramework__DerivationKernel("EXOT9Kernel", ThinningTools = thinningTools)

#=======================================
# JETS
#=======================================

#restore AOD-reduced jet collections
from DerivationFrameworkJetEtMiss.ExtendedJetCommon import replaceAODReducedJets
OutputJets["EXOT9"] = []
reducedJetList = [
  "AntiKt4TruthWZJets" , "AntiKt4TruthJets"]
replaceAODReducedJets(reducedJetList,exot9Seq,"EXOT9")

#====================================================================
# Add the containers to the output stream - slimming done here
#====================================================================
from DerivationFrameworkCore.SlimmingHelper import SlimmingHelper
from DerivationFrameworkExotics.EXOT9ContentList import *
EXOT9SlimmingHelper = SlimmingHelper("EXOT9SlimmingHelper")
EXOT9SlimmingHelper.StaticContent = EXOT9Content
EXOT9SlimmingHelper.AllVariables = EXOT9AllVariables
EXOT9SlimmingHelper.SmartCollections = EXOT9SmartCollections
EXOT9SlimmingHelper.ExtraVariables += EXOT9Extravariables
EXOT9SlimmingHelper.ExtraVariables += ElectronsCPDetailedContent
EXOT9SlimmingHelper.IncludeEGammaTriggerContent = True
EXOT9SlimmingHelper.IncludeMuonTriggerContent = False
addMETOutputs(EXOT9SlimmingHelper, ["EXOT9", "Track","AntiKt4EMTopo","AntiKt4EMPFlow"], ["AntiKt4EMTopo","AntiKt4EMPFlow"])
EXOT9SlimmingHelper.AppendContentToStream(EXOT9Stream)
