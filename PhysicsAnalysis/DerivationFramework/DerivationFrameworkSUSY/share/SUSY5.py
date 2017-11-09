#********************************************************************
# SUSY5.py 
# reductionConf flag SUSY5 in Reco_tf.py   
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
streamName = derivationFlags.WriteDAOD_SUSY5Stream.StreamName
fileName   = buildFileName( derivationFlags.WriteDAOD_SUSY5Stream )
SUSY5Stream = MSMgr.NewPoolRootStream( streamName, fileName )
SUSY5Stream.AcceptAlgs(["SUSY5KernelSkim"])

### Init
from DerivationFrameworkCore.ThinningHelper import ThinningHelper
SUSY5ThinningHelper = ThinningHelper( "SUSY5ThinningHelper" )
thinningTools       = []
AugmentationTools   = []

# stream-specific sequence for on-the-fly jet building
SeqSUSY5 = CfgMgr.AthSequencer("SeqSUSY5")
DerivationFrameworkJob += SeqSUSY5


#====================================================================
# Trigger navigation thinning
#====================================================================
from DerivationFrameworkSUSY.SUSY5TriggerList import triggersNavThin
SUSY5ThinningHelper.TriggerChains = '|'.join(triggersNavThin)

SUSY5ThinningHelper.AppendToStream( SUSY5Stream )
     
#====================================================================
# THINNING TOOLS 
#====================================================================

# B.M.: likely not used
# TrackParticles directly
#SUSY5TPThinningTool = DerivationFramework__TrackParticleThinning(name = "SUSY5TPThinningTool",
#                                                                 ThinningService         = SUSY5ThinningHelper.ThinningSvc(),
#                                                                 SelectionString         = "InDetTrackParticles.pt > 10*GeV",
#                                                                 InDetTrackParticlesKey  = "InDetTrackParticles")
#ToolSvc += SUSY5TPThinningTool
#thinningTools.append(SUSY5TPThinningTool)

# TrackParticles associated with Muons
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__MuonTrackParticleThinning
SUSY5MuonTPThinningTool = DerivationFramework__MuonTrackParticleThinning(name                    = "SUSY5MuonTPThinningTool",
                                                                         ThinningService         = SUSY5ThinningHelper.ThinningSvc(),
                                                                         MuonKey                 = "Muons",
                                                                         InDetTrackParticlesKey  = "InDetTrackParticles")
ToolSvc += SUSY5MuonTPThinningTool
thinningTools.append(SUSY5MuonTPThinningTool)

# TrackParticles associated with electrons
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__EgammaTrackParticleThinning
SUSY5ElectronTPThinningTool = DerivationFramework__EgammaTrackParticleThinning(name                    	= "SUSY5ElectronTPThinningTool",
                                                                                 ThinningService        = SUSY5ThinningHelper.ThinningSvc(),
                                                                                 SGKey             	= "Electrons",
                                                                                 InDetTrackParticlesKey = "InDetTrackParticles")
ToolSvc += SUSY5ElectronTPThinningTool
thinningTools.append(SUSY5ElectronTPThinningTool)

# TrackParticles associated with photons
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__EgammaTrackParticleThinning
SUSY5PhotonTPThinningTool = DerivationFramework__EgammaTrackParticleThinning(name			 = "SUSY5PhotonTPThinningTool",
										 ThinningService	 = SUSY5ThinningHelper.ThinningSvc(),
										 SGKey			 = "Photons",
										 InDetTrackParticlesKey  = "InDetTrackParticles")
ToolSvc += SUSY5PhotonTPThinningTool
thinningTools.append(SUSY5PhotonTPThinningTool)

# TrackParticles associated with taus
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__TauTrackParticleThinning
SUSY5TauTPThinningTool = DerivationFramework__TauTrackParticleThinning( name            = "SUSY5TauTPThinningTool",
                                                                        ThinningService = SUSY5ThinningHelper.ThinningSvc(),
                                                                        TauKey          = "TauJets",
                                                                        InDetTrackParticlesKey  = "InDetTrackParticles")
ToolSvc += SUSY5TauTPThinningTool
thinningTools.append(SUSY5TauTPThinningTool)

# TrackParticles associated with LC jets: useful when the global track thinning has a pT threshold ~1-2 GeV
#from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__JetTrackParticleThinning
#SUSY5LCJetsTrackThinningTool = DerivationFramework__JetTrackParticleThinning( name            = "SUSY5LCJetsTrackThinningTool",
#                                                                              ThinningService = SUSY5ThinningHelper.ThinningSvc(),
#                                                                              JetKey          = "AntiKt4LCTopoJets",
#                                                                              SelectionString         = "AntiKt4LCTopoJets.pt > 18*GeV",
#                                                                              InDetTrackParticlesKey  = "InDetTrackParticles")
#ToolSvc += SUSY5LCJetsTrackThinningTool
#thinningTools.append(SUSY5LCJetsTrackThinningTool)

