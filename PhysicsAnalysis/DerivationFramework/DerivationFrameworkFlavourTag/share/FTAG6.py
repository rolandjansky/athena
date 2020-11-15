# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

#====================================================================
# DAOD_FTAG6.py
# This defines DAOD_FTAG6, an unskimmed DAOD format for Run 3.
# It contains inputs for the flavor tagging algorithms training.
# It requires the reductionConf flag FTAG6 in Reco_tf.py
#====================================================================

from DerivationFrameworkCore.DerivationFrameworkMaster import buildFileName, DerivationFrameworkIsMonteCarlo, DerivationFrameworkJob
from DerivationFrameworkInDet import InDetCommon
from DerivationFrameworkJetEtMiss.JetCommon import OutputJets
from DerivationFrameworkJetEtMiss.ExtendedJetCommon import replaceAODReducedJets, addDefaultTrimmedJets, addJetTruthLabel, addQGTaggerTool
from DerivationFrameworkJetEtMiss import METCommon
from DerivationFrameworkEGamma import EGammaCommon
from DerivationFrameworkEGamma import ElectronsCPDetailedContent
from DerivationFrameworkMuons import MuonsCommon
from TriggerMenu.api.TriggerAPI import TriggerAPI
from TriggerMenu.api.TriggerEnums import TriggerPeriod, TriggerType
from DerivationFrameworkTrigger.TriggerMatchingHelper import TriggerMatchingHelper

#====================================================================
# SET UP STREAM
#====================================================================
streamName = derivationFlags.WriteDAOD_FTAG6Stream.StreamName
fileName   = buildFileName( derivationFlags.WriteDAOD_FTAG6Stream )
FTAG6Stream = MSMgr.NewPoolRootStream( streamName, fileName )
FTAG6Stream.AcceptAlgs(["FTAG6Kernel"])

### Thinning and augmentation tools lists
thinningTools       = []
AugmentationTools   = []

# Special sequence
SeqFTAG6 = CfgMgr.AthSequencer("SeqFTAG6")

#====================================================================
# MONTE CARLO TRUTH
#====================================================================
if (DerivationFrameworkIsMonteCarlo):
   from DerivationFrameworkMCTruth.MCTruthCommon import addStandardTruthContents,addMiniTruthCollectionLinks,addHFAndDownstreamParticles,addPVCollection
   #import DerivationFrameworkHiggs.TruthCategories
   # Add charm quark collection
   from DerivationFrameworkMCTruth.DerivationFrameworkMCTruthConf import DerivationFramework__TruthCollectionMaker
   FTAG6TruthCharmTool = DerivationFramework__TruthCollectionMaker(name                    = "FTAG6TruthCharmTool",
                                                                  NewCollectionName       = "TruthCharm",
                                                                  KeepNavigationInfo      = False,
                                                                  ParticleSelectionString = "(abs(TruthParticles.pdgId) == 4)",
                                                                  Do_Compress             = True)
   ToolSvc += FTAG6TruthCharmTool
   #from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__CommonAugmentation
   SeqFTAG6 += CfgMgr.DerivationFramework__CommonAugmentation("FTAG6TruthCharmKernel",AugmentationTools=[FTAG6TruthCharmTool])
   # Add HF particles
   addHFAndDownstreamParticles(SeqFTAG6)
   # Add standard truth
   addStandardTruthContents(SeqFTAG6,prefix='FTAG6_')
   # Update to include charm quarks and HF particles - require a separate instance to be train safe
   from DerivationFrameworkMCTruth.DerivationFrameworkMCTruthConf import DerivationFramework__TruthNavigationDecorator
   FTAG6TruthNavigationDecorator = DerivationFramework__TruthNavigationDecorator( name="FTAG6TruthNavigationDecorator",
          InputCollections=["TruthElectrons", "TruthMuons", "TruthPhotons", "TruthTaus", "TruthNeutrinos", "TruthBSM", "TruthBottom", "TruthTop", "TruthBoson","TruthCharm","TruthHFWithDecayParticles"])
   ToolSvc += FTAG6TruthNavigationDecorator
   SeqFTAG6.FTAG6_MCTruthNavigationDecoratorKernel.AugmentationTools = [FTAG6TruthNavigationDecorator]
   # Re-point links on reco objects
   addMiniTruthCollectionLinks(SeqFTAG6)
   addPVCollection(SeqFTAG6)
   # Set appropriate truth jet collection for tau truth matching
   ToolSvc.DFCommonTauTruthMatchingTool.TruthJetContainerName = "AntiKt4TruthDressedWZJets"
   # Add sumOfWeights metadata for LHE3 multiweights =======
   #from DerivationFrameworkCore.LHE3WeightMetadata import *



