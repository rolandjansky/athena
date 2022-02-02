# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

#====================================================================
# DAOD_PHYSVAL.py
# This defines DAOD_PHYSVAL, for running physics validation of
# DAOD-level containers
# It uses the same high-level content as DAOD_PHYS but also includes
# It requires the reductionConf flag PHYS in Reco_tf.py   
#====================================================================

from typing import Sequence
from DerivationFrameworkCore.DerivationFrameworkMaster import buildFileName, DerivationFrameworkIsMonteCarlo, DerivationFrameworkJob
from DerivationFrameworkInDet import InDetCommon
from DerivationFrameworkEGamma import EGammaCommon
from DerivationFrameworkEGamma.ElectronsCPDetailedContent import (
   GSFTracksCPDetailedContent)
from DerivationFrameworkMuons import MuonsCommon
# Common augmentations ("DFCommon")
InDetCommon.makeInDetDFCommon()
EGammaCommon.makeEGammaDFCommon()
MuonsCommon.makeMuonsDFCommon()
from DerivationFrameworkJetEtMiss.JetCommon import addEventCleanFlags, addDAODJets, addQGTaggerTool, getPFlowfJVT, addJetTruthLabel, addVRTrackJetMoments
from JetRecConfig.StandardSmallRJets import AntiKt4EMTopo,AntiKt4LCTopo,AntiKt4EMPFlow,AntiKtVR30Rmax4Rmin02PV0Track
from JetRecConfig.StandardLargeRJets import AntiKt10LCTopoTrimmed
from DerivationFrameworkJetEtMiss.METCommon import scheduleStandardMETContent
from TriggerMenuMT.TriggerAPI.TriggerAPI import TriggerAPI
from TriggerMenuMT.TriggerAPI.TriggerEnums import TriggerPeriod, TriggerType
from DerivationFrameworkTrigger.TriggerMatchingHelper import TriggerMatchingHelper
from DerivationFrameworkTrigger.TrigSlimmingHelper import addTrigEDMSetToOutput
from TrkDetDescrSvc.AtlasTrackingGeometrySvc import AtlasTrackingGeometrySvc

#====================================================================
# SET UP STREAM   
#====================================================================
streamName = derivationFlags.WriteDAOD_PHYSVALStream.StreamName
fileName   = buildFileName( derivationFlags.WriteDAOD_PHYSVALStream )
PHYSVALStream = MSMgr.NewPoolRootStream( streamName, fileName )
PHYSVALStream.AcceptAlgs(["PHYSVALKernel"])

### Augmentation tools lists
AugmentationTools   = []

# Special sequence 
SeqPHYSVAL = CfgMgr.AthSequencer("SeqPHYSVAL")

#====================================================================
# MONTE CARLO TRUTH
#====================================================================
if (DerivationFrameworkIsMonteCarlo):
   from DerivationFrameworkMCTruth.MCTruthCommon import addStandardTruthContents,addMiniTruthCollectionLinks,addHFAndDownstreamParticles,addPVCollection
   #import DerivationFrameworkHiggs.TruthCategories
   # Add charm quark collection
   from DerivationFrameworkMCTruth.DerivationFrameworkMCTruthConf import DerivationFramework__TruthCollectionMaker
   PHYSVALTruthCharmTool = DerivationFramework__TruthCollectionMaker(name                    = "PHYSVALTruthCharmTool",
                                                                  NewCollectionName       = "TruthCharm",
                                                                  KeepNavigationInfo      = False,
                                                                  ParticleSelectionString = "(abs(TruthParticles.pdgId) == 4)",
                                                                  Do_Compress             = True)
   ToolSvc += PHYSVALTruthCharmTool
   #from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__CommonAugmentation
   SeqPHYSVAL += CfgMgr.DerivationFramework__CommonAugmentation("PHYSVALTruthCharmKernel",AugmentationTools=[PHYSVALTruthCharmTool])
   # Add HF particles
   addHFAndDownstreamParticles(SeqPHYSVAL)
   # Add standard truth
   addStandardTruthContents(SeqPHYSVAL,prefix='PHYSVAL_')
   # Update to include charm quarks and HF particles - require a separate instance to be train safe
   from DerivationFrameworkMCTruth.DerivationFrameworkMCTruthConf import DerivationFramework__TruthNavigationDecorator
   PHYSVALTruthNavigationDecorator = DerivationFramework__TruthNavigationDecorator( name="PHYSVALTruthNavigationDecorator",
          InputCollections=["TruthElectrons", "TruthMuons", "TruthPhotons", "TruthTaus", "TruthNeutrinos", "TruthBSM", "TruthBottom", "TruthTop", "TruthBoson","TruthCharm","TruthHFWithDecayParticles"])
   ToolSvc += PHYSVALTruthNavigationDecorator
   SeqPHYSVAL.PHYSVAL_MCTruthNavigationDecoratorKernel.AugmentationTools = [PHYSVALTruthNavigationDecorator]
   # Re-point links on reco objects
   addMiniTruthCollectionLinks(SeqPHYSVAL)
   addPVCollection(SeqPHYSVAL)
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
trigmatching_helper_notau = TriggerMatchingHelper(name='PHYSVALTriggerMatchingToolNoTau',
        trigger_list = trigger_names_notau, add_to_df_job=True)
