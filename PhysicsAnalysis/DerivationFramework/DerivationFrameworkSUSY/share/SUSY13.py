#********************************************************************
# SUSY13.py 
# reductionConf flag SUSY13 in Reco_tf.py   
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
streamName = derivationFlags.WriteDAOD_SUSY13Stream.StreamName
fileName   = buildFileName( derivationFlags.WriteDAOD_SUSY13Stream )
SUSY13Stream = MSMgr.NewPoolRootStream( streamName, fileName )
SUSY13Stream.AcceptAlgs(["SUSY13KernelSkim"])

### Init
from DerivationFrameworkCore.ThinningHelper import ThinningHelper
SUSY13ThinningHelper = ThinningHelper( "SUSY13ThinningHelper" )
thinningTools       = []
AugmentationTools   = []

# stream-specific sequence for on-the-fly jet building
SeqSUSY13 = CfgMgr.AthSequencer("SeqSUSY13")
DerivationFrameworkJob += SeqSUSY13


#====================================================================
# Trigger navigation thinning
#====================================================================
from DerivationFrameworkSUSY.SUSY13TriggerList import triggersNavThin
SUSY13ThinningHelper.TriggerChains = '|'.join(triggersNavThin)

SUSY13ThinningHelper.AppendToStream( SUSY13Stream )

     
#====================================================================
# THINNING TOOLS 
#====================================================================

# B.M.: likely not used
# TrackParticles directly
#SUSY13TPThinningTool = DerivationFramework__TrackParticleThinning(name = "SUSY13TPThinningTool",
#                                                                 ThinningService         = SUSY13ThinningHelper.ThinningSvc(),
#                                                                 SelectionString         = "InDetTrackParticles.pt > 10*GeV",
#                                                                 InDetTrackParticlesKey  = "InDetTrackParticles")
#ToolSvc += SUSY13TPThinningTool
#thinningTools.append(SUSY13TPThinningTool)

# TrackParticles associated with Muons
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__MuonTrackParticleThinning
SUSY13MuonTPThinningTool = DerivationFramework__MuonTrackParticleThinning(name                    = "SUSY13MuonTPThinningTool",
                                                                         ThinningService         = SUSY13ThinningHelper.ThinningSvc(),
                                                                         MuonKey                 = "Muons",
                                                                         InDetTrackParticlesKey  = "InDetTrackParticles")
ToolSvc += SUSY13MuonTPThinningTool
thinningTools.append(SUSY13MuonTPThinningTool)

# TrackParticles associated with electrons
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__EgammaTrackParticleThinning
SUSY13ElectronTPThinningTool = DerivationFramework__EgammaTrackParticleThinning(name                    	= "SUSY13ElectronTPThinningTool",
                                                                                 ThinningService        = SUSY13ThinningHelper.ThinningSvc(),
                                                                                 SGKey             	= "Electrons",
                                                                                 InDetTrackParticlesKey = "InDetTrackParticles")
ToolSvc += SUSY13ElectronTPThinningTool
thinningTools.append(SUSY13ElectronTPThinningTool)

# TrackParticles associated with photons
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__EgammaTrackParticleThinning
SUSY13PhotonTPThinningTool = DerivationFramework__EgammaTrackParticleThinning(name			 = "SUSY13PhotonTPThinningTool",
										 ThinningService	 = SUSY13ThinningHelper.ThinningSvc(),
										 SGKey			 = "Photons",
										 InDetTrackParticlesKey  = "InDetTrackParticles")
ToolSvc += SUSY13PhotonTPThinningTool
thinningTools.append(SUSY13PhotonTPThinningTool)

# TrackParticles associated with taus
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__TauTrackParticleThinning
SUSY13TauTPThinningTool = DerivationFramework__TauTrackParticleThinning( name            = "SUSY13TauTPThinningTool",
                                                                        ThinningService = SUSY13ThinningHelper.ThinningSvc(),
                                                                        TauKey          = "TauJets",
                                                                        InDetTrackParticlesKey  = "InDetTrackParticles")
ToolSvc += SUSY13TauTPThinningTool
thinningTools.append(SUSY13TauTPThinningTool)

