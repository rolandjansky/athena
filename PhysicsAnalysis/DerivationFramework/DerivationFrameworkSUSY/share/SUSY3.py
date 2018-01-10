#********************************************************************
# SUSY3.py
# reductionConf flag SUSY3 in Reco_tf.py   
#********************************************************************

from DerivationFrameworkCore.DerivationFrameworkMaster import *
from DerivationFrameworkJetEtMiss.JetCommon import *
from DerivationFrameworkJetEtMiss.ExtendedJetCommon import *
from DerivationFrameworkEGamma.EGammaCommon import *
from DerivationFrameworkMuons.MuonsCommon import *
from DerivationFrameworkTau.TauCommon import *
if DerivationFrameworkIsMonteCarlo:
  from DerivationFrameworkMCTruth.MCTruthCommon import addStandardTruthContents
  addStandardTruthContents()
from DerivationFrameworkInDet.InDetCommon import *
from DerivationFrameworkJetEtMiss.METCommon import *
from DerivationFrameworkFlavourTag.FlavourTagCommon import *


### Set up stream
streamName = derivationFlags.WriteDAOD_SUSY3Stream.StreamName
fileName   = buildFileName( derivationFlags.WriteDAOD_SUSY3Stream )
SUSY3Stream = MSMgr.NewPoolRootStream( streamName, fileName )
SUSY3Stream.AcceptAlgs(["SUSY3KernelSkim"])

### Init
from DerivationFrameworkCore.ThinningHelper import ThinningHelper
SUSY3ThinningHelper = ThinningHelper( "SUSY3ThinningHelper" )
thinningTools       = []
AugmentationTools   = []

# stream-specific sequence for on-the-fly jet building
SeqSUSY3 = CfgMgr.AthSequencer("SeqSUSY3")
DerivationFrameworkJob += SeqSUSY3


#====================================================================
# Trigger navigation thinning
#====================================================================
from DerivationFrameworkSUSY.SUSY3TriggerList import triggerRegEx, triggerRegExThinning
SUSY3ThinningHelper.TriggerChains = '|'.join(triggerRegExThinning)

from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__TriggerSkimmingTool
SUSY3TriggerSkimmingTool = DerivationFramework__TriggerSkimmingTool( 
    name          = "SUSY3TriggerSkimmingTool",
    TriggerListOR = triggerRegEx )
ToolSvc += SUSY3TriggerSkimmingTool

SUSY3ThinningHelper.AppendToStream( SUSY3Stream ) # needs to go after SUSY3ThinningHelper.TriggerChains = ... but before accesses to SUSY3ThinningHelper.ThinningSvc()


#====================================================================
# THINNING TOOLS 
#====================================================================

#from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__TrackParticleThinning

# TrackParticles directly
#SUSY3TPThinningTool = DerivationFramework__TrackParticleThinning(name = "SUSY3TPThinningTool",
#                                                                 ThinningService         = SUSY3ThinningHelper.ThinningSvc(),
#                                                                 SelectionString         = "InDetTrackParticles.pt > 10*GeV",
#                                                                 InDetTrackParticlesKey  = "InDetTrackParticles")
#ToolSvc += SUSY3TPThinningTool
#thinningTools.append(SUSY3TPThinningTool)

# TrackParticles associated with Muons
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__MuonTrackParticleThinning
SUSY3MuonTPThinningTool = DerivationFramework__MuonTrackParticleThinning(name                    = "SUSY3MuonTPThinningTool",
                                                                         ThinningService         = SUSY3ThinningHelper.ThinningSvc(),
                                                                         MuonKey                 = "Muons",
                                                                         InDetTrackParticlesKey  = "InDetTrackParticles")
ToolSvc += SUSY3MuonTPThinningTool
thinningTools.append(SUSY3MuonTPThinningTool)

# TrackParticles associated with electrons
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__EgammaTrackParticleThinning
SUSY3ElectronTPThinningTool = DerivationFramework__EgammaTrackParticleThinning(	name                    = "SUSY3ElectronTPThinningTool",
                                                                                ThinningService         = SUSY3ThinningHelper.ThinningSvc(),
                                                                                SGKey             	= "Electrons",
                                                                                InDetTrackParticlesKey  = "InDetTrackParticles")
ToolSvc += SUSY3ElectronTPThinningTool
thinningTools.append(SUSY3ElectronTPThinningTool)

# TrackParticles associated with photons
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__EgammaTrackParticleThinning
SUSY3PhotonTPThinningTool = DerivationFramework__EgammaTrackParticleThinning(name			 = "SUSY3PhotonTPThinningTool",
										 ThinningService	 = SUSY3ThinningHelper.ThinningSvc(),
										 SGKey			 = "Photons",
										 InDetTrackParticlesKey  = "InDetTrackParticles")
