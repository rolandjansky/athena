# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

#====================================================================
# FtagCommon.py
# This is to define common objects used by PHYSVAL, FTAG1 and FTAG2.
#====================================================================

from DerivationFrameworkCore.DerivationFrameworkMaster import DerivationFrameworkIsMonteCarlo
from DerivationFrameworkMCTruth.MCTruthCommon import addTruth3ContentToSlimmerTool
from DerivationFrameworkEGamma.ElectronsCPDetailedContent import GSFTracksCPDetailedContent

## Common items used in PHYSVAL, FTAG1 and FTAG2
PHYSVAL_FTAG1_FTAG2_mc_AppendToDictionary = {
        'TruthEvents':'xAOD::TruthEventContainer','TruthEventsAux':'xAOD::TruthEventAuxContainer',
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
        'AntiKt10PV0TrackJets':'xAOD::JetContainer','AntiKt10PV0TrackJetsAux':'xAOD::JetAuxContainer',
        'BTagging_AntiKtVR30Rmax4Rmin02Track':'xAOD::BTaggingContainer','BTagging_AntiKtVR30Rmax4Rmin02TrackAux':'xAOD::BTaggingAuxContainer',
        'EMOriginTopoClusters':'xAOD::CaloClusterContainer', 'EMOriginTopoClustersAux':'xAOD::ShallowAuxContainer',
        'LCOriginTopoClusters':'xAOD::CaloClusterContainer', 'LCOriginTopoClustersAux':'xAOD::ShallowAuxContainer',
        'BTagging_AntiKt4EMPFlowJFVtx':'xAOD::BTagVertexContainer','BTagging_AntiKt4EMPFlowJFVtxAux':'xAOD::BTagVertexAuxContainer',
        'BTagging_AntiKt4EMPFlowSecVtx':'xAOD::VertexContainer','BTagging_AntiKt4EMPFlowSecVtxAux':'xAOD::VertexAuxContainer',
        'CHSChargedParticleFlowObjects': 'xAOD::FlowElementContainer', 'CHSChargedParticleFlowObjectsAux':'xAOD::ShallowAuxContainer',
        'CHSNeutralParticleFlowObjects': 'xAOD::FlowElementContainer', 'CHSNeutralParticleFlowObjectsAux':'xAOD::ShallowAuxContainer',
        }

PHYSVAL_FTAG1_FTAG2_ExtraVariables = ["AntiKt10TruthTrimmedPtFrac5SmallR20Jets.Tau1_wta.Tau2_wta.Tau3_wta.D2.GhostBHadronsFinalCount",
        "Electrons.TruthLink",
        "Muons.TruthLink",
        "Photons.TruthLink",
        "AntiKt2PV0TrackJets.pt.eta.phi.m",
        "AntiKt4EMTopoJets.DFCommonJets_QGTagger_truthjet_nCharged.DFCommonJets_QGTagger_truthjet_pt.DFCommonJets_QGTagger_truthjet_eta.DFCommonJets_QGTagger_NTracks.DFCommonJets_QGTagger_TracksWidth.DFCommonJets_QGTagger_TracksC1.PartonTruthLabelID.GhostBHadronsFinalPt.GhostBHadronsFinalCount.GhostCHadronsFinalCount.GhostCHadronsFinalPt",
        "AntiKt4EMPFlowJets.DFCommonJets_QGTagger_truthjet_nCharged.DFCommonJets_QGTagger_truthjet_pt.DFCommonJets_QGTagger_truthjet_eta.DFCommonJets_QGTagger_NTracks.DFCommonJets_QGTagger_TracksWidth.DFCommonJets_QGTagger_TracksC1.PartonTruthLabelID.DFCommonJets_fJvt.GhostBHadronsFinalPt.GhostBHadronsFinalCount.GhostCHadronsFinalCount.GhostCHadronsFinalPt.HadronConeExclTruthLabelPt.SumPtChargedPFOPt1000.SumPtTrkPt1000.TrackSumMass.TrackSumPt.TrackWidthPt500.TracksForBTagging.TruthLabelDeltaR_B.TruthLabelDeltaR_C.TruthLabelDeltaR_T.JetEMScaleMomentum_pt.JetEMScaleMomentum_eta.HECQuality.GhostHBosonsPt",
        "TruthPrimaryVertices.t.x.y.z",
        "TauNeutralParticleFlowObjects.pt.eta.phi.m.bdtPi0Score.nPi0Proto",
        "TauChargedParticleFlowObjects.pt.eta.phi.m.bdtPi0Score",
        "MET_Track.sumet",
        ]