trigmatching_helper_tau = TriggerMatchingHelper(name='PHYSVALTriggerMatchingToolTau',
        trigger_list = trigger_names_tau, add_to_df_job=True, DRThreshold=0.2)



#====================================================================
# JET/MET   
#====================================================================

jetList = [AntiKt4EMTopo,AntiKt4LCTopo,AntiKt4EMPFlow,
           AntiKtVR30Rmax4Rmin02PV0Track,
           AntiKt10LCTopoTrimmed]

addDAODJets(jetList,SeqPHYSVAL)

# Add large-R jet truth labeling
if (DerivationFrameworkIsMonteCarlo):
   addJetTruthLabel(jetalg="AntiKt10LCTopoTrimmedPtFrac5SmallR20",sequence=SeqPHYSVAL,labelname="R10TruthLabel_R21Consolidated")
   addJetTruthLabel(jetalg="AntiKt10LCTopoTrimmedPtFrac5SmallR20",sequence=SeqPHYSVAL,labelname="R10TruthLabel_R21Precision")

addQGTaggerTool(jetalg="AntiKt4EMTopo",sequence=SeqPHYSVAL)
addQGTaggerTool(jetalg="AntiKt4EMPFlow",sequence=SeqPHYSVAL)

# fJVT
getPFlowfJVT(jetalg='AntiKt4EMPFlow',sequence=SeqPHYSVAL)

# Event cleaning flags
addEventCleanFlags(sequence=SeqPHYSVAL)

scheduleStandardMETContent(sequence=SeqPHYSVAL, algname="METAssociationAlg")

#====================================================================
# Add our sequence to the top sequence
#====================================================================
DerivationFrameworkJob += SeqPHYSVAL


#====================================================================
# CREATE THE DERIVATION KERNEL ALGORITHM   
#====================================================================
# Add the kernel for thinning (requires the objects be defined)
SeqPHYSVAL += CfgMgr.DerivationFramework__DerivationKernel("PHYSVALKernel")


#====================================================================
# FLAVOUR TAGGING   
#====================================================================

from DerivationFrameworkFlavourTag.FtagRun3DerivationConfig import FtagJetCollections
FtagJetCollections(['AntiKt4EMPFlowJets','AntiKtVR30Rmax4Rmin02TrackJets'],SeqPHYSVAL)

#====================================================================
# TC-LVT Vertices 
#====================================================================

# from SoftBVrtClusterTool.SoftBVrtConfig import addSoftBVrt
# addSoftBVrt(SeqPHYSVAL,'Loose')
# addSoftBVrt(SeqPHYSVAL,'Medium')
# addSoftBVrt(SeqPHYSVAL,'Tight')

#====================================================================
# CONTENTS   
#====================================================================
from DerivationFrameworkCore.SlimmingHelper import SlimmingHelper
PHYSVALSlimmingHelper = SlimmingHelper("PHYSVALSlimmingHelper")

PHYSVALSlimmingHelper.SmartCollections = ["Electrons",
                                       "Photons",
                                       "Muons",
                                       "PrimaryVertices",
                                       "InDetTrackParticles",
                                       "AntiKt4EMTopoJets",
                                       "AntiKt4EMPFlowJets",
                                       "BTagging_AntiKt4EMPFlow",
                                       "BTagging_AntiKt4EMTopo",
                                       "BTagging_AntiKtVR30Rmax4Rmin02Track",
                                       "MET_Reference_AntiKt4EMTopo",
                                       "MET_Reference_AntiKt4EMPFlow",
                                       "TauJets",
                                       "DiTauJets",
                                       "DiTauJetsLowPt",
                                       "AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets",
                                       "AntiKtVR30Rmax4Rmin02PV0TrackJets",
                                      ]