#====================================================================
# TRIGGER CONTENT
#====================================================================
## See https://twiki.cern.ch/twiki/bin/view/Atlas/TriggerAPI
## Get single and multi mu, e, photon triggers
## Jet, tau, multi-object triggers not available in the matching code
allperiods = TriggerPeriod.y2015 | TriggerPeriod.y2016 | TriggerPeriod.y2017 | TriggerPeriod.y2018 | TriggerPeriod.future2e34
trig_el  = TriggerAPI.getLowestUnprescaledAnyPeriod(allperiods, triggerType=TriggerType.el,  livefraction=0.8)
trig_mu  = TriggerAPI.getLowestUnprescaledAnyPeriod(allperiods, triggerType=TriggerType.mu,  livefraction=0.8)
trig_g   = TriggerAPI.getLowestUnprescaledAnyPeriod(allperiods, triggerType=TriggerType.g,   livefraction=0.8)
trig_tau = TriggerAPI.getLowestUnprescaledAnyPeriod(allperiods, triggerType=TriggerType.tau, livefraction=0.8)
## Add cross-triggers for some sets
trig_em = TriggerAPI.getLowestUnprescaledAnyPeriod(allperiods, triggerType=TriggerType.el, additionalTriggerType=TriggerType.mu,  livefraction=0.8)
trig_et = TriggerAPI.getLowestUnprescaledAnyPeriod(allperiods, triggerType=TriggerType.el, additionalTriggerType=TriggerType.tau, livefraction=0.8)
trig_mt = TriggerAPI.getLowestUnprescaledAnyPeriod(allperiods, triggerType=TriggerType.mu, additionalTriggerType=TriggerType.tau, livefraction=0.8)
## Note that this seems to pick up both isolated and non-isolated triggers already, so no need for extra grabs
trig_txe = TriggerAPI.getLowestUnprescaledAnyPeriod(allperiods, triggerType=TriggerType.tau, additionalTriggerType=TriggerType.xe, livefraction=0.8)
#
## Merge and remove duplicates
trigger_names_full_notau = list(set(trig_el+trig_mu+trig_g+trig_em+trig_et+trig_mt))
trigger_names_full_tau = list(set(trig_tau+trig_txe))
#
## Now reduce the list...
trigger_names_notau = []
trigger_names_tau = []
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
from AthenaConfiguration.AutoConfigFlags import GetFileMD
for chain_name in GetFileMD(athenaCommonFlags.FilesInput.get_Value())['TriggerMenu']['HLTChains']:
   if chain_name in trigger_names_full_notau: trigger_names_notau.append(chain_name)
   if chain_name in trigger_names_full_tau:   trigger_names_tau.append(chain_name)
# Create trigger matching decorations
trigmatching_helper_notau = TriggerMatchingHelper(name='FTAG6TriggerMatchingToolNoTau',
        trigger_list = trigger_names_notau, add_to_df_job=True)
trigmatching_helper_tau = TriggerMatchingHelper(name='FTAG6TriggerMatchingToolTau',
        trigger_list = trigger_names_tau, add_to_df_job=True, DRThreshold=0.2)



#====================================================================
# INNER DETECTOR TRACK THINNING
#====================================================================
# See recommedations here:
# https://twiki.cern.ch/twiki/bin/viewauth/AtlasProtected/DaodRecommendations

# Inner detector group recommendations for indet tracks in analysis
FTAG6_thinning_expression = "InDetTrackParticles.DFCommonTightPrimary && abs(DFCommonInDetTrackZ0AtPV)*sin(InDetTrackParticles.theta) < 3.0*mm && InDetTrackParticles.pt > 10*GeV"
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__TrackParticleThinning
FTAG6TrackParticleThinningTool = DerivationFramework__TrackParticleThinning(name                    = "FTAG6TrackParticleThinningTool",
                                                                           StreamName              = FTAG6Stream.Name,
                                                                           SelectionString         = FTAG6_thinning_expression,
                                                                           InDetTrackParticlesKey  = "InDetTrackParticles")

