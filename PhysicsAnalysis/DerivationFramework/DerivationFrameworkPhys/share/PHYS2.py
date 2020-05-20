# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

#====================================================================
# DAOD_PHYS2.py
# This defines DAOD_PHYS2, an unskimmed DAOD format for Run 3.
# It is a clone of DAOD_PHYS but also includes lossy compression
# In the end it will be merged back into DAOD_PHYS
# It requires the reductionConf flag PHYS2 in Reco_tf.py   
#====================================================================

from DerivationFrameworkCore.DerivationFrameworkMaster import *
from DerivationFrameworkInDet.InDetCommon import *
from DerivationFrameworkJetEtMiss.JetCommon import *
from DerivationFrameworkJetEtMiss.ExtendedJetCommon import *
from DerivationFrameworkJetEtMiss.METCommon import *
from DerivationFrameworkEGamma.EGammaCommon import *
from DerivationFrameworkEGamma.ElectronsCPDetailedContent import *
from DerivationFrameworkMuons.MuonsCommon import *
from DerivationFrameworkCore.WeightMetadata import *
from DerivationFrameworkFlavourTag.FlavourTagCommon import FlavorTagInit
from DerivationFrameworkFlavourTag.HbbCommon import *
from TriggerMenu.api.TriggerAPI import TriggerAPI
from TriggerMenu.api.TriggerEnums import TriggerPeriod, TriggerType
from DerivationFrameworkTrigger.TriggerMatchingHelper import TriggerMatchingHelper

#====================================================================
# SET UP STREAM   
#====================================================================
streamName = derivationFlags.WriteDAOD_PHYS2Stream.StreamName
fileName   = buildFileName( derivationFlags.WriteDAOD_PHYS2Stream )
PHYS2Stream = MSMgr.NewPoolRootStream( streamName, fileName )
PHYS2Stream.AcceptAlgs(["PHYS2Kernel"])

### Thinning and augmentation tools lists
from DerivationFrameworkCore.ThinningHelper import ThinningHelper
PHYS2ThinningHelper = ThinningHelper( "PHYS2ThinningHelper" )
PHYS2ThinningHelper.AppendToStream( PHYS2Stream )
thinningTools       = []
AugmentationTools   = []

# Special sequence 
SeqPHYS2 = CfgMgr.AthSequencer("SeqPHYS2")
DerivationFrameworkJob += SeqPHYS2

#====================================================================
# Truth collections
#====================================================================
if (DerivationFrameworkIsMonteCarlo):
   from DerivationFrameworkMCTruth.MCTruthCommon import addStandardTruthContents,addMiniTruthCollectionLinks,addHFAndDownstreamParticles,addPVCollection
   import DerivationFrameworkHiggs.TruthCategories
   # Add charm quark collection
   from DerivationFrameworkMCTruth.DerivationFrameworkMCTruthConf import DerivationFramework__TruthCollectionMaker
   PHYS2TruthCharmTool = DerivationFramework__TruthCollectionMaker(name                    = "PHYS2TruthCharmTool",
                                                                  NewCollectionName       = "TruthCharm",
                                                                  KeepNavigationInfo      = False,
                                                                  ParticleSelectionString = "(abs(TruthParticles.pdgId) == 4)",
                                                                  Do_Compress             = True)
   ToolSvc += PHYS2TruthCharmTool
   from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__CommonAugmentation
   SeqPHYS2 += CfgMgr.DerivationFramework__CommonAugmentation("PHYS2TruthCharmKernel",AugmentationTools=[PHYS2TruthCharmTool])
   # Add HF particles
   addHFAndDownstreamParticles(SeqPHYS2)
   # Add standard truth
   addStandardTruthContents(SeqPHYS2,prefix='PHYS2_')
   # Update to include charm quarks and HF particles - require a separate instance to be train safe
   from DerivationFrameworkMCTruth.DerivationFrameworkMCTruthConf import DerivationFramework__TruthNavigationDecorator
   PHYS2TruthNavigationDecorator = DerivationFramework__TruthNavigationDecorator( name="PHYS2TruthNavigationDecorator",
          InputCollections=["TruthElectrons", "TruthMuons", "TruthPhotons", "TruthTaus", "TruthNeutrinos", "TruthBSM", "TruthBottom", "TruthTop", "TruthBoson","TruthCharm","TruthHFWithDecayParticles"])
   ToolSvc += PHYS2TruthNavigationDecorator
   SeqPHYS2.PHYS2_MCTruthNavigationDecoratorKernel.AugmentationTools = [PHYS2TruthNavigationDecorator]
   # Re-point links on reco objects
   addMiniTruthCollectionLinks(SeqPHYS2)
   addPVCollection(SeqPHYS2)
   # Set appropriate truth jet collection for tau truth matching
   ToolSvc.DFCommonTauTruthMatchingTool.TruthJetContainerName = "AntiKt4TruthDressedWZJets"
   # SUSY signal
   from DerivationFrameworkSUSY.DecorateSUSYProcess import IsSUSYSignal
   if IsSUSYSignal():
      from DerivationFrameworkSUSY.SUSYWeightMetadata import *
   # Add sumOfWeights metadata for LHE3 multiweights =======
   from DerivationFrameworkCore.LHE3WeightMetadata import *

