# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

#====================================================================
# LLP1.py
# This defines DAOD_LLP1, a DAOD format for Run 3.
# It contains the variables and objects needed for the large majority 
# of physics analyses in ATLAS.
# It requires the reductionConf flag LLP1 in Reco_tf.py   
#====================================================================

from AthenaCommon import Logging
nanolog = Logging.logging.getLogger('LLP1')

from DerivationFrameworkCore.DerivationFrameworkMaster import buildFileName
from DerivationFrameworkCore.DerivationFrameworkMaster import DerivationFrameworkIsMonteCarlo, DerivationFrameworkJob
from DerivationFrameworkPhys import PhysCommon
from DerivationFrameworkPhys import PhysCommonTrigger
from DerivationFrameworkEGamma import ElectronsCPDetailedContent
from DerivationFrameworkEGamma import LargeD0ElectronsCPDetailedContent
from DerivationFrameworkJetEtMiss import METCommon
from DerivationFrameworkJetEtMiss.METCommon import scheduleMETAssocAlg
from DerivationFrameworkCore import LHE3WeightMetadata

from TriggerMenuMT.TriggerAPI.TriggerAPI import TriggerAPI
from TriggerMenuMT.TriggerAPI.TriggerEnums import TriggerPeriod, TriggerType


#====================================================================
# Set up sequence for this format and add to the top sequence 
#====================================================================
SeqLLP1 = CfgMgr.AthSequencer("SeqLLP1")
DerivationFrameworkJob += SeqLLP1

#====================================================================
# SET UP STREAM   
#====================================================================
streamName = derivationFlags.WriteDAOD_LLP1Stream.StreamName
fileName   = buildFileName( derivationFlags.WriteDAOD_LLP1Stream )
LLP1Stream = MSMgr.NewPoolRootStream( streamName, fileName )
LLP1Stream.AcceptAlgs(["LLP1Kernel"])

### Thinning, skimming and augmentation tools lists
thinningTools       = []
skimmingTools       = []
augmentationTools   = []

# Special sequence 
SeqLLP1 = CfgMgr.AthSequencer("SeqLLP1")

#====================================================================
# Turn on LRT
#====================================================================
from InDetRecExample.InDetJobProperties import InDetFlags
InDetFlags.doR3LargeD0.set_Value_and_Lock(True)

#====================================================================
# Run the LRT merger
#====================================================================
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__TrackParticleMerger
LRTAndStandardTrackParticleMerger = DerivationFramework__TrackParticleMerger(name                        = "LRTAndStandardTrackParticleMerger",
                                                                             TrackParticleLocation       = ["InDetTrackParticles","InDetLargeD0TrackParticles"],
                                                                             OutputTrackParticleLocation = "InDetWithLRTTrackParticles",
                                                                             CreateViewColllection       = True)

ToolSvc += LRTAndStandardTrackParticleMerger
SeqLLP1 += CfgMgr.DerivationFramework__CommonAugmentation("InDetWithLRTLRTMerge",
                                                                         AugmentationTools = [LRTAndStandardTrackParticleMerger])

#====================================================================
# Run VSI
#====================================================================
from VrtSecInclusive.VrtSecInclusive import VrtSecInclusive
from VrtSecInclusive.VrtSecInclusive_Configuration import setupVSI
from TrkExTools.AtlasExtrapolator import AtlasExtrapolator
from InDetRecExample import TrackingCommon

ToolSvc += AtlasExtrapolator(name     = "AtlasExtrapolator")

ToolSvc += TrackingCommon.getTrackToVertexIPEstimator(name = "LLP1IPETool")
ToolSvc += TrackingCommon.getInDetTrackToVertexTool(name = "LLP1T2VTool")

# set options related to the vertex fitter
from TrkVKalVrtFitter.TrkVKalVrtFitterConf import Trk__TrkVKalVrtFitter
InclusiveVxFitterTool = Trk__TrkVKalVrtFitter(name                = "InclusiveVxFitter",
                                              Extrapolator        = ToolSvc.AtlasExtrapolator,
                                              IterationNumber     = 30,
                                              )
ToolSvc +=  InclusiveVxFitterTool
InclusiveVxFitterTool.OutputLevel = INFO