#====================================================================
# TRUTH THINNING
#====================================================================
if DerivationFrameworkIsMonteCarlo:
  from DerivationFrameworkMCTruth.DerivationFrameworkMCTruthConf import DerivationFramework__MenuTruthThinning
  SUSY5TruthThinningTool = DerivationFramework__MenuTruthThinning(name              = "SUSY5TruthThinningTool",
                                                       ThinningService              = SUSY5ThinningHelper.ThinningSvc(),
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
                                                       WriteAllLeptons              = True,
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

  ToolSvc += SUSY5TruthThinningTool
  thinningTools.append(SUSY5TruthThinningTool)

	
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

# now done in ExtendedJetCommon
#applyJetCalibration_xAODColl("AntiKt4EMTopo", SeqSUSY5)

from DerivationFrameworkSUSY.SUSY5TriggerList import triggersNavThin
from DerivationFrameworkSUSY.SUSY5TriggerList import METorPhoton_triggers
from DerivationFrameworkSUSY.SUSY5TriggerList import Lepton_triggers
from DerivationFrameworkSUSY.SUSY5TriggerList import PrescaledLowPtTriggers
from DerivationFrameworkSUSY.SUSY5TriggerList import PrescaledHighPtTriggers

trig_expression = '(' + ' || '.join(METorPhoton_triggers+Lepton_triggers) + ')' 
MEttrig_expression ='(' + ' || '.join(METorPhoton_triggers) + ')' 

if not DerivationFrameworkIsMonteCarlo:
  Prestrig_expression ='(' + ' || '.join(PrescaledLowPtTriggers + PrescaledHighPtTriggers) + ')' 
  PresLowPttrig_expression ='(' + ' || '.join(PrescaledLowPtTriggers) + ')' 
  JetEleExpression = '(count(AntiKt4EMTopoJets.DFCommonJets_Calib_pt>25*GeV && abs(AntiKt4EMTopoJets.DFCommonJets_Calib_eta)<2.8)>=2)'
  JetEleLooseExpression = '(count(AntiKt4EMTopoJets.DFCommonJets_Calib_pt>10*GeV && abs(AntiKt4EMTopoJets.DFCommonJets_Calib_eta)<2.8)>=1)'
  LepTrigexpression = '('+'('+trig_expression+'&&'+objectSelectionHL+'&&'+JetEleExpression+')'+'||'+'('+MEttrig_expression +'&&'+ objectSelectionSL+'&&'+JetEleExpression+')'+'||'+'('+Prestrig_expression +'&&'+ JetEleExpression +'&&'+ objectSelection+')'+'||'+'('+PresLowPttrig_expression +'&&'+ JetEleLooseExpression +'&&'+ objectSelectionSL+')'+')'
else :
  LepTrigexpression = '('+'('+trig_expression+'&&'+objectSelectionHL+')'+'||'+'('+MEttrig_expression +'&&'+ objectSelectionSL+')'+')' 

expression = LepTrigexpression

from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__xAODStringSkimmingTool
SUSY5SkimmingTool = DerivationFramework__xAODStringSkimmingTool( name = "SUSY5SkimmingTool",
                                                                expression = expression)

ToolSvc += SUSY5SkimmingTool

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
   SeqSUSY5 += CfgMgr.DerivationFramework__DerivationKernel("SUSY5KernelSigAug",
                                                            AugmentationTools = DecorateSUSYProcess("SUSY5")
                                                            )
   
   from DerivationFrameworkSUSY.SUSYWeightMetadata import *


#==============================================================================
# SUSY skimming selection
#==============================================================================
SeqSUSY5 += CfgMgr.DerivationFramework__DerivationKernel(
  "SUSY5KernelSkim",
  SkimmingTools = [SUSY5SkimmingTool]
)


#==============================================================================
# Jet building
#==============================================================================
OutputJets["SUSY5"] = [] 
reducedJetList = [ "AntiKt2PV0TrackJets" ]

# now part of MCTruthCommon
#if DerivationFrameworkIsMonteCarlo:
#  reducedJetList += [ "AntiKt4TruthJets", "AntiKt4TruthWZJets" ]

replaceAODReducedJets(reducedJetList, SeqSUSY5, "SUSY5")


#==============================================================================
# Tau truth building/matching
#==============================================================================
# now part of MCTruthCommon
if DerivationFrameworkIsMonteCarlo:
#  from DerivationFrameworkSUSY.SUSYTruthCommon import addTruthTaus
#  addTruthTaus(AugmentationTools)
  ToolSvc.DFCommonTauTruthMatchingTool.WriteInvisibleFourMomentum = True

#==============================================================================
# Augment after skim
#==============================================================================
SeqSUSY5 += CfgMgr.DerivationFramework__DerivationKernel(
	"SUSY5KernelAug",
	AugmentationTools = AugmentationTools,
	ThinningTools = thinningTools,
)


#====================================================================
# CONTENT LIST  
#====================================================================
# This might be the kind of set-up one would have for a muon based analysis
from DerivationFrameworkCore.SlimmingHelper import SlimmingHelper
SUSY5SlimmingHelper = SlimmingHelper("SUSY5SlimmingHelper")
SUSY5SlimmingHelper.SmartCollections = ["Electrons",
                                        "Photons",
                                        "Muons",
                                        "TauJets",
                                        "AntiKt4EMTopoJets",
                                        #"AntiKt4LCTopoJets",
                                        "MET_Reference_AntiKt4EMTopo",
                                        "BTagging_AntiKt4EMTopo",
                                        "InDetTrackParticles",
                                        "PrimaryVertices"]
SUSY5SlimmingHelper.AllVariables = ["TruthParticles", "TruthEvents", "TruthVertices", "MET_Truth", "MET_Track"]
SUSY5SlimmingHelper.ExtraVariables = ["BTagging_AntiKt4EMTopo.MV1_discriminant.MV1c_discriminant",
                                      "Muons.ptcone30.ptcone20.charge.quality.InnerDetectorPt.MuonSpectrometerPt.CaloLRLikelihood.CaloMuonIDTag",
                                      "Photons.author.Loose.Tight",
                                      "AntiKt4EMTopoJets.NumTrkPt1000.TrackWidthPt1000.NumTrkPt500.DFCommonJets_Calib_pt.DFCommonJets_Calib_eta.DFCommonJets_Calib_phi"
                                      + ".N90Constituents.Timing.Width",
                                      "GSFTrackParticles.z0.d0.vz.definingParametersCovMatrix","CombinedMuonTrackParticles.d0.z0.vz.definingParametersCovMatrix.truthOrigin.truthType",
                                      "ExtrapolatedMuonTrackParticles.d0.z0.vz.definingParametersCovMatrix.truthOrigin.truthType",
                                      "TauJets.IsTruthMatched.truthOrigin.truthType.truthParticleLink.truthJetLink",
                                      "MuonTruthParticles.barcode.decayVtxLink.e.m.pdgId.prodVtxLink.px.py.pz.recoMuonLink.status.truthOrigin.truthType",
                                      "AntiKt4TruthJets.eta.m.phi.pt.TruthLabelDeltaR_B.TruthLabelDeltaR_C.TruthLabelDeltaR_T.TruthLabelID.ConeTruthLabelID.PartonTruthLabelID.HadronConeExclTruthLabelID",
                                      "Electrons.bkgTruthType.bkgTruthOrigin.bkgMotherPdgId.firstEgMotherTruthType.firstEgMotherTruthOrigin.firstEgMotherPdgId",
 #P. Pani removed 20/06/16                                     "AntiKt3PV0TrackJets.eta.m.phi.pt.btagging.btaggingLink",
 #P. Pani removed 20/06/16                                     "BTagging_AntiKt3Track.MV2c20_discriminant",
                                      "AntiKt2PV0TrackJets.eta.m.phi.pt.btagging.btaggingLink",
                                      "BTagging_AntiKt2Track.MV2c10_discriminant"]

SUSY5SlimmingHelper.IncludeMuonTriggerContent   = True
SUSY5SlimmingHelper.IncludeEGammaTriggerContent = True
SUSY5SlimmingHelper.IncludeBPhysTriggerContent  = False
SUSY5SlimmingHelper.IncludeJetTriggerContent    = False
SUSY5SlimmingHelper.IncludeTauTriggerContent    = False
SUSY5SlimmingHelper.IncludeEtMissTriggerContent = True
SUSY5SlimmingHelper.IncludeBJetTriggerContent   = False


# Remove 03/06/2016 P.Pani
#addJetOutputs(SUSY5SlimmingHelper,["LargeR", "SUSY5"], [], ["CamKt12LCTopoJets","AntiKt10LCTopoJets","AntiKt10TruthJets","CamKt12TruthWZJets","CamKt12TruthJets","AntiKt10TruthWZJets"])

# All standard truth particle collections are provided by DerivationFrameworkMCTruth (TruthDerivationTools.py)
# Most of the new containers are centrally added to SlimmingHelper via DerivationFrameworkCore ContainersOnTheFly.py
if DerivationFrameworkIsMonteCarlo:

  SUSY5SlimmingHelper.AppendToDictionary = {'TruthTop':'xAOD::TruthParticleContainer','TruthTopAux':'xAOD::TruthParticleAuxContainer',
                                            'TruthBSM':'xAOD::TruthParticleContainer','TruthBSMAux':'xAOD::TruthParticleAuxContainer',
                                            'TruthBoson':'xAOD::TruthParticleContainer','TruthBosonAux':'xAOD::TruthParticleAuxContainer'}
  
  SUSY5SlimmingHelper.AllVariables += ["TruthElectrons", "TruthMuons", "TruthTaus", "TruthPhotons", "TruthNeutrinos", "TruthTop", "TruthBSM", "TruthBoson"]   

SUSY5SlimmingHelper.AppendContentToStream(SUSY5Stream)