#====================================================================
# TRIGGER CONTENT   
#====================================================================
# See https://twiki.cern.ch/twiki/bin/view/Atlas/TriggerAPI
# Get single and multi mu, e, photon triggers
# Jet, tau, multi-object triggers not available in the matching code
allperiods = TriggerPeriod.y2015 | TriggerPeriod.y2016 | TriggerPeriod.y2017 | TriggerPeriod.y2018 | TriggerPeriod.future2e34
trig_el  = TriggerAPI.getLowestUnprescaledAnyPeriod(allperiods, triggerType=TriggerType.el,  livefraction=0.8)
trig_mu  = TriggerAPI.getLowestUnprescaledAnyPeriod(allperiods, triggerType=TriggerType.mu,  livefraction=0.8)
trig_g   = TriggerAPI.getLowestUnprescaledAnyPeriod(allperiods, triggerType=TriggerType.g,   livefraction=0.8)
trig_tau = TriggerAPI.getLowestUnprescaledAnyPeriod(allperiods, triggerType=TriggerType.tau, livefraction=0.8)
# Add cross-triggers for some sets
trig_em = TriggerAPI.getLowestUnprescaledAnyPeriod(allperiods, triggerType=TriggerType.el, additionalTriggerType=TriggerType.mu,  livefraction=0.8)
trig_et = TriggerAPI.getLowestUnprescaledAnyPeriod(allperiods, triggerType=TriggerType.el, additionalTriggerType=TriggerType.tau, livefraction=0.8)
trig_mt = TriggerAPI.getLowestUnprescaledAnyPeriod(allperiods, triggerType=TriggerType.mu, additionalTriggerType=TriggerType.tau, livefraction=0.8)
# Note that this seems to pick up both isolated and non-isolated triggers already, so no need for extra grabs

# Merge and remove duplicates
trigger_names_full = list(set(trig_el+trig_mu+trig_g+trig_tau+trig_em+trig_et+trig_mt))

# Now reduce the list...
from RecExConfig.InputFilePeeker import inputFileSummary
trigger_names = []
for trig_item in inputFileSummary['metadata']['/TRIGGER/HLT/Menu']:
    if not 'ChainName' in trig_item: continue
    if trig_item['ChainName'] in trigger_names_full: trigger_names += [ trig_item['ChainName'] ]

# Create trigger matching decorations
trigmatching_helper = TriggerMatchingHelper(
        trigger_list = trigger_names, add_to_df_job=True)

#====================================================================
# INNER DETECTOR TRACK THINNING
#====================================================================
# See recommedations here: 
# https://twiki.cern.ch/twiki/bin/viewauth/AtlasProtected/DaodRecommendations

# Inner detector group recommendations for indet tracks in analysis
PHYS2_thinning_expression = "InDetTrackParticles.DFCommonTightPrimary && abs(DFCommonInDetTrackZ0AtPV)*sin(InDetTrackParticles.theta) < 3.0*mm && InDetTrackParticles.pt > 10*GeV"
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__TrackParticleThinning
PHYS2TrackParticleThinningTool = DerivationFramework__TrackParticleThinning(name                    = "PHYS2TrackParticleThinningTool",
                                                                           ThinningService         = PHYS2ThinningHelper.ThinningSvc(),
                                                                           SelectionString         = PHYS2_thinning_expression,
                                                                           InDetTrackParticlesKey  = "InDetTrackParticles",
                                                                           ApplyAnd                = False)