ToolSvc += SUSY3PhotonTPThinningTool
thinningTools.append(SUSY3PhotonTPThinningTool)

# TrackParticles associated with taus
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__TauTrackParticleThinning
SUSY3TauTPThinningTool = DerivationFramework__TauTrackParticleThinning( name            = "SUSY3TauTPThinningTool",
                                                                        ThinningService = SUSY3ThinningHelper.ThinningSvc(),
                                                                        TauKey          = "TauJets",
                                                                        InDetTrackParticlesKey  = "InDetTrackParticles")
ToolSvc += SUSY3TauTPThinningTool
thinningTools.append(SUSY3TauTPThinningTool)

#====================================================================
# TRUTH THINNING
#====================================================================
if DerivationFrameworkIsMonteCarlo:
  from DerivationFrameworkMCTruth.DerivationFrameworkMCTruthConf import DerivationFramework__MenuTruthThinning
  SUSY3TruthThinningTool = DerivationFramework__MenuTruthThinning(name              = "SUSY3TruthThinningTool",
                                                       ThinningService              = SUSY3ThinningHelper.ThinningSvc(),
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
                                                       WriteStatus3                 = True,
                                                       WriteFirstN                  = -1,
                                                       PreserveAncestors            = True,
                                                       PreserveGeneratorDescendants = False,
                                                       SimBarcodeOffset             = DerivationFrameworkSimBarcodeOffset)


  # Decorate Electron with bkg electron type/origin
  from MCTruthClassifier.MCTruthClassifierBase import MCTruthClassifier as BkgElectronMCTruthClassifier   
  from DerivationFrameworkEGamma.DerivationFrameworkEGammaConf import DerivationFramework__BkgElectronClassification
  BkgElectronClassificationTool = DerivationFramework__BkgElectronClassification (name = "BkgElectronClassificationTool", MCTruthClassifierTool = BkgElectronMCTruthClassifier)
  ToolSvc += BkgElectronClassificationTool
  AugmentationTools.append(BkgElectronClassificationTool)

  ToolSvc += SUSY3TruthThinningTool
  thinningTools.append(SUSY3TruthThinningTool)


#====================================================================
# SKIMMING TOOL 
#====================================================================

TauRequirements = '(abs(TauJets.eta) < 2.6) && (TauJets.nTracks == 1 || TauJets.nTracks == 3) && (TauJets.DFCommonTausLoose) && (TauJets.ptFinalCalib >= 15.*GeV)'

objectSelection = 'count('+TauRequirements+') >= 1'

from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__xAODStringSkimmingTool
SUSY3SkimmingTool = DerivationFramework__xAODStringSkimmingTool( name = "SUSY3SkimmingTool",
                                                                expression = objectSelection)

ToolSvc += SUSY3SkimmingTool


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
   SeqSUSY3 += CfgMgr.DerivationFramework__DerivationKernel("SUSY3KernelSigAug",
                                                            AugmentationTools = DecorateSUSYProcess("SUSY3")
                                                            )
   
   from DerivationFrameworkSUSY.SUSYWeightMetadata import *


#==============================================================================
# SUSY skimming selection
#==============================================================================
SeqSUSY3 += CfgMgr.DerivationFramework__DerivationKernel(
    "SUSY3KernelSkim",
    SkimmingTools = [SUSY3SkimmingTool, SUSY3TriggerSkimmingTool]
    )


#==============================================================================
# Jet building
#==============================================================================
#re-tag PFlow jets so they have b-tagging info.
FlavorTagInit(JetCollections = ['AntiKt4EMPFlowJets'], Sequencer = SeqSUSY3)

#==============================================================================
# now part of MCTruthCommon
#if DerivationFrameworkIsMonteCarlo:
#
#  OutputJets["SUSY3"] = []
#  reducedJetList = [ "AntiKt4TruthJets", "AntiKt4TruthWZJets" ]
#
#  replaceAODReducedJets(reducedJetList, SeqSUSY3, "SUSY3")


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

SeqSUSY3 += CfgMgr.DerivationFramework__DerivationKernel(
	"SUSY3KernelAug",
	AugmentationTools = AugmentationTools,
	ThinningTools = thinningTools,
)