#====================================================================
# TRUTH THINNING
#====================================================================
if DerivationFrameworkIsMonteCarlo:
  from DerivationFrameworkMCTruth.DerivationFrameworkMCTruthConf import DerivationFramework__MenuTruthThinning
  SUSY13TruthThinningTool = DerivationFramework__MenuTruthThinning(name              = "SUSY13TruthThinningTool",
                                                       ThinningService              = SUSY13ThinningHelper.ThinningSvc(),
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

  ToolSvc += SUSY13TruthThinningTool
  thinningTools.append(SUSY13TruthThinningTool)
	
#====================================================================
# SKIMMING TOOL 
#====================================================================

#=======
# Triggers for skimming
#=======

photonRequirements = '(count((DFCommonPhotons_et > 100*GeV) && (abs(DFCommonPhotons_eta) < 2.6)) > 0)'
from DerivationFrameworkSUSY.SUSY13TriggerList import triggersNavThin
trig_expression = '(' + ' || '.join(triggersNavThin) + ')' 
expression = '('+photonRequirements+'&&'+trig_expression+')'
print "DEBUG PPPPP full", expression

from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__xAODStringSkimmingTool
SUSY13SkimmingTool = DerivationFramework__xAODStringSkimmingTool( name = "SUSY13SkimmingTool",
                                                                expression = expression)

ToolSvc += SUSY13SkimmingTool

#=======================================
# CREATE THE DERIVATION KERNEL ALGORITHM   
#=======================================
# now done in ExtendedJetCommon
#applyJetCalibration_xAODColl("AntiKt4EMTopo", SeqSUSY13)

from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__DerivationKernel

# Add sumOfWeights metadata for LHE3 multiweights =======
from DerivationFrameworkCore.LHE3WeightMetadata import *

#==============================================================================
# SUSY signal augmentation (before skimming!)
#==============================================================================
from DerivationFrameworkSUSY.DecorateSUSYProcess import IsSUSYSignal
if IsSUSYSignal():
   
   from DerivationFrameworkSUSY.DecorateSUSYProcess import DecorateSUSYProcess
   SeqSUSY13 += CfgMgr.DerivationFramework__DerivationKernel("SUSY13KernelSigAug",
                                                            AugmentationTools = DecorateSUSYProcess("SUSY13")
                                                            )
   
   from DerivationFrameworkSUSY.SUSYWeightMetadata import *


#==============================================================================
# SUSY skimming selection
#==============================================================================
SeqSUSY13 += CfgMgr.DerivationFramework__DerivationKernel(
  "SUSY13KernelSkim",
  SkimmingTools = [SUSY13SkimmingTool]
)


#==============================================================================
# Jet building
#==============================================================================
#re-tag PFlow jets so they have b-tagging info.
FlavorTagInit(JetCollections = ['AntiKt4EMPFlowJets'], Sequencer = SeqSUSY13)

#==============================================================================
# now part of MCTruthCommon
#if DerivationFrameworkIsMonteCarlo:
#
#  OutputJets["SUSY13"] = []
#  reducedJetList = [ "AntiKt4TruthJets", "AntiKt4TruthWZJets" ]
#
#  replaceAODReducedJets(reducedJetList, SeqSUSY13, "SUSY13")


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
SeqSUSY13 += CfgMgr.DerivationFramework__DerivationKernel(
	"SUSY13KernelAug",
	AugmentationTools = AugmentationTools,
	ThinningTools = thinningTools,
)


#====================================================================
# CONTENT LIST  
#====================================================================
# This might be the kind of set-up one would have for a muon based analysis
from DerivationFrameworkCore.SlimmingHelper import SlimmingHelper
SUSY13SlimmingHelper = SlimmingHelper("SUSY13SlimmingHelper")
SUSY13SlimmingHelper.SmartCollections = ["Electrons","Photons","Muons","TauJets","AntiKt4EMTopoJets",
"AntiKt4EMPFlowJets",
"MET_Reference_AntiKt4EMTopo",
"MET_Reference_AntiKt4EMPFlow",
"BTagging_AntiKt4EMTopo",
"BTagging_AntiKt4EMPFlow",
 "InDetTrackParticles", "PrimaryVertices"]
SUSY13SlimmingHelper.AllVariables = ["TruthParticles", "TruthEvents", "TruthVertices", "MET_Truth", "MET_Track"]
SUSY13SlimmingHelper.ExtraVariables = ["BTagging_AntiKt4EMTopo.MV1_discriminant.MV1c_discriminant",
                "Muons.ptcone30.ptcone20.charge.quality.InnerDetectorPt.MuonSpectrometerPt.CaloLRLikelihood.CaloMuonIDTag",
                "Photons.author.Loose.Tight",
                "AntiKt4EMTopoJets.NumTrkPt1000.TrackWidthPt1000.NumTrkPt500.DFCommonJets_Calib_pt.DFCommonJets_Calib_eta.DFCommonJets_Calib_phi",
                "GSFTrackParticles.z0.d0.vz.definingParametersCovMatrix","CombinedMuonTrackParticles.d0.z0.vz.definingParametersCovMatrix.truthOrigin.truthType",
                "ExtrapolatedMuonTrackParticles.d0.z0.vz.definingParametersCovMatrix.truthOrigin.truthType",
                "TauJets.IsTruthMatched.truthOrigin.truthType.truthParticleLink.truthJetLink",
                "MuonTruthParticles.barcode.decayVtxLink.e.m.pdgId.prodVtxLink.px.py.pz.recoMuonLink.status.truthOrigin.truthType",
                "AntiKt4TruthJets.eta.m.phi.pt.TruthLabelDeltaR_B.TruthLabelDeltaR_C.TruthLabelDeltaR_T.TruthLabelID.ConeTruthLabelID.PartonTruthLabelID",
                "Electrons.bkgTruthType.bkgTruthOrigin",
#P. Pani removed 20/06/16                "AntiKt3PV0TrackJets.eta.m.phi.pt.btagging.btaggingLink",
#P. Pani removed 20/06/16                "BTagging_AntiKt3Track.MV2c20_discriminant"
                                       ]

SUSY13SlimmingHelper.IncludeMuonTriggerContent   = False
SUSY13SlimmingHelper.IncludeEGammaTriggerContent = True
SUSY13SlimmingHelper.IncludeBPhysTriggerContent  = False
SUSY13SlimmingHelper.IncludeJetTriggerContent    = False
SUSY13SlimmingHelper.IncludeTauTriggerContent    = False
SUSY13SlimmingHelper.IncludeEtMissTriggerContent = False
SUSY13SlimmingHelper.IncludeBJetTriggerContent   = False

#addJetOutputs(SUSY13SlimmingHelper,["LargeR", "SUSY13"], [], ["CamKt12LCTopoJets","AntiKt10LCTopoJets","AntiKt10TruthJets","CamKt12TruthWZJets","CamKt12TruthJets","AntiKt10TruthWZJets"])
#addJetOutputs(slimhelper,contentlist,t0list_smart=[],vetolist=[]):
# Add the MET containers to the stream
# JRC: TEMPORARILY COMMENTED
#addMETOutputs(SUSY13Stream,["SUSY"])

# All standard truth particle collections are provided by DerivationFrameworkMCTruth (TruthDerivationTools.py)
# Most of the new containers are centrally added to SlimmingHelper via DerivationFrameworkCore ContainersOnTheFly.py
if DerivationFrameworkIsMonteCarlo:

  SUSY13SlimmingHelper.AppendToDictionary = {'BTagging_AntiKt4EMPFlow':'xAOD::BTaggingContainer','BTagging_AntiKt4EMPFlowAux':'xAOD::BTaggingAuxContainer',
'TruthTop':'xAOD::TruthParticleContainer','TruthTopAux':'xAOD::TruthParticleAuxContainer',
                                             'TruthBSM':'xAOD::TruthParticleContainer','TruthBSMAux':'xAOD::TruthParticleAuxContainer',
                                             'TruthBoson':'xAOD::TruthParticleContainer','TruthBosonAux':'xAOD::TruthParticleAuxContainer'}
  
  SUSY13SlimmingHelper.AllVariables += ["TruthElectrons", "TruthMuons", "TruthTaus", "TruthPhotons", "TruthNeutrinos", "TruthTop", "TruthBSM", "TruthBoson"]   


SUSY13SlimmingHelper.AppendContentToStream(SUSY13Stream)