ToolSvc += PHYS2TrackParticleThinningTool
thinningTools.append(PHYS2TrackParticleThinningTool)

# Include inner detector tracks associated with muons
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__MuonTrackParticleThinning
PHYS2MuonTPThinningTool = DerivationFramework__MuonTrackParticleThinning(name                    = "PHYS2MuonTPThinningTool",
                                                                        ThinningService         = PHYS2ThinningHelper.ThinningSvc(),
                                                                        MuonKey                 = "Muons",
                                                                        InDetTrackParticlesKey  = "InDetTrackParticles",
                                                                        ApplyAnd = False)
ToolSvc += PHYS2MuonTPThinningTool
thinningTools.append(PHYS2MuonTPThinningTool)

# TauJets thinning
tau_thinning_expression = "(TauJets.ptFinalCalib >= 13.*GeV) && (TauJets.nTracks>=1) && (TauJets.nTracks<=3) && (TauJets.RNNJetScoreSigTrans>0.01)"
from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__GenericObjectThinning
PHYS2TauJetsThinningTool = DerivationFramework__GenericObjectThinning(name            = "PHYS2TauJetsThinningTool",
                                                                     ThinningService = PHYS2ThinningHelper.ThinningSvc(),
                                                                     ContainerName   = "TauJets",
                                                                     SelectionString = tau_thinning_expression)
ToolSvc += PHYS2TauJetsThinningTool
thinningTools.append(PHYS2TauJetsThinningTool)

# Only keep tau tracks (and associated ID tracks) classified as charged tracks
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__TauTrackParticleThinning
PHYS2TauTPThinningTool = DerivationFramework__TauTrackParticleThinning(name                   = "PHYS2TauTPThinningTool",
                                                                      ThinningService        = PHYS2ThinningHelper.ThinningSvc(),
                                                                      TauKey                 = "TauJets",
                                                                      InDetTrackParticlesKey = "InDetTrackParticles",
                                                                      SelectionString        = tau_thinning_expression,
                                                                      ApplyAnd               = False,
                                                                      DoTauTracksThinning    = True,
                                                                      TauTracksKey           = "TauTracks")
ToolSvc += PHYS2TauTPThinningTool
thinningTools.append(PHYS2TauTPThinningTool)

# ID tracks associated with high-pt di-tau
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__DiTauTrackParticleThinning
PHYS2DiTauTPThinningTool = DerivationFramework__DiTauTrackParticleThinning(name                    = "PHYS2DiTauTPThinningTool",
                                                                          ThinningService         = PHYS2ThinningHelper.ThinningSvc(),
                                                                          DiTauKey                = "DiTauJets",
                                                                          InDetTrackParticlesKey  = "InDetTrackParticles")
ToolSvc += PHYS2DiTauTPThinningTool
thinningTools.append(PHYS2DiTauTPThinningTool)

#====================================================================
# JET/MET   
#====================================================================

OutputJets["PHYS2"] = ["AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets"]
reducedJetList = ["AntiKt2PV0TrackJets","AntiKt4PV0TrackJets"]

if (DerivationFrameworkIsMonteCarlo):
   OutputJets["PHYS2"].append("AntiKt10TruthTrimmedPtFrac5SmallR20Jets")

replaceAODReducedJets(reducedJetList,SeqPHYS2,"PHYS2")
addDefaultTrimmedJets(SeqPHYS2,"PHYS2",dotruth=DerivationFrameworkIsMonteCarlo)

# Add large-R jet truth labeling
if (DerivationFrameworkIsMonteCarlo):
   addJetTruthLabel(jetalg="AntiKt10LCTopoTrimmedPtFrac5SmallR20",sequence=SeqPHYS2,algname="JetTruthLabelingAlg",labelname="R10TruthLabel_R21Consolidated")

addQGTaggerTool(jetalg="AntiKt4EMTopo",sequence=SeqPHYS2,algname="QGTaggerToolAlg")
addQGTaggerTool(jetalg="AntiKt4EMPFlow",sequence=SeqPHYS2,algname="QGTaggerToolPFAlg")

# fJVT
getPFlowfJVT(jetalg='AntiKt4EMPFlow',sequence=DerivationFrameworkJob, algname='PHYS2JetForwardPFlowJvtToolAlg')

#====================================================================
# Tau   
#====================================================================