ToolSvc += FTAG6TrackParticleThinningTool
thinningTools.append(FTAG6TrackParticleThinningTool)

# Include inner detector tracks associated with muons
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__MuonTrackParticleThinning
FTAG6MuonTPThinningTool = DerivationFramework__MuonTrackParticleThinning(name                    = "FTAG6MuonTPThinningTool",
                                                                        StreamName              = FTAG6Stream.Name,
                                                                        MuonKey                 = "Muons",
                                                                        InDetTrackParticlesKey  = "InDetTrackParticles")

ToolSvc += FTAG6MuonTPThinningTool
thinningTools.append(FTAG6MuonTPThinningTool)

# TauJets thinning
tau_thinning_expression = "(TauJets.ptFinalCalib >= 13.*GeV) && (TauJets.nTracks>=1) && (TauJets.nTracks<=3) && (TauJets.RNNJetScoreSigTrans>0.01)"
from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__GenericObjectThinning
FTAG6TauJetsThinningTool = DerivationFramework__GenericObjectThinning(name            = "FTAG6TauJetsThinningTool",
                                                                     StreamName      = FTAG6Stream.Name,
                                                                     ContainerName   = "TauJets",
                                                                     SelectionString = tau_thinning_expression)
ToolSvc += FTAG6TauJetsThinningTool
thinningTools.append(FTAG6TauJetsThinningTool)

# Only keep tau tracks (and associated ID tracks) classified as charged tracks
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__TauTrackParticleThinning
FTAG6TauTPThinningTool = DerivationFramework__TauTrackParticleThinning(name                   = "FTAG6TauTPThinningTool",
                                                                      StreamName             = FTAG6Stream.Name,
                                                                      TauKey                 = "TauJets",
                                                                      InDetTrackParticlesKey = "InDetTrackParticles",
                                                                      SelectionString        = tau_thinning_expression,
                                                                      DoTauTracksThinning    = True,
                                                                      TauTracksKey           = "TauTracks")
ToolSvc += FTAG6TauTPThinningTool
thinningTools.append(FTAG6TauTPThinningTool)

# ID tracks associated with high-pt di-tau
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__DiTauTrackParticleThinning
FTAG6DiTauTPThinningTool = DerivationFramework__DiTauTrackParticleThinning(name                    = "FTAG6DiTauTPThinningTool",
                                                                          StreamName              = FTAG6Stream.Name,
                                                                          DiTauKey                = "DiTauJets",
                                                                          InDetTrackParticlesKey  = "InDetTrackParticles")
ToolSvc += FTAG6DiTauTPThinningTool
thinningTools.append(FTAG6DiTauTPThinningTool)

#====================================================================
# JET/MET
#====================================================================

OutputJets["FTAG6"] = ["AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets"]
reducedJetList = ["AntiKt2PV0TrackJets","AntiKt4PV0TrackJets"]

if (DerivationFrameworkIsMonteCarlo):
   OutputJets["FTAG6"].append("AntiKt10TruthTrimmedPtFrac5SmallR20Jets")

replaceAODReducedJets(reducedJetList,SeqFTAG6,"FTAG6")
add_largeR_truth_jets = DerivationFrameworkIsMonteCarlo and not hasattr(SeqFTAG6,'jetalgAntiKt10TruthTrimmedPtFrac5SmallR20')
addDefaultTrimmedJets(SeqFTAG6,"FTAG6",dotruth=add_largeR_truth_jets)

# Add large-R jet truth labeling
if (DerivationFrameworkIsMonteCarlo):
   addJetTruthLabel(jetalg="AntiKt10LCTopoTrimmedPtFrac5SmallR20",sequence=SeqFTAG6,algname="JetTruthLabelingAlg",labelname="R10TruthLabel_R21Consolidated")

addQGTaggerTool(jetalg="AntiKt4EMTopo",sequence=SeqFTAG6,algname="QGTaggerToolAlg")
addQGTaggerTool(jetalg="AntiKt4EMPFlow",sequence=SeqFTAG6,algname="QGTaggerToolPFAlg")

