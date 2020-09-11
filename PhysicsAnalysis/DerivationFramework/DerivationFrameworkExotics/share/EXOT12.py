#********************************************************************
# EXOT12.py 
# reductionConf flag EXOT12 in Reco_tf.py   
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

EXOT12Seq = CfgMgr.AthSequencer("EXOT12Sequence")
DerivationFrameworkJob += EXOT12Seq

#====================================================================
# SET UP STREAM   
#====================================================================
streamName = derivationFlags.WriteDAOD_EXOT12Stream.StreamName
fileName   = buildFileName( derivationFlags.WriteDAOD_EXOT12Stream )
EXOT12Stream = MSMgr.NewPoolRootStream( streamName, fileName )
EXOT12Stream.AcceptAlgs(["EXOT12Kernel"])

#====================================================================
# THINNING TOOLS
#====================================================================

#thinning helper
from DerivationFrameworkCore.ThinningHelper import ThinningHelper
EXOT12ThinningHelper = ThinningHelper( "EXOT12ThinningHelper" )
EXOT12ThinningHelper.AppendToStream( EXOT12Stream )

thinningTools = []
augmentationTools = []

# Tracks associated with Muons
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__MuonTrackParticleThinning
EXOT12MuonTPThinningTool = DerivationFramework__MuonTrackParticleThinning(name                    = "EXOT12MuonTPThinningTool",
                                                                          ThinningService         = EXOT12ThinningHelper.ThinningSvc(),
                                                                          MuonKey                 = "Muons",
                                                                          InDetTrackParticlesKey  = "InDetTrackParticles",
                                                                          ConeSize                =  0) # change wrt. EXOT0 that uses 0.4
ToolSvc += EXOT12MuonTPThinningTool
thinningTools.append(EXOT12MuonTPThinningTool)

# Tracks associated with Electrons
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__EgammaTrackParticleThinning
EXOT12ElectronTPThinningTool = DerivationFramework__EgammaTrackParticleThinning(name                    = "EXOT12ElectronTPThinningTool",
                                                                                ThinningService         = EXOT12ThinningHelper.ThinningSvc(),
                                                                                SGKey                   = "Electrons",
                                                                                InDetTrackParticlesKey  = "InDetTrackParticles",
                                                                                ConeSize                =  0) # change wrt. EXOT0 that uses 0.4
ToolSvc += EXOT12ElectronTPThinningTool
thinningTools.append(EXOT12ElectronTPThinningTool)

# TrackParticles associated with taus
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__TauTrackParticleThinning
EXOT12TauTPThinningTool = DerivationFramework__TauTrackParticleThinning(name                    = "EXOT12TauTPThinningTool",
                                                                        ThinningService         = EXOT12ThinningHelper.ThinningSvc(),
                                                                        TauKey                  = "TauJets",
                                                                        InDetTrackParticlesKey  = "InDetTrackParticles",
                                                                        ConeSize                =  0) # change wrt. EXOT0 that uses 0.4
ToolSvc += EXOT12TauTPThinningTool
thinningTools.append(EXOT12TauTPThinningTool)