# Schedule low-pt di-tau reconstruction (needs AntiKt2PV0TrackJets)
from DerivationFrameworkTau.TauCommon import addDiTauLowPt
addDiTauLowPt()

# Low-pt di-tau thinning
from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__GenericObjectThinning
PHYS2DiTauLowPtThinningTool = DerivationFramework__GenericObjectThinning(name            = "PHYS2DiTauLowPtThinningTool",
                                                                        ThinningService = PHYS2ThinningHelper.ThinningSvc(),
                                                                        ContainerName   = "DiTauJetsLowPt",
                                                                        SelectionString = "DiTauJetsLowPt.nSubjets > 1")
ToolSvc += PHYS2DiTauLowPtThinningTool
thinningTools.append(PHYS2DiTauLowPtThinningTool)

# ID tracks associated with low-pt ditau
PHYS2DiTauLowPtTPThinningTool = DerivationFramework__DiTauTrackParticleThinning(name                    = "PHYS2DiTauLowPtTPThinningTool",
                                                                               ThinningService         = PHYS2ThinningHelper.ThinningSvc(),
                                                                               DiTauKey                = "DiTauJetsLowPt",
                                                                               InDetTrackParticlesKey  = "InDetTrackParticles",
                                                                               SelectionString         = "DiTauJetsLowPt.nSubjets > 1")
ToolSvc += PHYS2DiTauLowPtTPThinningTool
thinningTools.append(PHYS2DiTauLowPtTPThinningTool)

#====================================================================
# CREATE THE DERIVATION KERNEL ALGORITHM   
#====================================================================

from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__DerivationKernel
DerivationFrameworkJob += CfgMgr.DerivationFramework__DerivationKernel("PHYS2Kernel",
                                                                       ThinningTools = thinningTools)


#====================================================================
# FLAVOUR TAGGING   
#====================================================================
# Create variable-R trackjets and dress AntiKt10LCTopo with ghost VR-trkjet 
addVRJets(SeqPHYS2)
addVRJets(SeqPHYS2, training='201903')
#addVRJetsTCC(DerivationFrameworkJob, "AntiKtVR30Rmax4Rmin02Track", "GhostVR30Rmax4Rmin02TrackJet",
#             VRJetAlg="AntiKt", VRJetRadius=0.4, VRJetInputs="pv0track",
#             ghostArea = 0 , ptmin = 2000, ptminFilter = 2000,
#             variableRMinRadius = 0.02, variableRMassScale = 30000, calibOpt = "none")
# add xbb taggers
from DerivationFrameworkFlavourTag.HbbCommon import addRecommendedXbbTaggers
addRecommendedXbbTaggers(SeqPHYS2, ToolSvc)

FlavorTagInit(JetCollections  = [ 'AntiKt4EMTopoJets','AntiKt4EMPFlowJets'], Sequencer = SeqPHYS2)

#====================================================================
# TC-LVT Vertices 
#====================================================================

from SoftBVrtClusterTool.SoftBVrtConfig import addSoftBVrt
addSoftBVrt(DerivationFrameworkJob,'Loose')
addSoftBVrt(DerivationFrameworkJob,'Medium')
addSoftBVrt(DerivationFrameworkJob,'Tight')

#====================================================================
# CONTENTS   
#====================================================================
from DerivationFrameworkCore.SlimmingHelper import SlimmingHelper
PHYS2SlimmingHelper = SlimmingHelper("PHYS2SlimmingHelper")

PHYS2SlimmingHelper.SmartCollections = ["Electrons",
                                       "Photons",
                                       "Muons",
                                       "PrimaryVertices",
                                       "InDetTrackParticles",
                                       "AntiKt4EMTopoJets",
                                       "AntiKt4EMPFlowJets",
                                       "BTagging_AntiKt4EMTopo_201810",
                                       "BTagging_AntiKt4EMPFlow_201810",
                                       "BTagging_AntiKt4EMPFlow_201903",
                                       "MET_Baseline_AntiKt4EMTopo",
                                       "MET_Baseline_AntiKt4EMPFlow",
                                       "TauJets",
                                       "DiTauJets",
                                       "DiTauJetsLowPt",
                                       "AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets",
                                       "AntiKtVR30Rmax4Rmin02TrackJets_BTagging201903",
                                       "BTagging_AntiKtVR30Rmax4Rmin02Track_201903"
                                      ]