VrtSecInclusive_InDet = setupVSI( "InDet" )

VrtSecInclusive_InDet.VertexFitterTool             = InclusiveVxFitterTool
VrtSecInclusive_InDet.Extrapolator                 = ToolSvc.AtlasExtrapolator
VrtSecInclusive_InDet.TrackToVertexIPEstimatorTool = ToolSvc.LLP1IPETool
VrtSecInclusive_InDet.TrackToVertexTool            = ToolSvc.LLP1T2VTool
VrtSecInclusive_InDet.FillIntermediateVertices     = False
VrtSecInclusive_InDet.TrackLocation                = "InDetWithLRTTrackParticles"

VrtSecInclusive_InDet.doAugmentDVimpactParametersToMuons = False
VrtSecInclusive_InDet.doAugmentDVimpactParametersToElectrons = False

SeqLLP1 += VrtSecInclusive_InDet


#====================================================================
# SKIMMING
#====================================================================
allperiods = TriggerPeriod.y2015 | TriggerPeriod.y2016 | TriggerPeriod.y2017 | TriggerPeriod.y2018 | TriggerPeriod.future2e34
trig_el  = TriggerAPI.getLowestUnprescaledAnyPeriod(allperiods, triggerType=TriggerType.el,  livefraction=0.8)
trig_mu  = TriggerAPI.getLowestUnprescaledAnyPeriod(allperiods, triggerType=TriggerType.mu,  livefraction=0.8)
trig_g   = TriggerAPI.getLowestUnprescaledAnyPeriod(allperiods, triggerType=TriggerType.g,   livefraction=0.8)
trig_elmu = TriggerAPI.getLowestUnprescaledAnyPeriod(allperiods, triggerType=TriggerType.el, additionalTriggerType=TriggerType.mu,  livefraction=0.8)
trig_mug = TriggerAPI.getLowestUnprescaledAnyPeriod(allperiods, triggerType=TriggerType.mu, additionalTriggerType=TriggerType.g,  livefraction=0.8)


triggers = trig_el + trig_mu + trig_g + trig_elmu + trig_mug
#remove duplicates
triggers = sorted(list(set(triggers)))

#trigger
from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__TriggerSkimmingTool
LLP1TriggerSkimmingTool = DerivationFramework__TriggerSkimmingTool(name = "LLP1TriggerPreSkimmingTool",
                                                              TriggerListAND = [],
                                                              TriggerListOR  = triggers)
ToolSvc += LLP1TriggerSkimmingTool

print('LLP1 list of triggers used for skimming:')
for trig in triggers: print(trig)

skimmingTools.append(LLP1TriggerSkimmingTool)

#====================================================================
# THINNING
#====================================================================
# ID tracks: See recommedations here: 
# https://twiki.cern.ch/twiki/bin/viewauth/AtlasProtected/DaodRecommendations

# Inner detector group recommendations for indet tracks in analysis
LLP1_thinning_expression = "InDetTrackParticles.DFCommonTightPrimary && abs(DFCommonInDetTrackZ0AtPV)*sin(InDetTrackParticles.theta) < 3.0*mm && InDetTrackParticles.pt > 10*GeV"
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__TrackParticleThinning
LLP1TrackParticleThinningTool = DerivationFramework__TrackParticleThinning(name                    = "LLP1TrackParticleThinningTool",
                                                                           StreamName              = LLP1Stream.Name, 
                                                                           SelectionString         = LLP1_thinning_expression,
                                                                           InDetTrackParticlesKey  = "InDetTrackParticles")

ToolSvc += LLP1TrackParticleThinningTool
thinningTools.append(LLP1TrackParticleThinningTool)

# Include inner detector tracks associated with muons
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__MuonTrackParticleThinning
LLP1MuonTPThinningTool = DerivationFramework__MuonTrackParticleThinning(name                    = "LLP1MuonTPThinningTool",
                                                                        StreamName              = LLP1Stream.Name,
                                                                        MuonKey                 = "Muons",
                                                                        InDetTrackParticlesKey  = "InDetTrackParticles")