# truth thinning
from DerivationFrameworkMCTruth.DerivationFrameworkMCTruthConf import DerivationFramework__MenuTruthThinning
EXOT12TruthTool = DerivationFramework__MenuTruthThinning(name                  = "EXOT12TruthTool",
                                                         ThinningService       = EXOT12ThinningHelper.ThinningSvc(),
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
  EXOT12TruthClassifier = MCTruthClassifier(name                      = "EXOT12TruthClassifier",
                                            ParticleCaloExtensionTool = "")
  ToolSvc += EXOT12TruthClassifier

  from DerivationFrameworkMCTruth.DerivationFrameworkMCTruthConf import DerivationFramework__TruthClassificationDecorator
  EXOT12ClassificationDecorator = DerivationFramework__TruthClassificationDecorator(name              = "EXOT12ClassificationDecorator",
                                                                                    ParticlesKey      = "TruthParticles",
                                                                                    MCTruthClassifier = EXOT12TruthClassifier)
  ToolSvc += EXOT12ClassificationDecorator
  augmentationTools.append(EXOT12ClassificationDecorator)

  ToolSvc += EXOT12TruthTool
  thinningTools.append(EXOT12TruthTool)

  # tau truth
  from DerivationFrameworkTau.TauTruthCommon import scheduleTauTruthTools
  scheduleTauTruthTools()

truth_cond = "((abs(TruthParticles.pdgId) >= 11) && (abs(TruthParticles.pdgId) <= 16) && (TruthParticles.pt > 1*GeV) && ((TruthParticles.status ==1) || (TruthParticles.status ==2) || (TruthParticles.status ==3) || (TruthParticles.status ==23)) && (TruthParticles.barcode<200000))" # lepton conditions
truth_cond += "|| (abs(TruthParticles.pdgId) == 9900041) || (abs(TruthParticles.pdgId) == 9900042)" # H++
truth_cond += "|| (abs(TruthParticles.pdgId) == 3023)" # Y++
truth_cond += "|| (abs(TruthParticles.pdgId) == 8000018) || (abs(TruthParticles.pdgId) == 8000020)" # Type III seesaw
truth_cond += "|| (abs(TruthParticles.pdgId) == 9000005) || (abs(TruthParticles.pdgId) == 9000006)" # Vector-like tau-prime
truth_cond += "|| (abs(TruthParticles.pdgId) == 9900012) || (abs(TruthParticles.pdgId) == 9900014) || (abs(TruthParticles.pdgId) == 9900016) || (abs(TruthParticles.pdgId) == 9900024) || (abs(TruthParticles.pdgId) == 34)" # LRSM Heavy Neutrino

from DerivationFrameworkMCTruth.DerivationFrameworkMCTruthConf import DerivationFramework__GenericTruthThinning
EXOT12TruthTool2 = DerivationFramework__GenericTruthThinning(name                         = "EXOT12TruthTool2",
                                                             ThinningService              = EXOT12ThinningHelper.ThinningSvc(),
                                                             ParticleSelectionString      = truth_cond,
                                                             PreserveDescendants          = False,
                                                             PreserveGeneratorDescendants = True,
                                                             PreserveAncestors            = True)
if DerivationFrameworkHasTruth:
  ToolSvc += EXOT12TruthTool2
  thinningTools.append(EXOT12TruthTool2)

#====================================================================
# SKIMMING TOOL 
#====================================================================

from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__xAODStringSkimmingTool
el_exp = '(Electrons.pt > 20*GeV && Electrons.DFCommonElectronsLHLoose)' # LHLoose electron ID
mu_exp = '(Muons.pt > 20*GeV && Muons.DFCommonGoodMuon && Muons.muonType == 0)' # pass ID cuts and be a combined muon
expression = '(count(' + el_exp + ') >= 2)'
expression += ' || (count(' + mu_exp + ') >= 2)'
expression += ' || ((count(' + el_exp + ') + count(' + mu_exp + ')) >= 2)'
EXOT12SkimmingTool = DerivationFramework__xAODStringSkimmingTool(name = "EXOT12SkimmingTool1", expression = expression)
ToolSvc += EXOT12SkimmingTool

#=======================================
# CREATE THE DERIVATION KERNEL ALGORITHM   
#=======================================

from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__DerivationKernel
EXOT12Seq += CfgMgr.DerivationFramework__DerivationKernel("EXOT12Kernel_skim", SkimmingTools = [EXOT12SkimmingTool])
EXOT12Seq += CfgMgr.DerivationFramework__DerivationKernel("EXOT12Kernel",
                                                          ThinningTools = thinningTools,
                                                          AugmentationTools = augmentationTools)

#=======================================
# JETS
#=======================================
OutputJets["EXOT12"] = []

# Adding Btagging for PFlowJets
from DerivationFrameworkFlavourTag.FlavourTagCommon import FlavorTagInit
FlavorTagInit(JetCollections = ['AntiKt4EMPFlowJets'], Sequencer = EXOT12Seq)


#====================================================================
# Add the containers to the output stream - slimming done here
#====================================================================
from DerivationFrameworkCore.SlimmingHelper import SlimmingHelper
from DerivationFrameworkExotics.EXOT12ContentList import *
EXOT12SlimmingHelper = SlimmingHelper("EXOT12SlimmingHelper")
EXOT12SlimmingHelper.StaticContent = EXOT12Content
EXOT12SlimmingHelper.AllVariables = EXOT12AllVariables
EXOT12SlimmingHelper.ExtraVariables = EXOT12ExtraVariables
EXOT12SlimmingHelper.SmartCollections = EXOT12SmartCollections
if DerivationFrameworkHasTruth:
  EXOT12SlimmingHelper.AllVariables += EXOT12AllVariablesTruth
  EXOT12SlimmingHelper.ExtraVariables += EXOT12ExtraVariablesTruth
  EXOT12SlimmingHelper.SmartCollections += EXOT12SmartCollectionsTruth

EXOT12SlimmingHelper.IncludeEGammaTriggerContent = True
EXOT12SlimmingHelper.IncludeMuonTriggerContent = True
EXOT12SlimmingHelper.AppendContentToStream(EXOT12Stream)