excludedVertexAuxData = "-vxTrackAtVertex.-MvfFitInfo.-isInitialized.-VTAV"
PHYSVAL_FTAG1_FTAG2_StaticContent = []
PHYSVAL_FTAG1_FTAG2_StaticContent += ["xAOD::VertexContainer#SoftBVrtClusterTool_Tight_Vertices"]
PHYSVAL_FTAG1_FTAG2_StaticContent += ["xAOD::VertexAuxContainer#SoftBVrtClusterTool_Tight_VerticesAux." + excludedVertexAuxData]
PHYSVAL_FTAG1_FTAG2_StaticContent += ["xAOD::VertexContainer#SoftBVrtClusterTool_Medium_Vertices"]
PHYSVAL_FTAG1_FTAG2_StaticContent += ["xAOD::VertexAuxContainer#SoftBVrtClusterTool_Medium_VerticesAux." + excludedVertexAuxData]
PHYSVAL_FTAG1_FTAG2_StaticContent += ["xAOD::VertexContainer#SoftBVrtClusterTool_Loose_Vertices"]
PHYSVAL_FTAG1_FTAG2_StaticContent += ["xAOD::VertexAuxContainer#SoftBVrtClusterTool_Loose_VerticesAux." + excludedVertexAuxData]
PHYSVAL_FTAG1_FTAG2_StaticContent += ["xAOD::VertexAuxContainer#BTagging_AntiKt4EMPFlowSecVtxAux.-vxTrackAtVertex"]


## Common functions used in PHYSVAL, FTAG1 and FTAG2
def add_static_content_to_SlimmingHelper(SlimmingHelper):
    SlimmingHelper.StaticContent = PHYSVAL_FTAG1_FTAG2_StaticContent

def add_truth_to_SlimmingHelper(SlimmingHelper):
    if DerivationFrameworkIsMonteCarlo:
        SlimmingHelper.AppendToDictionary = PHYSVAL_FTAG1_FTAG2_mc_AppendToDictionary
        addTruth3ContentToSlimmerTool(SlimmingHelper)
        SlimmingHelper.AllVariables += ['TruthHFWithDecayParticles','TruthHFWithDecayVertices','TruthCharm']

def add_ExtraVariables_to_SlimmingHelper(SlimmingHelper):
    SlimmingHelper.ExtraVariables += PHYSVAL_FTAG1_FTAG2_ExtraVariables
    SlimmingHelper.ExtraVariables += GSFTracksCPDetailedContent

## Common function used in FTAG1 and FTAG2
def trigger_setup(SlimmingHelper, option=''):
    SlimmingHelper.IncludeTriggerNavigation = False
    SlimmingHelper.IncludeJetTriggerContent = False
    SlimmingHelper.IncludeMuonTriggerContent = False
    SlimmingHelper.IncludeEGammaTriggerContent = False
    SlimmingHelper.IncludeJetTauEtMissTriggerContent = False
    SlimmingHelper.IncludeTauTriggerContent = False
    SlimmingHelper.IncludeEtMissTriggerContent = False
    SlimmingHelper.IncludeBJetTriggerContent = False
    SlimmingHelper.IncludeBPhysTriggerContent = False
    SlimmingHelper.IncludeMinBiasTriggerContent = False
    if option == 'FTAG2':
        SlimmingHelper.IncludeMuonTriggerContent = True
        SlimmingHelper.IncludeEGammaTriggerContent = True
        SlimmingHelper.IncludeBJetTriggerContent = True
        SlimmingHelper.IncludeBPhysTriggerContent = True



