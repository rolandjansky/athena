#********************************************************************
# SUSY14.py 
# reductionConf flag SUSY14 in Reco_tf.py   
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
from DerivationFrameworkFlavourTag.FlavourTagCommon import *


### Set up stream
streamName = derivationFlags.WriteDAOD_SUSY14Stream.StreamName
fileName   = buildFileName( derivationFlags.WriteDAOD_SUSY14Stream )
SUSY14Stream = MSMgr.NewPoolRootStream( streamName, fileName )
SUSY14Stream.AcceptAlgs(["SUSY14KernelSkim"])

### Init
from DerivationFrameworkCore.ThinningHelper import ThinningHelper
SUSY14ThinningHelper = ThinningHelper( "SUSY14ThinningHelper" )
thinningTools       = []
AugmentationTools   = []

# stream-specific sequence for on-the-fly jet building
SeqSUSY14 = CfgMgr.AthSequencer("SeqSUSY14")
DerivationFrameworkJob += SeqSUSY14


#====================================================================
# Trigger navigation thinning
#====================================================================
SUSY14ThinningHelper.TriggerChains = "HLT_xe100"

SUSY14ThinningHelper.AppendToStream( SUSY14Stream )

     
#====================================================================
# THINNING TOOLS 
#====================================================================

# B.M.: likely not used
# TrackParticles directly
#SUSY14TPThinningTool = DerivationFramework__TrackParticleThinning(name = "SUSY14TPThinningTool",
#                                                                 ThinningService         = SUSY14ThinningHelper.ThinningSvc(),
#                                                                 SelectionString         = "InDetTrackParticles.pt > 10*GeV",
#                                                                 InDetTrackParticlesKey  = "InDetTrackParticles")
#ToolSvc += SUSY14TPThinningTool
#thinningTools.append(SUSY14TPThinningTool)

# TrackParticles associated with Muons
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__MuonTrackParticleThinning
SUSY14MuonTPThinningTool = DerivationFramework__MuonTrackParticleThinning(name                    = "SUSY14MuonTPThinningTool",
                                                                         ThinningService         = SUSY14ThinningHelper.ThinningSvc(),
                                                                         MuonKey                 = "Muons",
                                                                         InDetTrackParticlesKey  = "InDetTrackParticles")
ToolSvc += SUSY14MuonTPThinningTool
thinningTools.append(SUSY14MuonTPThinningTool)

# TrackParticles associated with electrons
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__EgammaTrackParticleThinning
SUSY14ElectronTPThinningTool = DerivationFramework__EgammaTrackParticleThinning(name                    	= "SUSY14ElectronTPThinningTool",
                                                                                 ThinningService        = SUSY14ThinningHelper.ThinningSvc(),
                                                                                 SGKey             	= "Electrons",
                                                                                 InDetTrackParticlesKey = "InDetTrackParticles")
ToolSvc += SUSY14ElectronTPThinningTool
thinningTools.append(SUSY14ElectronTPThinningTool)

# TrackParticles associated with photons
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__EgammaTrackParticleThinning
SUSY14PhotonTPThinningTool = DerivationFramework__EgammaTrackParticleThinning(name			 = "SUSY14PhotonTPThinningTool",
										 ThinningService	 = SUSY14ThinningHelper.ThinningSvc(),
										 SGKey			 = "Photons",
										 InDetTrackParticlesKey  = "InDetTrackParticles")
ToolSvc += SUSY14PhotonTPThinningTool
thinningTools.append(SUSY14PhotonTPThinningTool)

# TrackParticles associated with taus
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__TauTrackParticleThinning
SUSY14TauTPThinningTool = DerivationFramework__TauTrackParticleThinning( name            = "SUSY14TauTPThinningTool",
                                                                        ThinningService = SUSY14ThinningHelper.ThinningSvc(),
                                                                        TauKey          = "TauJets",
                                                                        InDetTrackParticlesKey  = "InDetTrackParticles")
ToolSvc += SUSY14TauTPThinningTool
thinningTools.append(SUSY14TauTPThinningTool)

