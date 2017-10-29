#********************************************************************
# SUSY17.py 
# reductionConf flag SUSY17 in Reco_tf.py   
#********************************************************************

from DerivationFrameworkCore.DerivationFrameworkMaster import *
from DerivationFrameworkJetEtMiss.JetCommon import *
from DerivationFrameworkJetEtMiss.ExtendedJetCommon import *
from DerivationFrameworkEGamma.EGammaCommon import *
from DerivationFrameworkMuons.MuonsCommon import *
if DerivationFrameworkIsMonteCarlo:
  from DerivationFrameworkMCTruth.MCTruthCommon import addStandardTruthContents
  addStandardTruthContents()
from DerivationFrameworkInDet.InDetCommon import *
from DerivationFrameworkJetEtMiss.METCommon import *

### Set up stream
streamName = derivationFlags.WriteDAOD_SUSY17Stream.StreamName
fileName   = buildFileName( derivationFlags.WriteDAOD_SUSY17Stream )
SUSY17Stream = MSMgr.NewPoolRootStream( streamName, fileName )
SUSY17Stream.AcceptAlgs(["SUSY17KernelSkim"])

### Init
from DerivationFrameworkCore.ThinningHelper import ThinningHelper
SUSY17ThinningHelper = ThinningHelper( "SUSY17ThinningHelper" )
thinningTools       = []
AugmentationTools   = []

# stream-specific sequence for on-the-fly jet building
SeqSUSY17 = CfgMgr.AthSequencer("SeqSUSY17")
DerivationFrameworkJob += SeqSUSY17


#====================================================================
# Trigger navigation thinning
#====================================================================
from DerivationFrameworkSUSY.SUSY17TriggerList import triggersNavThin
SUSY17ThinningHelper.TriggerChains = '|'.join(triggersNavThin)

SUSY17ThinningHelper.AppendToStream( SUSY17Stream )
     
#====================================================================
# THINNING TOOLS 
#====================================================================

# B.M.: likely not used
# TrackParticles directly
#SUSY17TPThinningTool = DerivationFramework__TrackParticleThinning(name = "SUSY17TPThinningTool",
#                                                                 ThinningService         = SUSY17ThinningHelper.ThinningSvc(),
#                                                                 SelectionString         = "InDetTrackParticles.pt > 10*GeV",
#                                                                 InDetTrackParticlesKey  = "InDetTrackParticles")
#ToolSvc += SUSY17TPThinningTool
#thinningTools.append(SUSY17TPThinningTool)

# TrackParticles associated with Muons
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__MuonTrackParticleThinning
SUSY17MuonTPThinningTool = DerivationFramework__MuonTrackParticleThinning(name                    = "SUSY17MuonTPThinningTool",
                                                                         ThinningService         = SUSY17ThinningHelper.ThinningSvc(),
                                                                         MuonKey                 = "Muons",
                                                                         InDetTrackParticlesKey  = "InDetTrackParticles")
ToolSvc += SUSY17MuonTPThinningTool
thinningTools.append(SUSY17MuonTPThinningTool)

# TrackParticles associated with electrons
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__EgammaTrackParticleThinning
SUSY17ElectronTPThinningTool = DerivationFramework__EgammaTrackParticleThinning(name                    	= "SUSY17ElectronTPThinningTool",
                                                                                 ThinningService        = SUSY17ThinningHelper.ThinningSvc(),
                                                                                 SGKey             	= "Electrons",
                                                                                 InDetTrackParticlesKey = "InDetTrackParticles")
ToolSvc += SUSY17ElectronTPThinningTool
thinningTools.append(SUSY17ElectronTPThinningTool)

# TrackParticles associated with photons
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__EgammaTrackParticleThinning
SUSY17PhotonTPThinningTool = DerivationFramework__EgammaTrackParticleThinning(name			 = "SUSY17PhotonTPThinningTool",
										 ThinningService	 = SUSY17ThinningHelper.ThinningSvc(),
										 SGKey			 = "Photons",
										 InDetTrackParticlesKey  = "InDetTrackParticles")
ToolSvc += SUSY17PhotonTPThinningTool
thinningTools.append(SUSY17PhotonTPThinningTool)