ToolSvc += LLP1MuonTPThinningTool
thinningTools.append(LLP1MuonTPThinningTool)

# Include LRT inner detector tracks associated with LRT muons
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__MuonTrackParticleThinning
LLP1LRTMuonTPThinningTool = DerivationFramework__MuonTrackParticleThinning(name                    = "LLP1LRRMuonTPThinningTool",
                                                                              StreamName              = LLP1Stream.Name,
                                                                              MuonKey                 = "MuonsLRT",
                                                                              InDetTrackParticlesKey  = "InDetLargeD0TrackParticles")

ToolSvc += LLP1LRTMuonTPThinningTool
thinningTools.append(LLP1LRTMuonTPThinningTool)

# TauJets thinning
# Disabled for 1st production in 2021, to allow use by tau CP group
#tau_thinning_expression = "(TauJets.ptFinalCalib >= 13.*GeV) && (TauJets.nTracks>=1) && (TauJets.nTracks<=3) && (TauJets.RNNJetScoreSigTrans>0.01)"
#from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__GenericObjectThinning
#LLP1TauJetsThinningTool = DerivationFramework__GenericObjectThinning(name            = "LLP1TauJetsThinningTool",
#                                                                     StreamName      = LLP1Stream.Name,
#                                                                     ContainerName   = "TauJets",
#                                                                     SelectionString = tau_thinning_expression)
#ToolSvc += LLP1TauJetsThinningTool
#thinningTools.append(LLP1TauJetsThinningTool)

# Only keep tau tracks (and associated ID tracks) classified as charged tracks
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__TauTrackParticleThinning
LLP1TauTPThinningTool = DerivationFramework__TauTrackParticleThinning(name                   = "LLP1TauTPThinningTool",
                                                                      StreamName             = LLP1Stream.Name,
                                                                      TauKey                 = "TauJets",
                                                                      InDetTrackParticlesKey = "InDetTrackParticles",
#                                                                      SelectionString        = tau_thinning_expression,
                                                                      DoTauTracksThinning    = True,
                                                                      TauTracksKey           = "TauTracks")
ToolSvc += LLP1TauTPThinningTool
thinningTools.append(LLP1TauTPThinningTool)

# ID tracks associated with high-pt di-tau
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__DiTauTrackParticleThinning
LLP1DiTauTPThinningTool = DerivationFramework__DiTauTrackParticleThinning(name                    = "LLP1DiTauTPThinningTool",
                                                                          StreamName              = LLP1Stream.Name,
                                                                          DiTauKey                = "DiTauJets",
                                                                          InDetTrackParticlesKey  = "InDetTrackParticles")
ToolSvc += LLP1DiTauTPThinningTool
thinningTools.append(LLP1DiTauTPThinningTool)

# Low-pt di-tau thinning
from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__GenericObjectThinning
LLP1DiTauLowPtThinningTool = DerivationFramework__GenericObjectThinning(name            = "LLP1DiTauLowPtThinningTool",
                                                                        StreamName      = LLP1Stream.Name,
                                                                        ContainerName   = "DiTauJetsLowPt",
                                                                        SelectionString = "DiTauJetsLowPt.nSubjets > 1")
ToolSvc += LLP1DiTauLowPtThinningTool
thinningTools.append(LLP1DiTauLowPtThinningTool)

# ID tracks associated with low-pt ditau
LLP1DiTauLowPtTPThinningTool = DerivationFramework__DiTauTrackParticleThinning(name                    = "LLP1DiTauLowPtTPThinningTool",
                                                                               StreamName              = LLP1Stream.Name,
                                                                               DiTauKey                = "DiTauJetsLowPt",
                                                                               InDetTrackParticlesKey  = "InDetTrackParticles",
                                                                               SelectionString         = "DiTauJetsLowPt.nSubjets > 1")
ToolSvc += LLP1DiTauLowPtTPThinningTool
thinningTools.append(LLP1DiTauLowPtTPThinningTool)

