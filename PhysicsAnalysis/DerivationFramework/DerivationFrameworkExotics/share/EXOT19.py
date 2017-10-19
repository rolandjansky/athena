#********************************************************************
# EXOT19.py 
# reductionConf flag EXOT19 in Reco_tf.py   
#********************************************************************
from DerivationFrameworkCore.DerivationFrameworkMaster import *
from DerivationFrameworkInDet.InDetCommon import *
from DerivationFrameworkJetEtMiss.JetCommon import *
from DerivationFrameworkJetEtMiss.METCommon import *
from DerivationFrameworkEGamma.EGammaCommon import *
from DerivationFrameworkMuons.MuonsCommon import *
from DerivationFrameworkCore.WeightMetadata import *

exot19Seq = CfgMgr.AthSequencer("EXOT19Sequence")

#====================================================================
# THINNING TOOLS
#====================================================================

thinningTools = []

# Tracks associated with Muons
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__MuonTrackParticleThinning
EXOT19MuonTPThinningTool = DerivationFramework__MuonTrackParticleThinning(name                    = "EXOT19MuonTPThinningTool",
                                                                          ThinningService         = "EXOT19ThinningSvc",
                                                                          MuonKey                 = "Muons",
                                                                          InDetTrackParticlesKey  = "InDetTrackParticles",
                                                                          ConeSize                =  0) # change wrt. EXOT0 that uses 0.4
ToolSvc += EXOT19MuonTPThinningTool
thinningTools.append(EXOT19MuonTPThinningTool)

# Tracks associated with Electrons
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__EgammaTrackParticleThinning
EXOT19ElectronTPThinningTool = DerivationFramework__EgammaTrackParticleThinning(name                    = "EXOT19ElectronTPThinningTool",
                                                                                ThinningService         = "EXOT19ThinningSvc",
                                                                                SGKey                   = "Electrons",
                                                                                InDetTrackParticlesKey  = "InDetTrackParticles",
                                                                                ConeSize                =  0) # change wrt. EXOT0 that uses 0.4
ToolSvc += EXOT19ElectronTPThinningTool
thinningTools.append(EXOT19ElectronTPThinningTool)

# Tracks associated with Photons
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__EgammaTrackParticleThinning
EXOT19PhotonTPThinningTool = DerivationFramework__EgammaTrackParticleThinning(name                    = "EXOT19PhotonTPThinningTool",
                                                                              ThinningService         = "EXOT19ThinningSvc",
                                                                              SGKey                   = "Photons",
                                                                              InDetTrackParticlesKey  = "InDetTrackParticles",
                                                                              ConeSize                =  0) # change wrt. EXOT0 that uses 0.4
ToolSvc += EXOT19PhotonTPThinningTool
thinningTools.append(EXOT19PhotonTPThinningTool)

# truth thinning
from DerivationFrameworkMCTruth.DerivationFrameworkMCTruthConf import DerivationFramework__MenuTruthThinning
EXOT19TruthTool = DerivationFramework__MenuTruthThinning(name                  = "EXOT19TruthTool",
                                                        ThinningService       = "EXOT19ThinningSvc",
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

from AthenaCommon.GlobalFlags import globalflags
if globalflags.DataSource()=='geant4':
  ToolSvc += EXOT19TruthTool
  thinningTools.append(EXOT19TruthTool)

truth_cond_Lepton = "((abs(TruthParticles.pdgId) >= 11) && (abs(TruthParticles.pdgId) <= 16) && (TruthParticles.pt > 1*GeV) && ((TruthParticles.status ==1) || (TruthParticles.status ==2) || (TruthParticles.status ==3) || (TruthParticles.status ==23)) && (TruthParticles.barcode<200000))"

from DerivationFrameworkMCTruth.DerivationFrameworkMCTruthConf import DerivationFramework__GenericTruthThinning
EXOT19TruthTool2 = DerivationFramework__GenericTruthThinning(name                         = "EXOT19TruthTool2",
                                                            ThinningService              = "EXOT19ThinningSvc",
                                                            ParticleSelectionString      = truth_cond_Lepton,
                                                            PreserveDescendants          = False,
                                                            PreserveGeneratorDescendants = True,
                                                            PreserveAncestors            = True)
if globalflags.DataSource()=='geant4':
  ToolSvc += EXOT19TruthTool2
  thinningTools.append(EXOT19TruthTool2)

#====================================================================
# SKIMMING TOOL 
#====================================================================
from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__xAODStringSkimmingTool
expression = '(count(Electrons.pt > 20*GeV && Electrons.DFCommonElectronsLHLoose) >= 1)'
EXOT19SkimmingTool = DerivationFramework__xAODStringSkimmingTool(name = "EXOT19SkimmingTool1", expression = expression)
ToolSvc += EXOT19SkimmingTool

#=======================================
# JETS
#=======================================

#restore AOD-reduced jet collections
from DerivationFrameworkJetEtMiss.ExtendedJetCommon import replaceAODReducedJets
OutputJets["EXOT19"] = []
reducedJetList = [
  "AntiKt4TruthJets",
  "AntiKt4TruthWZJets"]
replaceAODReducedJets(reducedJetList,exot19Seq,"EXOT19")

#=======================================
# CREATE THE DERIVATION KERNEL ALGORITHM   
#=======================================

from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__DerivationKernel
DerivationFrameworkJob += exot19Seq
exot19Seq += CfgMgr.DerivationFramework__DerivationKernel("EXOT19Kernel_skim", SkimmingTools = [EXOT19SkimmingTool])
exot19Seq += CfgMgr.DerivationFramework__DerivationKernel("EXOT19Kernel", ThinningTools = thinningTools)

#====================================================================
# SET UP STREAM   
#====================================================================
streamName = derivationFlags.WriteDAOD_EXOT19Stream.StreamName
fileName   = buildFileName( derivationFlags.WriteDAOD_EXOT19Stream )
EXOT19Stream = MSMgr.NewPoolRootStream( streamName, fileName )
EXOT19Stream.AcceptAlgs(["EXOT19Kernel"])

#=====================
# TRIGGER NAV THINNING
#=====================
#Establish the thinning helper
from DerivationFrameworkCore.ThinningHelper import ThinningHelper
EXOT19ThinningHelper = ThinningHelper("EXOT19ThinningHelper")
#trigger navigation content
EXOT19ThinningHelper.TriggerChains = 'HLT_e.*|HLT_2e.*'
EXOT19ThinningHelper.AppendToStream( EXOT19Stream )

#====================================================================
# Add the containers to the output stream - slimming done here
#====================================================================
from DerivationFrameworkCore.SlimmingHelper import SlimmingHelper
from DerivationFrameworkExotics.EXOT19ContentList import *
EXOT19SlimmingHelper = SlimmingHelper("EXOT19SlimmingHelper")
EXOT19SlimmingHelper.StaticContent = EXOT19Content
EXOT19SlimmingHelper.AllVariables = EXOT19AllVariables
EXOT19SlimmingHelper.ExtraVariables = EXOT19ExtraVariables
EXOT19SlimmingHelper.SmartCollections = EXOT19SmartCollections
if globalflags.DataSource()=='geant4':
  EXOT19SlimmingHelper.AllVariables += EXOT19AllVariablesTruth
  EXOT19SlimmingHelper.ExtraVariables += EXOT19ExtraVariablesTruth

EXOT19SlimmingHelper.IncludeEGammaTriggerContent = True
EXOT19SlimmingHelper.AppendContentToStream(EXOT19Stream)
