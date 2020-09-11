#********************************************************************
# EXOT22.py
# reductionConf flag EXOT22 in Reco_tf.py
#********************************************************************
from DerivationFrameworkCore.DerivationFrameworkMaster import *
from DerivationFrameworkInDet.InDetCommon import *
from DerivationFrameworkJetEtMiss.ExtendedJetCommon import *
from DerivationFrameworkJetEtMiss.METCommon import *
from DerivationFrameworkEGamma.EGammaCommon import *
from DerivationFrameworkMuons.MuonsCommon import *
from DerivationFrameworkTau.TauCommon import *
from DerivationFrameworkCore.WeightMetadata import *

if DerivationFrameworkHasTruth:
  from DerivationFrameworkMCTruth.MCTruthCommon import addStandardTruthContents
  addStandardTruthContents()

EXOT22Seq = CfgMgr.AthSequencer("EXOT22Sequence")
DerivationFrameworkJob += EXOT22Seq

#====================================================================
# SET UP STREAM
#====================================================================
streamName = derivationFlags.WriteDAOD_EXOT22Stream.StreamName
fileName   = buildFileName( derivationFlags.WriteDAOD_EXOT22Stream )
EXOT22Stream = MSMgr.NewPoolRootStream( streamName, fileName )
EXOT22Stream.AcceptAlgs(["EXOT22Kernel"])

#=====================
# TRIGGER NAV THINNING
#=====================
#Establish the thinning helper
from DerivationFrameworkCore.ThinningHelper import ThinningHelper
EXOT22ThinningHelper = ThinningHelper("EXOT22ThinningHelper")
#trigger navigation content
EXOT22ThinningHelper.TriggerChains = 'HLT_mu.*'
EXOT22ThinningHelper.AppendToStream( EXOT22Stream )

#====================================================================
# THINNING TOOLS
#====================================================================

thinningTools = []
augmentationTools = []

# Tracks associated with Muons
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__MuonTrackParticleThinning
EXOT22MuonTPThinningTool = DerivationFramework__MuonTrackParticleThinning(name                    = "EXOT22MuonTPThinningTool",
                                                                          ThinningService         = EXOT22ThinningHelper.ThinningSvc(),
                                                                          MuonKey                 = "Muons",
                                                                          InDetTrackParticlesKey  = "InDetTrackParticles",
                                                                          ConeSize                =  0) # change wrt. EXOT0 that uses 0.4
ToolSvc += EXOT22MuonTPThinningTool
thinningTools.append(EXOT22MuonTPThinningTool)

# Tracks associated with Electrons
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__EgammaTrackParticleThinning
EXOT22ElectronTPThinningTool = DerivationFramework__EgammaTrackParticleThinning(name                    = "EXOT22ElectronTPThinningTool",
                                                                                ThinningService         = EXOT22ThinningHelper.ThinningSvc(),
                                                                                SGKey                   = "Electrons",
                                                                                InDetTrackParticlesKey  = "InDetTrackParticles",
                                                                                ConeSize                =  0) # change wrt. EXOT0 that uses 0.4
ToolSvc += EXOT22ElectronTPThinningTool
thinningTools.append(EXOT22ElectronTPThinningTool)

# Tracks associated with Photons
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__EgammaTrackParticleThinning
EXOT22PhotonTPThinningTool = DerivationFramework__EgammaTrackParticleThinning(name                    = "EXOT22PhotonTPThinningTool",
                                                                              ThinningService         = EXOT22ThinningHelper.ThinningSvc(),
                                                                              SGKey                   = "Photons",
                                                                              InDetTrackParticlesKey  = "InDetTrackParticles",
                                                                              ConeSize                =  0) # change wrt. EXOT0 that uses 0.4
ToolSvc += EXOT22PhotonTPThinningTool
thinningTools.append(EXOT22PhotonTPThinningTool)

# TrackParticles associated with taus
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__TauTrackParticleThinning
EXOT22TauTPThinningTool = DerivationFramework__TauTrackParticleThinning(name                    = "EXOT22TauTPThinningTool",
                                                                        ThinningService         = EXOT22ThinningHelper.ThinningSvc(),
                                                                        TauKey                  = "TauJets",
                                                                        InDetTrackParticlesKey  = "InDetTrackParticles",
                                                                        ConeSize                =  0) # change wrt. EXOT0 that uses 0.4
ToolSvc += EXOT22TauTPThinningTool
thinningTools.append(EXOT22TauTPThinningTool)

# truth thinning
from DerivationFrameworkMCTruth.DerivationFrameworkMCTruthConf import DerivationFramework__MenuTruthThinning
EXOT22TruthTool = DerivationFramework__MenuTruthThinning(name                  = "EXOT22TruthTool",
                                                         ThinningService       = EXOT22ThinningHelper.ThinningSvc(),
                                                         WritePartons          = False,
                                                         WriteHadrons          = False,
                                                         WriteBHadrons         = False,
                                                         WriteGeant            = False,
                                                         GeantPhotonPtThresh   = -1.0,
                                                         WriteTauHad           = True,
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
                                                         WriteFirstN           = -1,
                                                         SimBarcodeOffset      = DerivationFrameworkSimBarcodeOffset)