# ID Tracks associated with jets
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__JetTrackParticleThinning
LLP1JetTPThinningTool = DerivationFramework__JetTrackParticleThinning( name                    = "LLP1JetTPThinningTool",
                                                                          StreamName              = LLP1Stream.Name,
                                                                          JetKey                  = "AntiKt4EMTopoJets",
                                                                          SelectionString         = "(AntiKt4EMTopoJets.pt > 20.*GeV) && (abs(AntiKt4EMTopoJets.eta) < 2.5)",
                                                                          InDetTrackParticlesKey  = "InDetTrackParticles")
ToolSvc += LLP1JetTPThinningTool
thinningTools.append(LLP1JetTPThinningTool)

# LRT Tracks associated with jets
if InDetFlags.doR3LargeD0():
  from DerivationFrameworkLLP.DerivationFrameworkLLPConf import DerivationFramework__JetLargeD0TrackParticleThinning
  LLP1JetLD0TPThinningTool = DerivationFramework__JetLargeD0TrackParticleThinning( name                    = "LLP1JetLD0TPThinningTool",
                                                                                      StreamName              = LLP1Stream.Name,
                                                                                      JetKey                  = "AntiKt4EMTopoJets",
                                                                                      SelectionString         = "(AntiKt4EMTopoJets.pt > 20.*GeV) && (abs(AntiKt4EMTopoJets.eta) < 2.5)",
                                                                                      InDetTrackParticlesKey  = "InDetLargeD0TrackParticles")
  ToolSvc += LLP1JetLD0TPThinningTool
  thinningTools.append(LLP1JetLD0TPThinningTool)

# ID Tracks associated with secondary vertices
from DerivationFrameworkLLP.DerivationFrameworkLLPConf import DerivationFramework__VSITrackParticleThinning
LLP1VSITPThinningTool = DerivationFramework__VSITrackParticleThinning( name                  = "LLP1VSITPThinningTool",
                                                                          StreamName              = LLP1Stream.Name,
                                                                          InDetTrackParticlesKey  = "InDetTrackParticles")
                    
ToolSvc += LLP1VSITPThinningTool
thinningTools.append(LLP1VSITPThinningTool)

from DerivationFrameworkLLP.DerivationFrameworkLLPConf import DerivationFramework__VSITrackParticleThinning
LLP1LD0VSITPThinningTool = DerivationFramework__VSITrackParticleThinning( name                  = "LLP1LD0VSITPThinningTool",
                                                                            StreamName              = LLP1Stream.Name,
                                                                            InDetTrackParticlesKey  = "InDetLargeD0TrackParticles")
ToolSvc += LLP1LD0VSITPThinningTool
thinningTools.append(LLP1LD0VSITPThinningTool)


#====================================================================
# CREATE THE DERIVATION KERNEL ALGORITHM   
#====================================================================
# Add the kernel for thinning (requires the objects be defined)
#from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__DerivationKernel
SeqLLP1 += CfgMgr.DerivationFramework__DerivationKernel("LLP1Kernel",
                                                        SkimmingTools = skimmingTools,
                                                        ThinningTools = thinningTools)

#====================================================================
# FLAVOUR TAGGING   
#====================================================================

from DerivationFrameworkFlavourTag.FtagRun3DerivationConfig import FtagJetCollections
FtagJetCollections(['AntiKt4EMTopoJets'],SeqLLP1)


#====================================================================
# CONTENTS   
#====================================================================
from DerivationFrameworkCore.SlimmingHelper import SlimmingHelper
LLP1SlimmingHelper = SlimmingHelper("LLP1SlimmingHelper")

LLP1SlimmingHelper.SmartCollections = ["EventInfo",
                                       "Electrons",
                                       "LRTElectrons",
                                       "Photons",
                                       "Muons",
                                       "MuonsLRT",
                                       "PrimaryVertices",
                                       "InDetTrackParticles",
                                       "InDetLargeD0TrackParticles",
                                       "AntiKt4EMTopoJets",
                                       "AntiKt4EMPFlowJets",
                                       "BTagging_AntiKt4EMTopo",
                                       "BTagging_AntiKt4EMPFlow",
                                       "BTagging_AntiKtVR30Rmax4Rmin02Track", 
                                       "MET_Baseline_AntiKt4EMTopo",
                                       "MET_Baseline_AntiKt4EMPFlow",
                                       "TauJets",
                                       "DiTauJets",
                                       "DiTauJetsLowPt",
                                       "AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets",
                                       "AntiKtVR30Rmax4Rmin02PV0TrackJets",
                                      ]

