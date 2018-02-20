#********************************************************************
# SUSY15.py
# reductionConf flag SUSY15 in Reco_tf.py  
#********************************************************************

# TODO: check what of this is still needed /CO
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
streamName = derivationFlags.WriteDAOD_SUSY15Stream.StreamName
fileName   = buildFileName( derivationFlags.WriteDAOD_SUSY15Stream )
SUSY15Stream = MSMgr.NewPoolRootStream( streamName, fileName )
SUSY15Stream.AcceptAlgs(["SUSY15KernelSkim"])

### Init
from DerivationFrameworkCore.ThinningHelper import ThinningHelper
SUSY15ThinningHelper = ThinningHelper( "SUSY15ThinningHelper" )
thinningTools       = []
AugmentationTools   = []

# stream-specific sequence for on-the-fly jet building
SeqSUSY15 = CfgMgr.AthSequencer("SeqSUSY15")
DerivationFrameworkJob += SeqSUSY15

from DerivationFrameworkSUSY.SUSY15TriggerList import triggers_met, triggers_jet, triggers_lep

triggers = triggers_met + triggers_jet + triggers_lep

#SUSY15ThinningHelper.TriggerChains = '(' + ' | '.join(triggers) + ')' #triggerRegEx

# for now adding an unconfigured thinning helper (?) /CO
SUSY15ThinningHelper.AppendToStream( SUSY15Stream )

### Thinning for now taken from SUSY 2 with minor modifications as indicated /CO


#====================================================================
# THINNING TOOLS
#====================================================================

# MET/Jet tracks -> no longer needed, 11.05.2015

from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__TrackParticleThinning

# TrackParticles directly
SUSY15TPThinningTool = DerivationFramework__TrackParticleThinning(name = "SUSY15TPThinningTool",
                                                                 ThinningService         = SUSY15ThinningHelper.ThinningSvc(),
                                                                 SelectionString         = "InDetTrackParticles.pt > 1*GeV",
                                                                 InDetTrackParticlesKey  = "InDetTrackParticles")
ToolSvc += SUSY15TPThinningTool
thinningTools.append(SUSY15TPThinningTool)

# TrackParticles associated with Muons
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__MuonTrackParticleThinning
SUSY15MuonTPThinningTool = DerivationFramework__MuonTrackParticleThinning(name                   = "SUSY15MuonTPThinningTool",
                                                                         ThinningService         = SUSY15ThinningHelper.ThinningSvc(),
                                                                         MuonKey                 = "Muons",
                                                                         InDetTrackParticlesKey  = "InDetTrackParticles")
ToolSvc += SUSY15MuonTPThinningTool
thinningTools.append(SUSY15MuonTPThinningTool)

# TrackParticles associated with electrons
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__EgammaTrackParticleThinning
SUSY15ElectronTPThinningTool = DerivationFramework__EgammaTrackParticleThinning(name                 	= "SUSY15ElectronTPThinningTool",
                                                                                 ThinningService        = SUSY15ThinningHelper.ThinningSvc(),
                                                                                 SGKey             	= "Electrons",
                                                                                 BestMatchOnly          = False,
                                                                                 InDetTrackParticlesKey = "InDetTrackParticles")
ToolSvc += SUSY15ElectronTPThinningTool
thinningTools.append(SUSY15ElectronTPThinningTool)
# TrackParticles associated with photons
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__EgammaTrackParticleThinning
SUSY15PhotonTPThinningTool = DerivationFramework__EgammaTrackParticleThinning(name			 = "SUSY15PhotonTPThinningTool",
										 ThinningService	 = SUSY15ThinningHelper.ThinningSvc(),
										 SGKey			 = "Photons",
										 InDetTrackParticlesKey  = "InDetTrackParticles")
ToolSvc += SUSY15PhotonTPThinningTool
thinningTools.append(SUSY15PhotonTPThinningTool)

# TrackParticles associated with taus
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__TauTrackParticleThinning
SUSY15TauTPThinningTool = DerivationFramework__TauTrackParticleThinning( name           = "SUSY15TauTPThinningTool",
                                                                        ThinningService = SUSY15ThinningHelper.ThinningSvc(),
                                                                        TauKey          = "TauJets",
                                                                        InDetTrackParticlesKey = "InDetTrackParticles")
ToolSvc += SUSY15TauTPThinningTool
thinningTools.append(SUSY15TauTPThinningTool)