excludedVertexAuxData = "-vxTrackAtVertex.-MvfFitInfo.-isInitialized.-VTAV"
StaticContent = []
StaticContent += ["xAOD::VertexContainer#SoftBVrtClusterTool_Tight_Vertices"]
StaticContent += ["xAOD::VertexAuxContainer#SoftBVrtClusterTool_Tight_VerticesAux." + excludedVertexAuxData]
StaticContent += ["xAOD::VertexContainer#SoftBVrtClusterTool_Medium_Vertices"]
StaticContent += ["xAOD::VertexAuxContainer#SoftBVrtClusterTool_Medium_VerticesAux." + excludedVertexAuxData]
StaticContent += ["xAOD::VertexContainer#SoftBVrtClusterTool_Loose_Vertices"]
StaticContent += ["xAOD::VertexAuxContainer#SoftBVrtClusterTool_Loose_VerticesAux." + excludedVertexAuxData]

PHYS2SlimmingHelper.StaticContent = StaticContent

# Trigger content
PHYS2SlimmingHelper.IncludeTriggerNavigation = False
PHYS2SlimmingHelper.IncludeJetTriggerContent = False
PHYS2SlimmingHelper.IncludeMuonTriggerContent = False
PHYS2SlimmingHelper.IncludeEGammaTriggerContent = False
PHYS2SlimmingHelper.IncludeJetTauEtMissTriggerContent = False
PHYS2SlimmingHelper.IncludeTauTriggerContent = False
PHYS2SlimmingHelper.IncludeEtMissTriggerContent = False
PHYS2SlimmingHelper.IncludeBJetTriggerContent = False
PHYS2SlimmingHelper.IncludeBPhysTriggerContent = False
PHYS2SlimmingHelper.IncludeMinBiasTriggerContent = False

# Add the jet containers to the stream (defined in JetCommon if import needed)
#addJetOutputs(PHYS2SlimmingHelper,["PHYS2"])

# Truth containers
if DerivationFrameworkIsMonteCarlo:
   PHYS2SlimmingHelper.AppendToDictionary = {'TruthEvents':'xAOD::TruthEventContainer','TruthEventsAux':'xAOD::TruthEventAuxContainer',
                                            'MET_Truth':'xAOD::MissingETContainer','MET_TruthAux':'xAOD::MissingETAuxContainer',
                                            'TruthElectrons':'xAOD::TruthParticleContainer','TruthElectronsAux':'xAOD::TruthParticleAuxContainer',
                                            'TruthMuons':'xAOD::TruthParticleContainer','TruthMuonsAux':'xAOD::TruthParticleAuxContainer',
                                            'TruthPhotons':'xAOD::TruthParticleContainer','TruthPhotonsAux':'xAOD::TruthParticleAuxContainer',
                                            'TruthTaus':'xAOD::TruthParticleContainer','TruthTausAux':'xAOD::TruthParticleAuxContainer',
                                            'TruthNeutrinos':'xAOD::TruthParticleContainer','TruthNeutrinosAux':'xAOD::TruthParticleAuxContainer',
                                            'TruthBSM':'xAOD::TruthParticleContainer','TruthBSMAux':'xAOD::TruthParticleAuxContainer',
                                            'TruthBoson':'xAOD::TruthParticleContainer','TruthBosonAux':'xAOD::TruthParticleAuxContainer',
                                            'TruthTop':'xAOD::TruthParticleContainer','TruthTopAux':'xAOD::TruthParticleAuxContainer',
                                            'TruthForwardProtons':'xAOD::TruthParticleContainer','TruthForwardProtonsAux':'xAOD::TruthParticleAuxContainer',
                                            'BornLeptons':'xAOD::TruthParticleContainer','BornLeptonsAux':'xAOD::TruthParticleAuxContainer',
                                            'TruthBosonsWithDecayParticles':'xAOD::TruthParticleContainer','TruthBosonsWithDecayParticlesAux':'xAOD::TruthParticleAuxContainer',
                                            'TruthBosonsWithDecayVertices':'xAOD::TruthVertexContainer','TruthBosonsWithDecayVerticesAux':'xAOD::TruthVertexAuxContainer',
                                            'TruthBSMWithDecayParticles':'xAOD::TruthParticleContainer','TruthBSMWithDecayParticlesAux':'xAOD::TruthParticleAuxContainer',
                                            'TruthBSMWithDecayVertices':'xAOD::TruthVertexContainer','TruthBSMWithDecayVerticesAux':'xAOD::TruthVertexAuxContainer',
                                            'HardScatterParticles':'xAOD::TruthParticleContainer','HardScatterParticlesAux':'xAOD::TruthParticleAuxContainer',
                                            'HardScatterVertices':'xAOD::TruthVertexContainer','HardScatterVerticesAux':'xAOD::TruthVertexAuxContainer',
                                            'TruthHFWithDecayParticles':'xAOD::TruthParticleContainer','TruthHFWithDecayParticlesAux':'xAOD::TruthParticleAuxContainer',
                                            'TruthHFWithDecayVertices':'xAOD::TruthVertexContainer','TruthHFWithDecayVerticesAux':'xAOD::TruthVertexAuxContainer',
                                            'TruthCharm':'xAOD::TruthParticleContainer','TruthCharmAux':'xAOD::TruthParticleAuxContainer',
                                            'TruthPrimaryVertices':'xAOD::TruthVertexContainer','TruthPrimaryVerticesAux':'xAOD::TruthVertexAuxContainer',
                                            'AntiKt10TruthTrimmedPtFrac5SmallR20Jets':'xAOD::JetContainer', 'AntiKt10TruthTrimmedPtFrac5SmallR20JetsAux':'xAOD::JetAuxContainer',
                                            'AntiKtVR30Rmax4Rmin02TrackJets_BTagging201903':'xAOD::JetContainer','BTagging_AntiKtVR30Rmax4Rmin02Track_201903':'xAOD::BTagging'
                                           }

   from DerivationFrameworkMCTruth.MCTruthCommon import addTruth3ContentToSlimmerTool
   addTruth3ContentToSlimmerTool(PHYS2SlimmingHelper)
   PHYS2SlimmingHelper.AllVariables += ['TruthHFWithDecayParticles','TruthHFWithDecayVertices','TruthCharm']