#LLP1SlimmingHelper.AllVariables =  [#"CaloCalTopoClusters", 
                                       #"EMOriginTopoClusters",
                                       #"MSDisplacedVertex",
                                       #"MuonSpectrometerTrackParticles",
                                       #"MuonSegments",
                                       #"MSonlyTracklets",
LLP1SlimmingHelper.AllVariables =  [#"LRTElectrons",
                                    #"MuonsLRT",
                                    #"LRTGSFTrackParticles",
                                    #"LRTegammaClusters",
                                    "MuonSegments",
                                      ]


excludedVertexAuxData = "-vxTrackAtVertex.-MvfFitInfo.-isInitialized.-VTAV"
StaticContent = []
StaticContent += ["xAOD::VertexContainer#SoftBVrtClusterTool_Tight_Vertices"]
StaticContent += ["xAOD::VertexAuxContainer#SoftBVrtClusterTool_Tight_VerticesAux." + excludedVertexAuxData]
StaticContent += ["xAOD::VertexContainer#SoftBVrtClusterTool_Medium_Vertices"]
StaticContent += ["xAOD::VertexAuxContainer#SoftBVrtClusterTool_Medium_VerticesAux." + excludedVertexAuxData]
StaticContent += ["xAOD::VertexContainer#SoftBVrtClusterTool_Loose_Vertices"]
StaticContent += ["xAOD::VertexAuxContainer#SoftBVrtClusterTool_Loose_VerticesAux." + excludedVertexAuxData]
StaticContent += ["xAOD::VertexContainer#VrtSecInclusive_SecondaryVertices"]
StaticContent += ["xAOD::VertexAuxContainer#VrtSecInclusive_SecondaryVerticesAux."]

LLP1SlimmingHelper.StaticContent = StaticContent

# Trigger content
LLP1SlimmingHelper.IncludeTriggerNavigation = False
LLP1SlimmingHelper.IncludeJetTriggerContent = False
LLP1SlimmingHelper.IncludeMuonTriggerContent = False
LLP1SlimmingHelper.IncludeEGammaTriggerContent = False
LLP1SlimmingHelper.IncludeJetTauEtMissTriggerContent = False
LLP1SlimmingHelper.IncludeTauTriggerContent = False
LLP1SlimmingHelper.IncludeEtMissTriggerContent = False
LLP1SlimmingHelper.IncludeBJetTriggerContent = False
LLP1SlimmingHelper.IncludeBPhysTriggerContent = False
LLP1SlimmingHelper.IncludeMinBiasTriggerContent = False