if DerivationFrameworkHasTruth:
  # Re-run MCTruthClassifier
  from MCTruthClassifier.MCTruthClassifierConf import MCTruthClassifier
  EXOT22TruthClassifier = MCTruthClassifier(name                      = "EXOT22TruthClassifier",
                                            ParticleCaloExtensionTool = "")
  ToolSvc += EXOT22TruthClassifier

  from DerivationFrameworkMCTruth.DerivationFrameworkMCTruthConf import DerivationFramework__TruthClassificationDecorator
  EXOT22ClassificationDecorator = DerivationFramework__TruthClassificationDecorator(name              = "EXOT22ClassificationDecorator",
                                                                                    ParticlesKey      = "TruthParticles",
                                                                                    MCTruthClassifier = EXOT22TruthClassifier)
  ToolSvc += EXOT22ClassificationDecorator
  augmentationTools.append(EXOT22ClassificationDecorator)

  ToolSvc += EXOT22TruthTool
  thinningTools.append(EXOT22TruthTool)

  # tau truth
  from DerivationFrameworkTau.TauTruthCommon import scheduleTauTruthTools
  scheduleTauTruthTools()

truth_cond = "((abs(TruthParticles.pdgId) >= 11) && (abs(TruthParticles.pdgId) <= 16) && (TruthParticles.pt > 1*GeV) && ((TruthParticles.status ==1) || (TruthParticles.status ==2) || (TruthParticles.status ==3) || (TruthParticles.status ==23)) && (TruthParticles.barcode<200000))" # lepton conditions

from DerivationFrameworkMCTruth.DerivationFrameworkMCTruthConf import DerivationFramework__GenericTruthThinning
EXOT22TruthTool2 = DerivationFramework__GenericTruthThinning(name                         = "EXOT22TruthTool2",
                                                             ThinningService              = EXOT22ThinningHelper.ThinningSvc(),
                                                             ParticleSelectionString      = truth_cond,
                                                             PreserveDescendants          = False,
                                                             PreserveGeneratorDescendants = True,
                                                             PreserveAncestors            = True)
if DerivationFrameworkHasTruth:
  ToolSvc += EXOT22TruthTool2
  thinningTools.append(EXOT22TruthTool2)

#====================================================================
# SKIMMING TOOL
#====================================================================

from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__xAODStringSkimmingTool
mu_exp = '(Muons.pt > 20*GeV && Muons.DFCommonGoodMuon && Muons.muonType == 0)' # pass ID cuts and be a combined muon
expression = '(count(' + mu_exp + ') >= 1)'
EXOT22SkimmingTool = DerivationFramework__xAODStringSkimmingTool(name = "EXOT22SkimmingTool1", expression = expression)
ToolSvc += EXOT22SkimmingTool

#=======================================
# CREATE THE DERIVATION KERNEL ALGORITHM
#=======================================

from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__DerivationKernel
EXOT22Seq += CfgMgr.DerivationFramework__DerivationKernel("EXOT22Kernel_skim", SkimmingTools = [EXOT22SkimmingTool])
EXOT22Seq += CfgMgr.DerivationFramework__DerivationKernel("EXOT22Kernel",
                                                          ThinningTools = thinningTools,
                                                          AugmentationTools = augmentationTools)

#=======================================
# JETS
#=======================================
OutputJets["EXOT22"] = []

# Adding Btagging for PFlowJets
from DerivationFrameworkFlavourTag.FlavourTagCommon import FlavorTagInit
FlavorTagInit(JetCollections = ['AntiKt4EMPFlowJets'], Sequencer = EXOT22Seq)


#====================================================================
# Add the containers to the output stream - slimming done here
#====================================================================
from DerivationFrameworkCore.SlimmingHelper import SlimmingHelper
from DerivationFrameworkExotics.EXOT22ContentList import *
EXOT22SlimmingHelper = SlimmingHelper("EXOT22SlimmingHelper")
EXOT22SlimmingHelper.StaticContent = EXOT22Content
EXOT22SlimmingHelper.AllVariables = EXOT22AllVariables
EXOT22SlimmingHelper.ExtraVariables = EXOT22ExtraVariables
EXOT22SlimmingHelper.SmartCollections = EXOT22SmartCollections
if DerivationFrameworkHasTruth:
  EXOT22SlimmingHelper.AllVariables += EXOT22AllVariablesTruth
  EXOT22SlimmingHelper.ExtraVariables += EXOT22ExtraVariablesTruth
  EXOT22SlimmingHelper.SmartCollections += EXOT22SmartCollectionsTruth

EXOT22SlimmingHelper.IncludeMuonTriggerContent = True
EXOT22SlimmingHelper.AppendContentToStream(EXOT22Stream)