# fJVT
# getPFlowfJVT(jetalg='AntiKt4EMPFlow',sequence=SeqPHYS, algname='PHYSJetForwardPFlowJvtToolAlg')

#====================================================================
# EGAMMA
#====================================================================

if DerivationFrameworkIsMonteCarlo:
   # Schedule the two energy density tools for running after the pseudojets are created.
   for alg in ['EDTruthCentralAlg', 'EDTruthForwardAlg']:
      if hasattr(topSequence, alg):
         edtalg = getattr(topSequence, alg)
         delattr(topSequence, alg)
         SeqFTAG6 += edtalg

#====================================================================
# Add our sequence to the top sequence
#====================================================================
# Ideally, this should come at the end of the job
DerivationFrameworkJob += SeqFTAG6

#====================================================================
# Tau
#====================================================================
# Add low-pt di-tau reconstruction
from DerivationFrameworkTau.TauCommon import addDiTauLowPt
addDiTauLowPt(Seq=SeqFTAG6)

# Low-pt di-tau thinning
from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__GenericObjectThinning
FTAG6DiTauLowPtThinningTool = DerivationFramework__GenericObjectThinning(name            = "FTAG6DiTauLowPtThinningTool",
                                                                        StreamName      = FTAG6Stream.Name,
                                                                        ContainerName   = "DiTauJetsLowPt",
                                                                        SelectionString = "DiTauJetsLowPt.nSubjets > 1")
ToolSvc += FTAG6DiTauLowPtThinningTool
thinningTools.append(FTAG6DiTauLowPtThinningTool)

# ID tracks associated with low-pt ditau
FTAG6DiTauLowPtTPThinningTool = DerivationFramework__DiTauTrackParticleThinning(name                    = "FTAG6DiTauLowPtTPThinningTool",
                                                                               StreamName              = FTAG6Stream.Name,
                                                                               DiTauKey                = "DiTauJetsLowPt",
                                                                               InDetTrackParticlesKey  = "InDetTrackParticles",
                                                                               SelectionString         = "DiTauJetsLowPt.nSubjets > 1")
ToolSvc += FTAG6DiTauLowPtTPThinningTool
thinningTools.append(FTAG6DiTauLowPtTPThinningTool)

#====================================================================
# CREATE THE DERIVATION KERNEL ALGORITHM
#====================================================================
# Add the kernel for thinning (requires the objects be defined)
#from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__DerivationKernel
SeqFTAG6 += CfgMgr.DerivationFramework__DerivationKernel("FTAG6Kernel",
                                                        ThinningTools = thinningTools)


#====================================================================
# FLAVOUR TAGGING
#====================================================================

from DerivationFrameworkFlavourTag.FtagRun3DerivationConfig import FtagJetCollection

FtagJetCollection('AntiKt4EMPFlowJets',SeqFTAG6)


#====================================================================
# TC-LVT Vertices
#====================================================================

# from SoftBVrtClusterTool.SoftBVrtConfig import addSoftBVrt
# addSoftBVrt(SeqFTAG6,'Loose')
# addSoftBVrt(SeqFTAG6,'Medium')
# addSoftBVrt(SeqFTAG6,'Tight')

#====================================================================
# CONTENTS
#====================================================================
from DerivationFrameworkCore.SlimmingHelper import SlimmingHelper
FTAG6SlimmingHelper = SlimmingHelper("FTAG6SlimmingHelper")

FTAG6SlimmingHelper.SmartCollections = ["Electrons",
                                       "Photons",
                                       "Muons",
                                       "PrimaryVertices",
                                       "InDetTrackParticles",
                                       "AntiKt4EMTopoJets",
                                       "AntiKt4EMPFlowJets",
                                       "BTagging_AntiKt4EMPFlow",
                                       "MET_Baseline_AntiKt4EMTopo",
                                       "MET_Baseline_AntiKt4EMPFlow",
                                       "TauJets",
                                       "DiTauJets",
                                       "DiTauJetsLowPt",
                                       "AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets",
                                       #"AntiKtVR30Rmax4Rmin02TrackJets_BTagging201903",
                                       #"BTagging_AntiKtVR30Rmax4Rmin02Track_201903"
                                      ]