PHYSVALSlimmingHelper.AllVariables =  ["EventInfo",
                                       "Electrons", "ForwardElectrons",
                                       "Photons",
                                       "Muons", "CombinedMuonTrackParticles","ExtrapolatedMuonTrackParticles",
                                       "MuonSpectrometerTrackParticles","MSOnlyExtrapolatedMuonTrackParticles","MuonSegments",
                                       "PrimaryVertices",
                                       "InDetTrackParticles","InDetForwardTrackParticles",
                                       "AntiKt4EMTopoJets",
                                       "AntiKt4EMPFlowJets",
                                       "BTagging_AntiKt4EMPFlow",
                                       "BTagging_AntiKt4EMTopo",
                                       "BTagging_AntiKtVR30Rmax4Rmin02Track",
                                       "BTagging_AntiKt4EMPFlowJFVtx", 
				       "BTagging_AntiKt4EMPFlowJFVtxFlip", #Flip version of JetFitter
                                       "BTagging_AntiKt4EMPFlowSecVtx",
                                       "MET_Reference_AntiKt4EMTopo",
                                       "MET_Reference_AntiKt4EMPFlow",
                                       "MET_Reference_AntiKt4LCTopo",
                                       "TauJets",
                                       "DiTauJets",
                                       "DiTauJetsLowPt",
                                       "AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets","AntiKt10LCTopoJets","AntiKt4LCTopoJets",
                                       "TruthParticles", "TruthEvents", "TruthVertices", "MuonTruthParticles", "egammaTruthParticles",
                                       "MuonTruthSegments",
                                       "MET_Truth","MET_TruthRegions",
                                       "TruthElectrons","TruthMuons","TruthPhotons","TruthTaus","TruthNeutrinos","TruthBSM","TruthTop","TruthBoson",
                                       "CaloCalTopoClusters", "EMOriginTopoClusters","LCOriginTopoClusters",
                                       "JetETMissChargedParticleFlowObjects", "JetETMissNeutralParticleFlowObjects"
                                     ]

excludedVertexAuxData = "-vxTrackAtVertex.-MvfFitInfo.-isInitialized.-VTAV"
StaticContent = []
StaticContent += ["xAOD::VertexContainer#SoftBVrtClusterTool_Tight_Vertices"]
StaticContent += ["xAOD::VertexAuxContainer#SoftBVrtClusterTool_Tight_VerticesAux." + excludedVertexAuxData]
StaticContent += ["xAOD::VertexContainer#SoftBVrtClusterTool_Medium_Vertices"]
StaticContent += ["xAOD::VertexAuxContainer#SoftBVrtClusterTool_Medium_VerticesAux." + excludedVertexAuxData]
StaticContent += ["xAOD::VertexContainer#SoftBVrtClusterTool_Loose_Vertices"]
StaticContent += ["xAOD::VertexAuxContainer#SoftBVrtClusterTool_Loose_VerticesAux." + excludedVertexAuxData]
StaticContent += ["xAOD::VertexAuxContainer#BTagging_AntiKt4EMPFlowSecVtxAux.-vxTrackAtVertex"]

PHYSVALSlimmingHelper.StaticContent = StaticContent

# Trigger content
PHYSVALSlimmingHelper.IncludeTriggerNavigation = True
PHYSVALSlimmingHelper.IncludeJetTriggerContent = True
PHYSVALSlimmingHelper.IncludeMuonTriggerContent = True
PHYSVALSlimmingHelper.IncludeEGammaTriggerContent = True
PHYSVALSlimmingHelper.IncludeJetTauEtMissTriggerContent = True
PHYSVALSlimmingHelper.IncludeTauTriggerContent = True
PHYSVALSlimmingHelper.IncludeEtMissTriggerContent = True
PHYSVALSlimmingHelper.IncludeBJetTriggerContent = True
PHYSVALSlimmingHelper.IncludeBPHYSVALTriggerContent = True
PHYSVALSlimmingHelper.IncludeMinBiasTriggerContent = True
addTrigEDMSetToOutput(PHYSVALSlimmingHelper, "AODFULL")