#====================================================================
# CONTENT LIST  
#====================================================================
from DerivationFrameworkCore.SlimmingHelper import SlimmingHelper
SUSY3SlimmingHelper = SlimmingHelper("SUSY3SlimmingHelper")
SUSY3SlimmingHelper.SmartCollections = ["Electrons","Photons","Muons","MET_Reference_AntiKt4EMTopo",
"MET_Reference_AntiKt4EMPFlow",
"TauJets","AntiKt4EMTopoJets",
"AntiKt4EMPFlowJets",
 "BTagging_AntiKt4EMTopo",
"BTagging_AntiKt4EMPFlow",
 "InDetTrackParticles", "PrimaryVertices"]
SUSY3SlimmingHelper.AllVariables = ["TruthParticles", "TruthEvents", "TruthVertices", "MET_Truth", "MET_Track"]
SUSY3SlimmingHelper.ExtraVariables = ["BTagging_AntiKt4EMTopo.MV1_discriminant.MV1c_discriminant",
                                      "Electrons.author.charge.ptcone20.truthOrigin.truthType.bkgMotherPdgId.bkgTruthOrigin.bkgTruthType.firstEgMotherTruthType.firstEgMotherTruthOrigin.firstEgMotherPdgId",
                                      "Muons.ptcone30.ptcone20.charge.quality.InnerDetectorPt.MuonSpectrometerPt.CaloLRLikelihood.CaloMuonIDTag",
                                      "Photons.author.Loose.Tight",
                                      "AntiKt4EMTopoJets.NumTrkPt1000.TrackWidthPt1000.NumTrkPt500.N90Constituents.Timing.Width",
                                      "GSFTrackParticles.z0.d0.vz.definingParametersCovMatrix.truthOrigin.truthType",
                                      "InDetTrackParticles.truthOrigin.truthType",
                                      "CombinedMuonTrackParticles.d0.z0.vz.definingParametersCovMatrix.truthOrigin.truthType",
                                      "ExtrapolatedMuonTrackParticles.d0.z0.vz.definingParametersCovMatrix.truthOrigin.truthType",
                                      "TauJets.IsTruthMatched.truthParticleLink.truthOrigin.truthType.truthJetLink.DFCommonTausLoose",
                                      "MuonTruthParticles.barcode.decayVtxLink.e.m.pdgId.prodVtxLink.px.py.pz.recoMuonLink.status.truthOrigin.truthType",
                                      "AntiKt4TruthJets.eta.m.phi.pt.TruthLabelDeltaR_B.TruthLabelDeltaR_C.TruthLabelDeltaR_T.TruthLabelID.ConeTruthLabelID.PartonTruthLabelID.HadronConeExclTruthLabelID",
                                      "HLT_xAOD__JetContainer_SplitJet.pt.eta.phi.m",
                                      "HLT_xAOD__BTaggingContainer_HLTBjetFex.MV2c20_discriminant.MV2c10_discriminant",
                                      "HLT_xAOD__TrigMissingETContainer_TrigEFMissingET_mht.ex.ey"]
SUSY3SlimmingHelper.IncludeMuonTriggerContent = True
SUSY3SlimmingHelper.IncludeEGammaTriggerContent = True
#SUSY3SlimmingHelper.IncludeBPhysTriggerContent = True 
#SUSY3SlimmingHelper.IncludeJetTauEtMissTriggerContent = True
SUSY3SlimmingHelper.IncludeJetTriggerContent = True
SUSY3SlimmingHelper.IncludeTauTriggerContent = True
SUSY3SlimmingHelper.IncludeEtMissTriggerContent = True
SUSY3SlimmingHelper.IncludeBJetTriggerContent = False


# All standard truth particle collections are provided by DerivationFrameworkMCTruth (TruthDerivationTools.py)
# Most of the new containers are centrally added to SlimmingHelper via DerivationFrameworkCore ContainersOnTheFly.py
if DerivationFrameworkIsMonteCarlo:

  SUSY3SlimmingHelper.AppendToDictionary = {'BTagging_AntiKt4EMPFlow':'xAOD::BTaggingContainer','BTagging_AntiKt4EMPFlowAux':'xAOD::BTaggingAuxContainer',
'TruthTop':'xAOD::TruthParticleContainer','TruthTopAux':'xAOD::TruthParticleAuxContainer',
                                            'TruthBSM':'xAOD::TruthParticleContainer','TruthBSMAux':'xAOD::TruthParticleAuxContainer',
                                            'TruthBoson':'xAOD::TruthParticleContainer','TruthBosonAux':'xAOD::TruthParticleAuxContainer'}
  
  SUSY3SlimmingHelper.AllVariables += ["TruthElectrons", "TruthMuons", "TruthTaus", "TruthPhotons", "TruthNeutrinos", "TruthTop", "TruthBSM", "TruthBoson"]   
 
SUSY3SlimmingHelper.AppendContentToStream(SUSY3Stream)