# TrackParticles associated with taus
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__TauTrackParticleThinning
SUSY17TauTPThinningTool = DerivationFramework__TauTrackParticleThinning( name            = "SUSY17TauTPThinningTool",
                                                                        ThinningService = SUSY17ThinningHelper.ThinningSvc(),
                                                                        TauKey          = "TauJets",
                                                                        InDetTrackParticlesKey  = "InDetTrackParticles")
ToolSvc += SUSY17TauTPThinningTool
thinningTools.append(SUSY17TauTPThinningTool)

# TrackParticles associated with LC jets: useful when the global track thinning has a pT threshold ~1-2 GeV
#from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__JetTrackParticleThinning
#SUSY17LCJetsTrackThinningTool = DerivationFramework__JetTrackParticleThinning( name            = "SUSY17LCJetsTrackThinningTool",
#                                                                              ThinningService = SUSY17ThinningHelper.ThinningSvc(),
#                                                                              JetKey          = "AntiKt4LCTopoJets",
#                                                                              SelectionString         = "AntiKt4LCTopoJets.pt > 18*GeV",
#                                                                              InDetTrackParticlesKey  = "InDetTrackParticles")
#ToolSvc += SUSY17LCJetsTrackThinningTool
#thinningTools.append(SUSY17LCJetsTrackThinningTool)