#====================================================================
# TRUTH THINNING
#====================================================================
if DerivationFrameworkIsMonteCarlo:
  from DerivationFrameworkMCTruth.DerivationFrameworkMCTruthConf import DerivationFramework__MenuTruthThinning
  SUSY14TruthThinningTool = DerivationFramework__MenuTruthThinning(name              = "SUSY14TruthThinningTool",
                                                       ThinningService              = SUSY14ThinningHelper.ThinningSvc(),
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

  ToolSvc += SUSY14TruthThinningTool
  thinningTools.append(SUSY14TruthThinningTool)
	

#====================================================================
# SKIMMING TOOL 
#====================================================================

muonsRequirements = '(Muons.pt >= 25.*GeV) && (abs(Muons.eta) < 2.7) && (Muons.DFCommonMuonsPreselection)'
electronsRequirements = '(Electrons.pt > 25.*GeV) && (abs(Electrons.eta) < 2.6) && ((Electrons.Loose) || (Electrons.DFCommonElectronsLHVeryLoose))'
objectSelection = '(count('+electronsRequirements+') + count('+muonsRequirements+') >= 1)'

# now done in ExtendedJetCommon
#applyJetCalibration_xAODColl("AntiKt4EMTopo", SeqSUSY14)

#from DerivationFrameworkSUSY.SUSY14TriggerList import triggersNavThin
from DerivationFrameworkSUSY.SUSY14TriggerList import MetTriggers
MEttrig_expression ='(' + ' || '.join(MetTriggers) + ')' 

LepTrigexpression = '('+MEttrig_expression+'&&'+objectSelection+')'

JetEleExpression = '(count(AntiKt4EMTopoJets.DFCommonJets_Calib_pt>25*GeV && abs(AntiKt4EMTopoJets.DFCommonJets_Calib_eta)<2.8)>=3)'


if DerivationFrameworkIsMonteCarlo:
    JetEleExpression = '(count(AntiKt4EMTopoJets.DFCommonJets_Calib_pt>15*GeV && abs(AntiKt4EMTopoJets.DFCommonJets_Calib_eta)<2.8)>=3)'

expression = '('+LepTrigexpression+'&&('+JetEleExpression+'))'



from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__xAODStringSkimmingTool
SUSY14SkimmingTool = DerivationFramework__xAODStringSkimmingTool( name = "SUSY14SkimmingTool",
                                                                expression = expression)

ToolSvc += SUSY14SkimmingTool

#=======================================
# CREATE THE DERIVATION KERNEL ALGORITHM   
#=======================================

from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__DerivationKernel

# Add sumOfWeights metadata for LHE3 multiweights =======
from DerivationFrameworkCore.LHE3WeightMetadata import *

#==============================================================================
# SUSY signal augmentation (before skimming!)
#==============================================================================
from DerivationFrameworkSUSY.DecorateSUSYProcess import IsSUSYSignal
if IsSUSYSignal():
   
   from DerivationFrameworkSUSY.DecorateSUSYProcess import DecorateSUSYProcess
   SeqSUSY14 += CfgMgr.DerivationFramework__DerivationKernel("SUSY14KernelSigAug",
                                                            AugmentationTools = DecorateSUSYProcess("SUSY14")
                                                            )
   
   from DerivationFrameworkSUSY.SUSYWeightMetadata import *


#==============================================================================
# SUSY skimming selection
#==============================================================================
SeqSUSY14 += CfgMgr.DerivationFramework__DerivationKernel(
  "SUSY14KernelSkim",
  SkimmingTools = [SUSY14SkimmingTool]
)


#==============================================================================
# Jet building
#==============================================================================
#re-tag PFlow jets so they have b-tagging info.
FlavorTagInit(JetCollections = ['AntiKt4EMPFlowJets'], Sequencer = SeqSUSY14)

#==============================================================================
OutputJets["SUSY14"] = []
reducedJetList = [ "AntiKt2PV0TrackJets", "AntiKt4PV0TrackJets", "AntiKt10LCTopoJets"]
# now part of MCTruthCommon
#if DerivationFrameworkIsMonteCarlo:
#  reducedJetList += [ "AntiKt4TruthJets", "AntiKt4TruthWZJets", "AntiKt10TruthJets" ]

# AntiKt2PV0TrackJets is flavour-tagged automatically
replaceAODReducedJets(reducedJetList, SeqSUSY14, "SUSY14")

# AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets
addDefaultTrimmedJets(SeqSUSY14, "SUSY14")


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
SeqSUSY14 += CfgMgr.DerivationFramework__DerivationKernel(
	"SUSY14KernelAug",
	AugmentationTools = AugmentationTools,
	ThinningTools = thinningTools,
)


#====================================================================
# CONTENT LIST  
#====================================================================
from DerivationFrameworkCore.SlimmingHelper import SlimmingHelper
SUSY14SlimmingHelper = SlimmingHelper("SUSY14SlimmingHelper")
SUSY14SlimmingHelper.SmartCollections = ["Electrons","Photons","Muons","TauJets","AntiKt4EMTopoJets",
"AntiKt4EMPFlowJets",
"MET_Reference_AntiKt4EMTopo",
"MET_Reference_AntiKt4EMPFlow",
"BTagging_AntiKt4EMTopo",
"BTagging_AntiKt4EMPFlow",
 "InDetTrackParticles", "PrimaryVertices", "AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets"]
SUSY14SlimmingHelper.AllVariables = ["TruthParticles", "TruthEvents", "TruthVertices", "MET_Truth", "MET_Track"]
SUSY14SlimmingHelper.ExtraVariables = ["BTagging_AntiKt4EMTopo.MV1_discriminant.MV1c_discriminant",
                                      "Muons.ptcone30.ptcone20.charge.quality.InnerDetectorPt.MuonSpectrometerPt.CaloLRLikelihood.CaloMuonIDTag",
                                      "Photons.author.Loose.Tight",
                                      "AntiKt4EMTopoJets.NumTrkPt1000.TrackWidthPt1000.NumTrkPt500.DFCommonJets_Calib_pt.DFCommonJets_Calib_eta.DFCommonJets_Calib_phi",
                                      "GSFTrackParticles.z0.d0.vz.definingParametersCovMatrix","CombinedMuonTrackParticles.d0.z0.vz.definingParametersCovMatrix.truthOrigin.truthType",
                                      "ExtrapolatedMuonTrackParticles.d0.z0.vz.definingParametersCovMatrix.truthOrigin.truthType",
                                      "TauJets.IsTruthMatched.truthOrigin.truthType.truthParticleLink.truthJetLink.DFCommonTausLoose"
                                      + ".PanTau_isPanTauCandidate.ptPanTauCellBased.etaPanTauCellBased.phiPanTauCellBased.mPanTauCellBased",
                                      "TauNeutralParticleFlowObjects.pt.eta.phi.m",
                                      "TauChargedParticleFlowObjects.pt.eta.phi.m",
                                      "MuonTruthParticles.barcode.decayVtxLink.e.m.pdgId.prodVtxLink.px.py.pz.recoMuonLink.status.truthOrigin.truthType",
                                      "AntiKt4TruthJets.eta.m.phi.pt.TruthLabelDeltaR_B.TruthLabelDeltaR_C.TruthLabelDeltaR_T.TruthLabelID.ConeTruthLabelID.PartonTruthLabelID",
                                      "Electrons.bkgTruthType.bkgTruthOrigin",
#P. Pani removed 20/06/16                                      "AntiKt3PV0TrackJets.eta.m.phi.pt.btagging.btaggingLink",
#P. Pani removed 20/06/16                                      "BTagging_AntiKt3Track.MV2c20_discriminant",
                                      "AntiKt2PV0TrackJets.eta.m.phi.pt.btagging.btaggingLink",
                                      "BTagging_AntiKt2Track.MV2c10_discriminant",
                                   ]

SUSY14SlimmingHelper.IncludeMuonTriggerContent   = True
SUSY14SlimmingHelper.IncludeEGammaTriggerContent = True
SUSY14SlimmingHelper.IncludeBPhysTriggerContent  = False
SUSY14SlimmingHelper.IncludeJetTriggerContent    = False
SUSY14SlimmingHelper.IncludeTauTriggerContent    = False
SUSY14SlimmingHelper.IncludeEtMissTriggerContent = True
SUSY14SlimmingHelper.IncludeBJetTriggerContent   = False

# PP removed 22/04/2016
#addJetOutputs(SUSY14SlimmingHelper,["LargeR", "SUSY14"], [], ["CamKt12LCTopoJets","AntiKt10LCTopoJets","AntiKt10TruthJets","CamKt12TruthWZJets","CamKt12TruthJets","AntiKt10TruthWZJets"])

# Most of the new containers are centrally added to SlimmingHelper via DerivationFrameworkCore ContainersOnTheFly.py
SUSY14SlimmingHelper.AppendToDictionary = {'BTagging_AntiKt4EMPFlow':'xAOD::BTaggingContainer','BTagging_AntiKt4EMPFlowAux':'xAOD::BTaggingAuxContainer',
'AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets':'xAOD::JetContainer','AntiKt10LCTopoTrimmedPtFrac5SmallR20JetsAux':'xAOD::JetAuxContainer',
                                           'TruthTop':'xAOD::TruthParticleContainer','TruthTopAux':'xAOD::TruthParticleAuxContainer',
                                           'TruthBSM':'xAOD::TruthParticleContainer','TruthBSMAux':'xAOD::TruthParticleAuxContainer',
                                           'TruthBoson':'xAOD::TruthParticleContainer','TruthBosonAux':'xAOD::TruthParticleAuxContainer'}

# All standard truth particle collections are provided by DerivationFrameworkMCTruth (TruthDerivationTools.py)
if DerivationFrameworkIsMonteCarlo:
  SUSY14SlimmingHelper.AllVariables += ["TruthElectrons", "TruthMuons", "TruthTaus", "TruthPhotons", "TruthNeutrinos", "TruthTop", "TruthBSM", "TruthBoson"]   


SUSY14SlimmingHelper.AppendContentToStream(SUSY14Stream)