PHYS2SlimmingHelper.ExtraVariables += ["AntiKt10TruthTrimmedPtFrac5SmallR20Jets.Tau1_wta.Tau2_wta.Tau3_wta.D2.GhostBHadronsFinalCount",
                                      "Electrons.TruthLink",
                                      "Muons.TruthLink",
                                      "Photons.TruthLink",
                                      "AntiKt2PV0TrackJets.pt.eta.phi.m",
                                      "AntiKt4EMTopoJets.DFCommonJets_QGTagger_truthjet_nCharged.DFCommonJets_QGTagger_truthjet_pt.DFCommonJets_QGTagger_truthjet_eta.DFCommonJets_QGTagger_NTracks.DFCommonJets_QGTagger_TracksWidth.DFCommonJets_QGTagger_TracksC1.PartonTruthLabelID",
                                      "AntiKt4EMPFlowJets.DFCommonJets_QGTagger_truthjet_nCharged.DFCommonJets_QGTagger_truthjet_pt.DFCommonJets_QGTagger_truthjet_eta.DFCommonJets_QGTagger_NTracks.DFCommonJets_QGTagger_TracksWidth.DFCommonJets_QGTagger_TracksC1.PartonTruthLabelID.DFCommonJets_fJvt",
                                      "TruthPrimaryVertices.t.x.y.z"]

# Float compression
from DerivationFrameworkPhys.PHYS_MCCompressionList import PHYS_MCCompressionList
from DerivationFrameworkPhys.PHYS_DataCompressionList import PHYS_DataCompressionList
PHYS2_compression_list = []
if (DerivationFrameworkIsMonteCarlo): PHYS2_compression_list = PHYS_MCCompressionList 
else: PHYS2_compression_list = PHYS_DataCompressionList
PHYS2_SeqCompress = CfgMgr.AthSequencer("PHYS2_SeqCompress")
DerivationFrameworkJob += PHYS2_SeqCompress
PHYS2_SeqCompress += CfgMgr.DerivationFramework__FloatCompressionAlg("CompressFloatCompression", SGKeys=PHYS2_compression_list)

# Add trigger matching
trigmatching_helper.add_to_slimming(PHYS2SlimmingHelper)

# Final construction of output stream
PHYS2SlimmingHelper.AppendContentToStream(PHYS2Stream)