#====================================================================
# TRUTH THINNING
#====================================================================
if DerivationFrameworkIsMonteCarlo:
  from DerivationFrameworkMCTruth.DerivationFrameworkMCTruthConf import DerivationFramework__MenuTruthThinning
  SUSY17TruthThinningTool = DerivationFramework__MenuTruthThinning(name              = "SUSY17TruthThinningTool",
                                                       ThinningService              = SUSY17ThinningHelper.ThinningSvc(),
                                                       WritePartons                 = False,
                                                       WriteHadrons                 = False,
                                                       WriteBHadrons                = True,                                                                                               
                                                       WriteGeant                   = False,
                                                       GeantPhotonPtThresh          = 20000,
                                                       WriteTauHad                  = True,
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


  # Decorate Electron with bkg electron type/origin 
  from MCTruthClassifier.MCTruthClassifierBase import MCTruthClassifier as BkgElectronMCTruthClassifier   
  from DerivationFrameworkEGamma.DerivationFrameworkEGammaConf import DerivationFramework__BkgElectronClassification 
  BkgElectronClassificationTool = DerivationFramework__BkgElectronClassification (name = "BkgElectronClassificationTool",MCTruthClassifierTool = BkgElectronMCTruthClassifier)
  ToolSvc += BkgElectronClassificationTool
  AugmentationTools.append(BkgElectronClassificationTool)

  ToolSvc += SUSY17TruthThinningTool
  thinningTools.append(SUSY17TruthThinningTool)
	
#====================================================================
# SKIMMING TOOL 
#====================================================================


muonsRequirementsSL = '(Muons.pt >= 3.5*GeV) && (Muons.pt <= 25.*GeV) && (abs(Muons.eta) < 2.7) && (Muons.DFCommonMuonsPreselection)'
electronsRequirementsSL = '(Electrons.pt > 4.5*GeV) && (Electrons.pt <= 25.*GeV) && (abs(Electrons.eta) < 2.6) && ((Electrons.Loose) || (Electrons.DFCommonElectronsLHLoose))'
objectSelectionSL = '(count('+electronsRequirementsSL+') + count('+muonsRequirementsSL+') >= 1)'

muonsRequirementsHL = '(Muons.pt >= 25.*GeV) && (abs(Muons.eta) < 2.7) && (Muons.DFCommonMuonsPreselection)'
electronsRequirementsHL = '(Electrons.pt > 25.*GeV) && (abs(Electrons.eta) < 2.6) && ((Electrons.Loose) || (Electrons.DFCommonElectronsLHLoose))'
objectSelectionHL = '(count('+electronsRequirementsHL+') + count('+muonsRequirementsHL+') >= 1)'

muonsRequirements = '(Muons.pt >= 3.5*GeV) && (abs(Muons.eta) < 2.7) && (Muons.DFCommonMuonsPreselection)'
electronsRequirements = '(Electrons.pt > 4.5*GeV) && (abs(Electrons.eta) < 2.6) && ((Electrons.Loose) || (Electrons.DFCommonElectronsLHLoose))'
objectSelection = '(count('+electronsRequirements+') + count('+muonsRequirements+') >= 1)'

expression = objectSelection

applyJetCalibration_xAODColl("AntiKt4EMTopo", SeqSUSY17)

from DerivationFrameworkSUSY.SUSY5TriggerList import triggersNavThin
from DerivationFrameworkSUSY.SUSY5TriggerList import METorPhoton_triggers
from DerivationFrameworkSUSY.SUSY5TriggerList import Lepton_triggers
from DerivationFrameworkSUSY.SUSY5TriggerList import PrescaledLowPtTriggers
from DerivationFrameworkSUSY.SUSY5TriggerList import PrescaledHighPtTriggers

trig_expression = '(' + ' || '.join(METorPhoton_triggers+Lepton_triggers) + ')' 
MEttrig_expression ='(' + ' || '.join(METorPhoton_triggers) + ')' 
Prestrig_expression ='(' + ' || '.join(PrescaledLowPtTriggers + PrescaledHighPtTriggers) + ')' 

JetEleExpression = '(count(AntiKt4EMTopoJets.DFCommonJets_Calib_pt>25*GeV && abs(AntiKt4EMTopoJets.DFCommonJets_Calib_eta)<2.8)>=2)'

LepTrigexpression = '('+'('+trig_expression+'&&'+objectSelectionHL+'&&'+JetEleExpression+')'+'||'+'('+MEttrig_expression +'&&'+ objectSelectionSL+'&&'+JetEleExpression+')'+'||'+'('+Prestrig_expression +'&&'+ JetEleExpression +'&&'+ objectSelection+')'+')'

expression = LepTrigexpression

from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__xAODStringSkimmingTool
SUSY17SkimmingTool = DerivationFramework__xAODStringSkimmingTool( name = "SUSY17SkimmingTool",
                                                                expression = expression)

ToolSvc += SUSY17SkimmingTool

#=======================================
# CREATE THE DERIVATION KERNEL ALGORITHM   
#=======================================
from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__DerivationKernel

# Add sumOfWeights metadata for LHE3 multiweights =======
from DerivationFrameworkCore.LHE3WeightMetadata import *

#==============================================================================
# SUSY signal augmentation
#==============================================================================
from DerivationFrameworkSUSY.DecorateSUSYProcess import IsSUSYSignal
if IsSUSYSignal():
   
   from DerivationFrameworkSUSY.DecorateSUSYProcess import DecorateSUSYProcess
   SeqSUSY17 += CfgMgr.DerivationFramework__DerivationKernel("SUSY17KernelSigAug",
                                                            AugmentationTools = DecorateSUSYProcess("SUSY17")
                                                            )
   
   from DerivationFrameworkSUSY.SUSYWeightMetadata import *


#==============================================================================
# SUSY skimming selection
#==============================================================================
SeqSUSY17 += CfgMgr.DerivationFramework__DerivationKernel(
  "SUSY17KernelSkim",
  SkimmingTools = [SUSY17SkimmingTool]
)


#==============================================================================
# Jet building
#==============================================================================
OutputJets["SUSY17"] = []
reducedJetList = [ "AntiKt2PV0TrackJets" ]
# now part of MCTruthCommon
#if DerivationFrameworkIsMonteCarlo:
#  reducedJetList += [ "AntiKt4TruthJets", "AntiKt4TruthWZJets" ]

# AntiKt2PV0TrackJets is flavour-tagged automatically
replaceAODReducedJets(reducedJetList, SeqSUSY17, "SUSY17")


#==============================================================================
# Tau truth building/matching
#==============================================================================
# now part of MCTruthCommon
#if DerivationFrameworkIsMonteCarlo:
#  from DerivationFrameworkSUSY.SUSYTruthCommon import addTruthTaus
#  addTruthTaus(AugmentationTools)


#==============================================================================
# Augment after skim
#==============================================================================
SeqSUSY17 += CfgMgr.DerivationFramework__DerivationKernel(
	"SUSY17KernelAug",
	AugmentationTools = AugmentationTools,
	ThinningTools = thinningTools,
)


#====================================================================
# CONTENT LIST  
#====================================================================
# This might be the kind of set-up one would have for a muon based analysis
from DerivationFrameworkCore.SlimmingHelper import SlimmingHelper
SUSY17SlimmingHelper = SlimmingHelper("SUSY17SlimmingHelper")
SUSY17SlimmingHelper.SmartCollections = ["Electrons",
                                        "Photons",
                                        "Muons",
                                        "TauJets",
                                        "AntiKt4EMTopoJets",
 #K.Onogi removed 15/11/16              "AntiKt4LCTopoJets",
                                        "MET_Reference_AntiKt4EMTopo",
                                        "BTagging_AntiKt4EMTopo",
                                        "InDetTrackParticles",
                                        "PrimaryVertices"]
SUSY17SlimmingHelper.AllVariables = ["TruthParticles", "TruthEvents", "TruthVertices", "MET_Truth", "MET_Track"]
SUSY17SlimmingHelper.ExtraVariables = ["BTagging_AntiKt4EMTopo.MV1_discriminant.MV1c_discriminant",
                                      "Muons.ptcone30.ptcone20.charge.quality.InnerDetectorPt.MuonSpectrometerPt.CaloLRLikelihood.CaloMuonIDTag",
                                      "Photons.author.Loose.Tight",
                                      "AntiKt4EMTopoJets.NumTrkPt1000.TrackWidthPt1000.NumTrkPt500.DFCommonJets_Calib_pt.DFCommonJets_Calib_eta.DFCommonJets_Calib_phi",
                                      "GSFTrackParticles.z0.d0.vz.definingParametersCovMatrix","CombinedMuonTrackParticles.d0.z0.vz.definingParametersCovMatrix.truthOrigin.truthType",
                                      "ExtrapolatedMuonTrackParticles.d0.z0.vz.definingParametersCovMatrix.truthOrigin.truthType",
                                      "TauJets.IsTruthMatched.truthOrigin.truthType.truthParticleLink.truthJetLink"
                                      + ".PanTau_isPanTauCandidate.ptPanTauCellBased.etaPanTauCellBased.phiPanTauCellBased.mPanTauCellBased",
                                      "TauNeutralParticleFlowObjects.pt.eta.phi.m",
                                      "TauChargedParticleFlowObjects.pt.eta.phi.m",
                                      "MuonTruthParticles.barcode.decayVtxLink.e.m.pdgId.prodVtxLink.px.py.pz.recoMuonLink.status.truthOrigin.truthType",
                                      "AntiKt4TruthJets.eta.m.phi.pt.TruthLabelDeltaR_B.TruthLabelDeltaR_C.TruthLabelDeltaR_T.TruthLabelID.ConeTruthLabelID.PartonTruthLabelID",
                                      "Electrons.bkgTruthType.bkgTruthOrigin",
 #P. Pani removed 20/06/16                                     "AntiKt3PV0TrackJets.eta.m.phi.pt.btagging.btaggingLink",
 #P. Pani removed 20/06/16                                     "BTagging_AntiKt3Track.MV2c20_discriminant",
                                      "AntiKt2PV0TrackJets.eta.m.phi.pt.btagging.btaggingLink",
                                      "BTagging_AntiKt2Track.MV2c10_discriminant"]

SUSY17SlimmingHelper.IncludeMuonTriggerContent   = True
SUSY17SlimmingHelper.IncludeEGammaTriggerContent = True
SUSY17SlimmingHelper.IncludeBPhysTriggerContent  = False
SUSY17SlimmingHelper.IncludeJetTriggerContent    = False
SUSY17SlimmingHelper.IncludeTauTriggerContent    = False
SUSY17SlimmingHelper.IncludeEtMissTriggerContent = True
SUSY17SlimmingHelper.IncludeBJetTriggerContent   = False

# All standard truth particle collections are provided by DerivationFrameworkMCTruth (TruthDerivationTools.py)
# Most of the new containers are centrally added to SlimmingHelper via DerivationFrameworkCore ContainersOnTheFly.py
if DerivationFrameworkIsMonteCarlo:

  SUSY17SlimmingHelper.AppendToDictionary = {'TruthTop':'xAOD::TruthParticleContainer','TruthTopAux':'xAOD::TruthParticleAuxContainer',
                                             'TruthBSM':'xAOD::TruthParticleContainer','TruthBSMAux':'xAOD::TruthParticleAuxContainer',
                                             'TruthBoson':'xAOD::TruthParticleContainer','TruthBosonAux':'xAOD::TruthParticleAuxContainer'}
  
  SUSY17SlimmingHelper.AllVariables += ["TruthElectrons", "TruthMuons", "TruthTaus", "TruthPhotons", "TruthNeutrinos", "TruthTop", "TruthBSM", "TruthBoson"]   


SUSY17SlimmingHelper.AppendContentToStream(SUSY17Stream)