#====================================================================
# TRUTH THINNING
#====================================================================
if DerivationFrameworkIsMonteCarlo:

  from DerivationFrameworkMCTruth.DerivationFrameworkMCTruthConf import DerivationFramework__MenuTruthThinning
  SUSY15TruthThinningTool = DerivationFramework__MenuTruthThinning(name             = "SUSY15TruthThinningTool",
                                                       ThinningService              = SUSY15ThinningHelper.ThinningSvc(),
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
                                                       WriteEverything              = True,
                                                       WriteAllLeptons              = True,
                                                       WriteLeptonsNotFromHadrons   = False,
                                                       WriteStatus3                 = False,
                                                       WriteFirstN                  = -1,
                                                       PreserveAncestors            = True,
                                                       PreserveGeneratorDescendants = False,
                                                       SimBarcodeOffset             = DerivationFrameworkSimBarcodeOffset)
  ToolSvc += SUSY15TruthThinningTool
  thinningTools.append(SUSY15TruthThinningTool)

  # Decorate Electron with bkg electron type/origin
  from MCTruthClassifier.MCTruthClassifierBase import MCTruthClassifier as BkgElectronMCTruthClassifier
  from DerivationFrameworkEGamma.DerivationFrameworkEGammaConf import DerivationFramework__BkgElectronClassification
  BkgElectronClassificationTool = DerivationFramework__BkgElectronClassification (name = "BkgElectronClassificationTool", MCTruthClassifierTool = BkgElectronMCTruthClassifier)
  ToolSvc += BkgElectronClassificationTool
  AugmentationTools.append(BkgElectronClassificationTool)

  # --- Decorate Truth Particles with some the track parameters:
  # --- The decorator tool (needed by the decorator)
  from InDetPhysValMonitoring.InDetPhysValMonitoringConf import InDetPhysValTruthDecoratorTool
  IDPV_TruthDecoratorTool = InDetPhysValTruthDecoratorTool()
  ToolSvc += IDPV_TruthDecoratorTool
  # --- and the augmentation tool 
  from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__TrackParametersForTruthParticles as TrkParam4TruthPart
  TrkParam4Truth = TrkParam4TruthPart(
                                      name="TrkParam4Truth",
                                      OnlyDressPrimaryTracks = False
                                     )

  # WARNING !!! this crashes on R21: ERROR SG::ExcStoreLocked: Attempted to modify auxiliary data in a locked store: `::d0'
  # commented out until the problem is fixed
  #ToolSvc += TrkParam4Truth
  #AugmentationTools.append(TrkParam4Truth)


#=============================================================================================
# SKIMMING - for now only skimming on MET triggers listed in python/SUSY15TriggerList.py /CO
#=============================================================================================
# now done in ExtendedJetCommon 
#applyJetCalibration_xAODColl("AntiKt4EMTopo", SeqSUSY15) # default: sequence=DerivationFrameworkJob
#updateJVT_xAODColl("AntiKt4EMTopo") # TODO: for next cache?

expression_trigger = "(" + " || ".join(triggers) + ")"

expression = expression_trigger

print "expression = '" + expression + "'"
print

from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__xAODStringSkimmingTool
SUSY15SkimmingTool = DerivationFramework__xAODStringSkimmingTool( name = "SUSY15SkimmingTool", expression = expression)

ToolSvc += SUSY15SkimmingTool


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
   SeqSUSY15 += CfgMgr.DerivationFramework__DerivationKernel("SUSY15KernelSigAug",
                                                            AugmentationTools = DecorateSUSYProcess("SUSY15")
                                                            )
   
   from DerivationFrameworkSUSY.SUSYWeightMetadata import *


#==============================================================================
# SUSY skimming selection
#==============================================================================
SeqSUSY15 += CfgMgr.DerivationFramework__DerivationKernel(
        "SUSY15KernelSkim",
        SkimmingTools = [SUSY15SkimmingTool],
)


#==============================================================================
# Jet building
#==============================================================================
#re-tag PFlow jets so they have b-tagging info.
FlavorTagInit(JetCollections = ['AntiKt4EMPFlowJets'], Sequencer = SeqSUSY15)

#==============================================================================
# now part of MCTruthCommon
#if DerivationFrameworkIsMonteCarlo:
#
#  OutputJets["SUSY15"] = []
#  reducedJetList = [ "AntiKt4TruthJets", "AntiKt4TruthWZJets" ]
#
#  replaceAODReducedJets(reducedJetList, SeqSUSY15, "SUSY15")


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
SeqSUSY15 += CfgMgr.DerivationFramework__DerivationKernel(
        "SUSY15KernelAug",
        AugmentationTools = AugmentationTools,
        ThinningTools = thinningTools,
)