# Truth containers
if DerivationFrameworkIsMonteCarlo:
   PHYSVALSlimmingHelper.AppendToDictionary = {'TruthEvents':'xAOD::TruthEventContainer','TruthEventsAux':'xAOD::TruthEventAuxContainer',
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
                                            'AntiKt10LCTopoJets':'xAOD::JetContainer', 'AntiKt10LCTopoJetsAux':'xAOD::JetAuxContainer',
                                            'BTagging_AntiKtVR30Rmax4Rmin02Track':'xAOD::BTaggingContainer','BTagging_AntiKtVR30Rmax4Rmin02TrackAux':'xAOD::BTaggingAuxContainer',
                                            'EMOriginTopoClusters':'xAOD::CaloClusterContainer', 'EMOriginTopoClustersAux':'xAOD::ShallowAuxContainer',
                                            'LCOriginTopoClusters':'xAOD::CaloClusterContainer', 'LCOriginTopoClustersAux':'xAOD::ShallowAuxContainer',
                                            'BTagging_AntiKt4EMPFlowJFVtx':'xAOD::BTagVertexContainer','BTagging_AntiKt4EMPFlowJFVtxAux':'xAOD::BTagVertexAuxContainer',
                                            'BTagging_AntiKt4EMPFlowSecVtx':'xAOD::VertexContainer','BTagging_AntiKt4EMPFlowSecVtxAux':'xAOD::VertexAuxContainer',
                                            'CHSChargedParticleFlowObjects': 'xAOD::FlowElementContainer', 'CHSChargedParticleFlowObjectsAux':'xAOD::ShallowAuxContainer',
                                            'CHSNeutralParticleFlowObjects': 'xAOD::FlowElementContainer', 'CHSNeutralParticleFlowObjectsAux':'xAOD::ShallowAuxContainer',
					    'BTagging_AntiKt4EMPFlowJFVtxFlip':'xAOD::BTagVertexContainer','BTagging_AntiKt4EMPFlowJFVtxFlipAux':'xAOD::BTagVertexAuxContainer',#For Flip version of JetFitter
                                           }

   from DerivationFrameworkMCTruth.MCTruthCommon import addTruth3ContentToSlimmerTool
   addTruth3ContentToSlimmerTool(PHYSVALSlimmingHelper)
   PHYSVALSlimmingHelper.AllVariables += ['TruthHFWithDecayParticles','TruthHFWithDecayVertices','TruthCharm','CHSChargedParticleFlowObjects', 'CHSNeutralParticleFlowObjects']
   PHYSVALSlimmingHelper.SmartCollections += ['AntiKt4TruthJets']

PHYSVALSlimmingHelper.ExtraVariables += ["AntiKt10TruthTrimmedPtFrac5SmallR20Jets.Tau1_wta.Tau2_wta.Tau3_wta.D2.GhostBHadronsFinalCount",
                                      "Electrons.TruthLink",
                                      "Muons.TruthLink",
                                      "Photons.TruthLink",
                                      "AntiKt2PV0TrackJets.pt.eta.phi.m",
                                      "AntiKt4EMTopoJets.DFCommonJets_QGTagger_truthjet_nCharged.DFCommonJets_QGTagger_truthjet_pt.DFCommonJets_QGTagger_truthjet_eta.DFCommonJets_QGTagger_NTracks.DFCommonJets_QGTagger_TracksWidth.DFCommonJets_QGTagger_TracksC1.PartonTruthLabelID",
                                      "AntiKt4EMPFlowJets.DFCommonJets_QGTagger_truthjet_nCharged.DFCommonJets_QGTagger_truthjet_pt.DFCommonJets_QGTagger_truthjet_eta.DFCommonJets_QGTagger_NTracks.DFCommonJets_QGTagger_TracksWidth.DFCommonJets_QGTagger_TracksC1.PartonTruthLabelID.DFCommonJets_fJvt",
                                      "TruthPrimaryVertices.t.x.y.z",
                                      "TauNeutralParticleFlowObjects.pt.eta.phi.m.bdtPi0Score.nPi0Proto",
                                      "TauChargedParticleFlowObjects.pt.eta.phi.m.bdtPi0Score",
                                      "MET_Track.sumet"
]
PHYSVALSlimmingHelper.ExtraVariables += GSFTracksCPDetailedContent
# Add trigger matching
trigmatching_helper_notau.add_to_slimming(PHYSVALSlimmingHelper)
trigmatching_helper_tau.add_to_slimming(PHYSVALSlimmingHelper)

# Final construction of output stream
PHYSVALSlimmingHelper.AppendContentToStream(PHYSVALStream)