# Truth containers
if DerivationFrameworkIsMonteCarlo:
   LLP1SlimmingHelper.AppendToDictionary = {'TruthEvents':'xAOD::TruthEventContainer','TruthEventsAux':'xAOD::TruthEventAuxContainer',
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
   addTruth3ContentToSlimmerTool(LLP1SlimmingHelper)
   LLP1SlimmingHelper.AllVariables += ['TruthHFWithDecayParticles','TruthHFWithDecayVertices','TruthCharm','TruthPileupParticles','InTimeAntiKt4TruthJets','OutOfTimeAntiKt4TruthJets']

LLP1SlimmingHelper.ExtraVariables += ["AntiKt10TruthTrimmedPtFrac5SmallR20Jets.Tau1_wta.Tau2_wta.Tau3_wta.D2.GhostBHadronsFinalCount",
                                      "Electrons.TruthLink",
                                      "Muons.TruthLink",
                                      "Photons.TruthLink",
                                      "AntiKt4EMTopoJets.DFCommonJets_QGTagger_truthjet_nCharged.DFCommonJets_QGTagger_truthjet_pt.DFCommonJets_QGTagger_truthjet_eta.DFCommonJets_QGTagger_NTracks.DFCommonJets_QGTagger_TracksWidth.DFCommonJets_QGTagger_TracksC1.PartonTruthLabelID.ConeExclBHadronsFinal.ConeExclCHadronsFinal.GhostBHadronsFinal.GhostCHadronsFinal.GhostBHadronsFinalCount.GhostBHadronsFinalPt.GhostCHadronsFinalCount.GhostCHadronsFinalPt.GhostBHadronsFinal.GhostCHadronsFinal.GhostTrack.GhostTrackCount.GhostTrackLRT.GhostTrackLRTCount",
                                      "AntiKt4EMPFlowJets.DFCommonJets_QGTagger_truthjet_nCharged.DFCommonJets_QGTagger_truthjet_pt.DFCommonJets_QGTagger_truthjet_eta.DFCommonJets_QGTagger_NTracks.DFCommonJets_QGTagger_TracksWidth.DFCommonJets_QGTagger_TracksC1.PartonTruthLabelID.DFCommonJets_fJvt.ConeExclBHadronsFinal.ConeExclCHadronsFinal.GhostBHadronsFinal.GhostCHadronsFinal.GhostBHadronsFinalCount.GhostBHadronsFinalPt.GhostCHadronsFinalCount.GhostCHadronsFinalPt.GhostBHadronsFinal.GhostCHadronsFinal",
                                     "AntiKtVR30Rmax4Rmin02TrackJets_BTagging201903.GhostBHadronsFinal.GhostCHadronsFinal.GhostBHadronsFinalCount.GhostBHadronsFinalPt.GhostCHadronsFinalCount.GhostCHadronsFinalPt.GhostTausFinal.GhostTausFinalCount",
                                     "AntiKtVR30Rmax4Rmin02TrackJets_BTagging201810.GhostBHadronsFinal.GhostCHadronsFinal.GhostBHadronsFinalCount.GhostBHadronsFinalPt.GhostCHadronsFinalCount.GhostCHadronsFinalPt.GhostTausFinal.GhostTausFinalCount",
                                      "TruthPrimaryVertices.t.x.y.z",
                                      "PrimaryVertices.t.x.y.z",
                                      "InDetTrackParticles.d0.z0.vz.TTVA_AMVFVertices.TTVA_AMVFWeights.eProbabilityHT.is_selected.is_associated.is_svtrk_final.pt_wrtSV.eta_wrtSV.phi_wrtSV.d0_wrtSV.z0_wrtSV.errP_wrtSV.errd0_wrtSV.errz0_wrtSV.chi2_toSV.truthParticleLink.parameterX.parameterY.parameterZ.parameterPX.parameterPY.parameterPZ.parameterPosition.truthMatchProbability.radiusOfFirstHit",
                                      "InDetLargeD0TrackParticles.d0.z0.vz.TTVA_AMVFVertices.TTVA_AMVFWeights.eProbabilityHT.is_selected.is_associated.is_svtrk_final.pt_wrtSV.eta_wrtSV.phi_wrtSV.d0_wrtSV.z0_wrtSV.errP_wrtSV.errd0_wrtSV.errz0_wrtSV.chi2_toSV.truthParticleLink.parameterX.parameterY.parameterZ.parameterPX.parameterPY.parameterPZ.parameterPosition.truthMatchProbability.radiusOfFirstHit",
                                      "EventInfo.hardScatterVertexLink.timeStampNSOffset",
                                      "TauJets.dRmax.etOverPtLeadTrk",
                                      "HLT_xAOD__TrigMissingETContainer_TrigEFMissingET.ex.ey",
                                      "HLT_xAOD__TrigMissingETContainer_TrigEFMissingET_mht.ex.ey"]

# Add trigger matching
# Run 2
PhysCommonTrigger.trigmatching_helper_notau.add_to_slimming(LLP1SlimmingHelper)
PhysCommonTrigger.trigmatching_helper_tau.add_to_slimming(LLP1SlimmingHelper)

# Final construction of output stream
LLP1SlimmingHelper.AppendContentToStream(LLP1Stream)


if not hasattr(ServiceMgr, 'THistSvc'):
    from GaudiSvc.GaudiSvcConf import THistSvc
    ServiceMgr += THistSvc()

ServiceMgr.THistSvc.Output += [ "AANT  DATAFILE='VrtSecInclusive.root' OPT='RECREATE'" ]
