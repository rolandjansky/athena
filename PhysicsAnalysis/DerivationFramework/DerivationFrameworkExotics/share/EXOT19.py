#********************************************************************
# EXOT19.py 
# reductionConf flag EXOT19 in Reco_tf.py   
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

EXOT19Seq = CfgMgr.AthSequencer("EXOT19Sequence")
DerivationFrameworkJob += EXOT19Seq

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
# THINNING TOOLS
#====================================================================

thinningTools = []
augmentationTools = []

# Tracks associated with Muons
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__MuonTrackParticleThinning
EXOT19MuonTPThinningTool = DerivationFramework__MuonTrackParticleThinning(name                    = "EXOT19MuonTPThinningTool",
                                                                          ThinningService         = EXOT19ThinningHelper.ThinningSvc(),
                                                                          MuonKey                 = "Muons",
                                                                          InDetTrackParticlesKey  = "InDetTrackParticles",
                                                                          ConeSize                =  0) # change wrt. EXOT0 that uses 0.4
ToolSvc += EXOT19MuonTPThinningTool
thinningTools.append(EXOT19MuonTPThinningTool)

# Tracks associated with Electrons
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__EgammaTrackParticleThinning
EXOT19ElectronTPThinningTool = DerivationFramework__EgammaTrackParticleThinning(name                    = "EXOT19ElectronTPThinningTool",
                                                                                ThinningService         = EXOT19ThinningHelper.ThinningSvc(),
                                                                                SGKey                   = "Electrons",
                                                                                InDetTrackParticlesKey  = "InDetTrackParticles",
                                                                                ConeSize                =  0) # change wrt. EXOT0 that uses 0.4
ToolSvc += EXOT19ElectronTPThinningTool
thinningTools.append(EXOT19ElectronTPThinningTool)

# Tracks associated with Photons
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__EgammaTrackParticleThinning
EXOT19PhotonTPThinningTool = DerivationFramework__EgammaTrackParticleThinning(name                    = "EXOT19PhotonTPThinningTool",
                                                                              ThinningService         = EXOT19ThinningHelper.ThinningSvc(),
                                                                              SGKey                   = "Photons",
                                                                              InDetTrackParticlesKey  = "InDetTrackParticles",
                                                                              ConeSize                =  0) # change wrt. EXOT0 that uses 0.4
ToolSvc += EXOT19PhotonTPThinningTool
thinningTools.append(EXOT19PhotonTPThinningTool)

# TrackParticles associated with taus
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__TauTrackParticleThinning
EXOT19TauTPThinningTool = DerivationFramework__TauTrackParticleThinning(name                    = "EXOT19TauTPThinningTool",
                                                                        ThinningService         = EXOT19ThinningHelper.ThinningSvc(),
                                                                        TauKey                  = "TauJets",
                                                                        InDetTrackParticlesKey  = "InDetTrackParticles",
                                                                        ConeSize                =  0) # change wrt. EXOT0 that uses 0.4
ToolSvc += EXOT19TauTPThinningTool
thinningTools.append(EXOT19TauTPThinningTool)

# truth thinning
from DerivationFrameworkMCTruth.DerivationFrameworkMCTruthConf import DerivationFramework__MenuTruthThinning
EXOT19TruthTool = DerivationFramework__MenuTruthThinning(name                  = "EXOT19TruthTool",
                                                         ThinningService       = EXOT19ThinningHelper.ThinningSvc(),
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
  EXOT19TruthClassifier = MCTruthClassifier(name                      = "EXOT19TruthClassifier",
                                            ParticleCaloExtensionTool = "")
  ToolSvc += EXOT19TruthClassifier

  from DerivationFrameworkMCTruth.DerivationFrameworkMCTruthConf import DerivationFramework__TruthClassificationDecorator
  EXOT19ClassificationDecorator = DerivationFramework__TruthClassificationDecorator(name              = "EXOT19ClassificationDecorator",
                                                                                    ParticlesKey      = "TruthParticles",
                                                                                    MCTruthClassifier = EXOT19TruthClassifier)
  ToolSvc += EXOT19ClassificationDecorator
  augmentationTools.append(EXOT19ClassificationDecorator)

  ToolSvc += EXOT19TruthTool
  thinningTools.append(EXOT19TruthTool)

  # tau truth
  from DerivationFrameworkTau.TauTruthCommon import scheduleTauTruthTools
  scheduleTauTruthTools()

truth_cond_Lepton = "((abs(TruthParticles.pdgId) >= 11) && (abs(TruthParticles.pdgId) <= 16) && (TruthParticles.pt > 1*GeV) && ((TruthParticles.status ==1) || (TruthParticles.status ==2) || (TruthParticles.status ==3) || (TruthParticles.status ==23)) && (TruthParticles.barcode<200000))"

from DerivationFrameworkMCTruth.DerivationFrameworkMCTruthConf import DerivationFramework__GenericTruthThinning
EXOT19TruthTool2 = DerivationFramework__GenericTruthThinning(name                         = "EXOT19TruthTool2",
                                                             ThinningService              = "EXOT19ThinningSvc",
                                                             ParticleSelectionString      = truth_cond_Lepton,
                                                             PreserveDescendants          = False,
                                                             PreserveGeneratorDescendants = True,
                                                             PreserveAncestors            = True)
if DerivationFrameworkHasTruth:
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
OutputJets["EXOT19"] = []

# Adding Btagging for PFlowJets
from DerivationFrameworkFlavourTag.FlavourTagCommon import FlavorTagInit
FlavorTagInit(JetCollections = ['AntiKt4EMPFlowJets'], Sequencer = EXOT19Seq)


#=======================================
# CREATE THE DERIVATION KERNEL ALGORITHM   
#=======================================

from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__DerivationKernel
EXOT19Seq += CfgMgr.DerivationFramework__DerivationKernel("EXOT19Kernel_skim", SkimmingTools = [EXOT19SkimmingTool])
EXOT19Seq += CfgMgr.DerivationFramework__DerivationKernel("EXOT19Kernel",
                                                          ThinningTools = thinningTools,
                                                          AugmentationTools = augmentationTools)

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
if DerivationFrameworkHasTruth:
  EXOT19SlimmingHelper.AllVariables += EXOT19AllVariablesTruth
  EXOT19SlimmingHelper.ExtraVariables += EXOT19ExtraVariablesTruth
  EXOT19SlimmingHelper.SmartCollections += EXOT19SmartCollectionsTruth

EXOT19SlimmingHelper.IncludeEGammaTriggerContent = True
EXOT19SlimmingHelper.AppendContentToStream(EXOT19Stream)