#====================================================================
# CONTENT LIST
#====================================================================
# This might be the kind of set-up one would have for a muon based analysis
from DerivationFrameworkCore.SlimmingHelper import SlimmingHelper
SUSY15SlimmingHelper = SlimmingHelper("SUSY15SlimmingHelper")
SUSY15SlimmingHelper.SmartCollections = [
                                         "Electrons",
                                         "Photons",
                                         "Muons",
                                         "TauJets",
                                         "MET_Reference_AntiKt4EMTopo",
"MET_Reference_AntiKt4EMPFlow",

                                         "AntiKt4EMTopoJets",
"AntiKt4EMPFlowJets",

                                         "BTagging_AntiKt4EMTopo",
"BTagging_AntiKt4EMPFlow",

                                         "InDetTrackParticles",
                                         "PrimaryVertices"
                                         ]

SUSY15SlimmingHelper.AllVariables = [
                                     "TruthParticles",
                                     "TruthEvents",
                                     "TruthVertices",
                                     "MET_Truth",
                                     # additions for DV specific content /CO
                                     "PrimaryVertices",
                                     "VrtSecInclusive_SecondaryVertices",
                                     "VrtSecInclusive_SelectedTrackParticles",
                                     "VrtSecInclusive_All2TrksVertices",
                                     "MET_LocHadTopo",
                                     "MET_Track"
                                     ]

SUSY15SlimmingHelper.ExtraVariables = [ "BTagging_AntiKt4EMTopo.MV1_discriminant.MV1c_discriminant",
                                        "Muons.ptcone30.ptcone20.charge.quality.InnerDetectorPt.MuonSpectrometerPt.CaloLRLikelihood.CaloMuonIDTag.msInnerMatchChi2.msInnerMatchDOF",
					"AntiKt4EMTopoJets.NumTrkPt1000.TrackWidthPt1000.NumTrkPt500.Timing",
					"GSFTrackParticles.chiSquared.hitPattern.patternRecoInfo.numberDoF.numberOfPixelHoles.numberOfPixelSharedHits.numberOfSCTSharedHits.vx.vy.vz.z0.d0.definingParametersCovMatrix.truthOrigin.truthType.beamlineTiltX.beamlineTiltY", # includes additions from Dominik (DVSUSYANLY-53)
					"InDetTrackParticles.truthOrigin.truthType.hitPattern.patternRecoInfo.vx.vy.vz.beamlineTiltX.beamlineTiltY",  # includes additions from Dominik (DVSUSYANLY-53)
					"CombinedMuonTrackParticles.d0.z0.vz.definingParametersCovMatrix.truthOrigin.truthType",
					"ExtrapolatedMuonTrackParticles.d0.z0.vz.definingParametersCovMatrix.truthOrigin.truthType",
					"TauJets.IsTruthMatched.truthOrigin.truthType.truthParticleLink.truthJetLink",
					"MuonTruthParticles.barcode.decayVtxLink.e.m.pdgId.prodVtxLink.decayVtxLink.px.py.pz.recoMuonLink.status.truthOrigin.truthType.charge",
					"AntiKt4TruthJets.eta.m.phi.pt.TruthLabelDeltaR_B.TruthLabelDeltaR_C.TruthLabelDeltaR_T.TruthLabelID.ConeTruthLabelID.PartonTruthLabelID",
                                        "TruthParticles.px.py.pz.m.e.status.pdgId.charge.barcode.prodVtxLink.decayVtxLink.truthOrigin.truthType" # from Dominik (DVANALYSUSY-53)
					"Electrons.bkgMotherPdgId.bkgTruthOrigin"]
SUSY15SlimmingHelper.IncludeMuonTriggerContent = True # needed? /CO
SUSY15SlimmingHelper.IncludeEGammaTriggerContent = True # can change to photons only? /CO
SUSY15SlimmingHelper.IncludeEtMissTriggerContent = True # Added /CO

# All standard truth particle collections are provided by DerivationFrameworkMCTruth (TruthDerivationTools.py)
# Most of the new containers are centrally added to SlimmingHelper via DerivationFrameworkCore ContainersOnTheFly.py
if DerivationFrameworkIsMonteCarlo:

  SUSY15SlimmingHelper.AppendToDictionary = {'BTagging_AntiKt4EMPFlow':'xAOD::BTaggingContainer','BTagging_AntiKt4EMPFlowAux':'xAOD::BTaggingAuxContainer',
'TruthTop':'xAOD::TruthParticleContainer','TruthTopAux':'xAOD::TruthParticleAuxContainer',
                                             'TruthBSM':'xAOD::TruthParticleContainer','TruthBSMAux':'xAOD::TruthParticleAuxContainer',
                                             'TruthBoson':'xAOD::TruthParticleContainer','TruthBosonAux':'xAOD::TruthParticleAuxContainer'}
  
  SUSY15SlimmingHelper.AllVariables += ["TruthElectrons", "TruthMuons", "TruthTaus", "TruthPhotons", "TruthNeutrinos", "TruthTop", "TruthBSM", "TruthBoson"]   


SUSY15SlimmingHelper.AppendContentToStream(SUSY15Stream)