excludedVertexAuxData = "-vxTrackAtVertex.-MvfFitInfo.-isInitialized.-VTAV"
StaticContent = []
StaticContent += ["xAOD::VertexContainer#SoftBVrtClusterTool_Tight_Vertices"]
StaticContent += ["xAOD::VertexAuxContainer#SoftBVrtClusterTool_Tight_VerticesAux." + excludedVertexAuxData]
StaticContent += ["xAOD::VertexContainer#SoftBVrtClusterTool_Medium_Vertices"]
StaticContent += ["xAOD::VertexAuxContainer#SoftBVrtClusterTool_Medium_VerticesAux." + excludedVertexAuxData]
StaticContent += ["xAOD::VertexContainer#SoftBVrtClusterTool_Loose_Vertices"]
StaticContent += ["xAOD::VertexAuxContainer#SoftBVrtClusterTool_Loose_VerticesAux." + excludedVertexAuxData]

FTAG6SlimmingHelper.StaticContent = StaticContent

# Trigger content
FTAG6SlimmingHelper.IncludeTriggerNavigation = False
FTAG6SlimmingHelper.IncludeJetTriggerContent = False
FTAG6SlimmingHelper.IncludeMuonTriggerContent = False
FTAG6SlimmingHelper.IncludeEGammaTriggerContent = False
FTAG6SlimmingHelper.IncludeJetTauEtMissTriggerContent = False
FTAG6SlimmingHelper.IncludeTauTriggerContent = False
FTAG6SlimmingHelper.IncludeEtMissTriggerContent = False
FTAG6SlimmingHelper.IncludeBJetTriggerContent = False
FTAG6SlimmingHelper.IncludeBPhysTriggerContent = False
FTAG6SlimmingHelper.IncludeMinBiasTriggerContent = False

# Add the jet containers to the stream (defined in JetCommon if import needed)
#addJetOutputs(FTAG6SlimmingHelper,["FTAG6"])

# Truth containers
if DerivationFrameworkIsMonteCarlo:
   FTAG6SlimmingHelper.AppendToDictionary = {'TruthEvents':'xAOD::TruthEventContainer','TruthEventsAux':'xAOD::TruthEventAuxContainer',
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
                                            'AntiKt10TruthTrimmedPtFrac5SmallR20Jets':'xAOD::JetContainer', 'AntiKt10TruthTrimmedPtFrac5SmallR20JetsAux':'xAOD::JetAuxContainer'
                                           }

   from DerivationFrameworkMCTruth.MCTruthCommon import addTruth3ContentToSlimmerTool
   addTruth3ContentToSlimmerTool(FTAG6SlimmingHelper)
   FTAG6SlimmingHelper.AllVariables += ['TruthHFWithDecayParticles','TruthHFWithDecayVertices','TruthCharm']

FTAG6SlimmingHelper.ExtraVariables += ["AntiKt10TruthTrimmedPtFrac5SmallR20Jets.Tau1_wta.Tau2_wta.Tau3_wta.D2.GhostBHadronsFinalCount",
                                      "Electrons.TruthLink",
                                      "Muons.TruthLink",
                                      "Photons.TruthLink",
                                      "AntiKt2PV0TrackJets.pt.eta.phi.m",
                                      "AntiKt4EMTopoJets.DFCommonJets_QGTagger_truthjet_nCharged.DFCommonJets_QGTagger_truthjet_pt.DFCommonJets_QGTagger_truthjet_eta.DFCommonJets_QGTagger_NTracks.DFCommonJets_QGTagger_TracksWidth.DFCommonJets_QGTagger_TracksC1.PartonTruthLabelID",
                                      "AntiKt4EMPFlowJets.DFCommonJets_QGTagger_truthjet_nCharged.DFCommonJets_QGTagger_truthjet_pt.DFCommonJets_QGTagger_truthjet_eta.DFCommonJets_QGTagger_NTracks.DFCommonJets_QGTagger_TracksWidth.DFCommonJets_QGTagger_TracksC1.PartonTruthLabelID.DFCommonJets_fJvt",
                                      "TruthPrimaryVertices.t.x.y.z"]

# Add trigger matching
trigmatching_helper_notau.add_to_slimming(FTAG6SlimmingHelper)
trigmatching_helper_tau.add_to_slimming(FTAG6SlimmingHelper)

# Final construction of output stream
FTAG6SlimmingHelper.AppendContentToStream(FTAG6Stream)
