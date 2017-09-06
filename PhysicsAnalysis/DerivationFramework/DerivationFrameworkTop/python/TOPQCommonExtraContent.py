# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

#====================================================================
# Common file used by TOPQCommonSlimming.py
# Access variables with:
#   from DerivationFrameworkTop.TOPQCommonExtraContent import *
# Available variables (arrays):
#   TOPQSmartSlimmingCollections
#   TOPQExtraVariablesPhotons
#   TOPQExtraVariablesElectrons
#   TOPQExtraVariablesMuons
#   TOPQExtraVariablesTaus
#   TOPQExtraVariablesAntiKt4EMTopoJets
#   TOPQExtraVariablesAntiKt4EMPFlowJets
#   TOPQExtraVariablesBTagging_AntiKt4EMPFlow
#   TOPQExtraVariablesPhotonsTruth
#   TOPQExtraVariablesElectronsTruth
#   TOPQExtraVariablesMuonsTruth
#   TOPQExtraVariablesTausTruth
#   TOPQExtraVarsBTag_HLT_Container
#   TOPQExtraVarsJet_EF_Container
#   TOPQExtraVarsJet_Split_Container
#   TOPQExtraContainersTrigger
#   TOPQExtraContainersStandard
#   TOPQExtraContainersTruth
#   TOPQStaticContent
#   TOPQStaticContentTruth
#====================================================================

#================================
# SMART SLIMMING COLLECTIONS
#================================
TOPQSmartSlimmingCollections = ["AntiKt4EMPFlowJets",
                                "AntiKt4EMTopoJets",
                                "BTagging_AntiKt2Track",
                                "BTagging_AntiKt4EMTopo",
                                "Electrons",
                                "InDetTrackParticles",
                                "MET_Reference_AntiKt4EMTopo",
                                "MET_Reference_AntiKt4EMPFlow",
                                "Muons",
                                "Photons",
                                "PrimaryVertices",
                                "TauJets"]

#================================
# EXTRA VARIABLES - Data & MC
#================================
TOPQExtraVariablesPhotons    = ["Photons."
                               + "Loose."
                               + "Medium."
                               + "Tight."
                               + "author."
                               + "OQ"]

TOPQExtraVariablesElectrons  = ["Electrons."
                               + "etcone20."
                               + "etcone30."
                               + "etcone40."
                               + "ptcone20."
                               + "ptcone30."
                               + "ptcone40."
                               + "Loose."
                               + "Medium."
                               + "Tight."
                               + "DFCommonElectronsLHLoose."
                               + "DFCommonElectronsLHMedium."
                               + "DFCommonElectronsLHTight."
                               + "DFCommonElectronsML."
                               + "author."
                               + "OQ."
                               + "firstEgMotherTruthType."
                               + "firstEgMotherTruthOrigin."
                               + "firstEgMotherPdgId."
                               + "deltaPhi1"]

TOPQExtraVariablesMuons      = ["Muons."
                               + "DFCommonGoodMuon."
                               + "DFCommonMuonsLoose."
                               + "DFCommonMuonsMedium."
                               + "DFCommonMuonsTight."
                               + "ptcone20."
                               + "ptcone30."
                               + "ptcone40."
                               + "etcone20."
                               + "etcone30."
                               + "etcone40"]

TOPQExtraVariablesTaus   = []

TOPQExtraVariablesAntiKt4EMTopoJets =  ["AntiKt4EMTopoJets."
                                       + "DFCommonJets_Calib_pt."
                                       + "DFCommonJets_Calib_eta."
                                       + "DFCommonJets_Calib_phi."
                                       + "DFCommonJets_Calib_m"]

TOPQExtraVariablesAntiKt4EMPFlowJets = ["AntiKt4EMPFlowJets."
                                       + "AntiKt2TrackJet."
                                       + "AverageLArQF."
                                       + "BchCorrCell."
                                       + "btagging."
                                       + "btaggingLink."
                                       + "ConeExclBHadronsFinal."
                                       + "ConeExclCHadronsFinal."
                                       + "ConeExclTausFinal."
                                       + "ConeTruthLabelID."
                                       + "constituentLinks."
                                       + "DetectorEta."
                                       + "eta."
                                       + "EMFrac."
                                       + "FracSamplingMax."
                                       + "FracSamplingMaxIndex."
                                       + "GhostAntiKt2TrackJet."
                                       + "GhostAntiKt3TrackJet."
                                       + "GhostAntiKt4TrackJet."
                                       + "GhostBHadronsFinal."
                                       + "GhostBHadronsInitial."
                                       + "GhostBQuarksFinal."
                                       + "GhostCHadronsFinal."
                                       + "GhostCHadronsInitial."
                                       + "GhostCQuarksFinal."
                                       + "GhostHBosons."
                                       + "GhostMuonSegment."
                                       + "GhostPartons."
                                       + "GhostTausFinal."
                                       + "GhostTQuarksFinal."
                                       + "GhostTrack."
                                       + "GhostTrackCount."
                                       + "GhostTrackPt."
                                       + "GhostTruth."
                                       + "GhostTruthAssociationLink."
                                       + "GhostWBosons."
                                       + "GhostZBosons."
                                       + "HECFrac."
                                       + "HECQuality."
                                       + "HadronConeExclTruthLabelID."
                                       + "HighestJVFLooseVtx."
                                       + "HighestJVFVtx."
                                       + "HighestJVFVtxGhost."
                                       + "JetConstitScaleMomentum_eta."
                                       + "JetConstitScaleMomentum_m."
                                       + "JetConstitScaleMomentum_phi."
                                       + "JetConstitScaleMomentum_pt."
                                       + "JetLCScaleMomentum_eta."
                                       + "JetLCScaleMomentum_m."
                                       + "JetLCScaleMomentum_phi."
                                       + "JetLCScaleMomentum_pt."
                                       + "Jvt."
                                       + "JvtJvfcorr."
                                       + "JvtRpt."
                                       + "LArQuality."
                                       + "m."
                                       + "NegativeE."
                                       + "originalObjectLink."
                                       + "OriginVertex."
                                       + "PartonTruthLabelID."
                                       + "phi."
                                       + "pt."
                                       + "SumPtTrkPt1000."
                                       + "SumPtTrkPt500."
                                       + "Timing."
                                       + "TruthLabelID"]

TOPQExtraVariablesBTagging_AntiKt4EMPFlow = ["BTagging_AntiKt4EMPFlow."
                                            + "BTagTrackToJetAssociator."
                                            + "BTagTrackToJetAssociatorBB."
                                            + "IP3D_pb."
                                            + "IP3D_pu."
                                            + "JetFitter_JFvertices."
                                            + "JetFitter_tracksAtPVlinks."
                                            + "MSV_badTracksIP."
                                            + "MSV_vertices."
                                            + "MV1c_discriminant."
                                            + "MV1_discriminant."
                                            + "MV2c00_discriminant."
                                            + "MV2c100_discriminant."
                                            + "MV2c10_discriminant."
                                            + "MV2c20_discriminant."
                                            + "MV2m_pb."
                                            + "MV2m_pc."
                                            + "MV2m_pu."
                                            + "MVb_discriminant."
                                            + "SV0_badTracksIP."
                                            + "SV0_vertices."
                                            + "SV1_badTracksIP."
                                            + "SV1_pb."
                                            + "SV1_pu."
                                            + "SV1_vertices"]

TOPQExtraVariablesBTagging_AntiKt4EMTopo = ["BTagging_AntiKt4EMTopo."
                                           + "SV1_TrackParticleLinks."
                                           + "MV2cl100_discriminant."
                                           + "JetVertexCharge_discriminant."
                                           + "MultiSVbb1_discriminant."
                                           + "MultiSVbb2_discriminant."
                                           + "MSV_N2Tpair."
                                           + "MSV_energyTrkInJet."
                                           + "MSV_nvsec."
                                           + "MSV_vtxmass."
                                           + "MSV_vtxpt."
                                           + "MSV_vtxeta."
                                           + "MSV_vtxphi."
                                           + "MSV_vtxefrac."
                                           + "MSV_vtxx."
                                           + "MSV_vtxy."
                                           + "MSV_vtxz."
                                           + "MSV_vtxdls."
                                           + "MSV_vtxntrk"]

TOPQExtraVariablesTrackJets = ["AntiKt2PV0TrackJets."
                               +"pt."
                               +"eta."
                               +"phi."
                               +"m.constituentLinks."
                               +"btaggingLink."
                               +"GhostBHadronsFinal."
                               +"GhostBHadronsInitial."
                               +"GhostBQuarksFinal."
                               +"GhostCHadronsFinal."
                               +"GhostCHadronsInitial."
                               +"GhostCQuarksFinal."
                               +"GhostHBosons."
                               +"GhostPartons."
                               +"GhostTQuarksFinal."
                               +"GhostTausFinal."
                               +"GhostWBosons."
                               +"GhostZBosons."
                               +"GhostTruth."
                               +"OriginVertex."
                               +"GhostAntiKt3TrackJet."
                               +"GhostAntiKt4TrackJet."
                               +"GhostTrack."
                               +"GhostTruthAssociationLink"]


#================================
# EXTRA VARIABLES - MC Only
#================================
TOPQExtraVariablesPhotonsTruth   = ["Photons."
                                   + "truthOrigin."
                                   + "truthParticleLink."
                                   + "truthType"]

TOPQExtraVariablesElectronsTruth = ["Electrons."
                                   + "bkgTruthType."
                                   + "bkgTruthOrigin."
                                   + "bkgTruthParticleLink."
                                   + "bkgMotherPdgId"
                                   + "truthOrigin."
                                   + "truthType."
                                   + "truthParticleLink"]

TOPQExtraVariablesMuonsTruth     = ["MuonTruthParticles."
                                   + "truthOrigin."
                                   + "truthType"]

TOPQExtraVariablesTausTruth      = ["TauJets."
                                   + "IsTruthMatched."
                                   + "truthJetLink."
                                   + "truthParticleLink"]

TOPQExtraVarsBTag_HLT_Container = ["HLT_xAOD__BTaggingContainer_HLTBjetFex."
                                  + "MV2c00_discriminant."
                                  + "MV2c10_discriminant."
                                  + "MV2c20_discriminant."
                                  + "BTagBtagToJetAssociator"]

TOPQExtraVarsJet_EF_Container = ["HLT_xAOD__JetContainer_EFJet."
                                + "eta."
                                + "m."
                                + "pt."
                                + "phi."]

TOPQExtraVarsJet_Split_Container = ["HLT_xAOD__JetContainer_SplitJet."
                                   + "eta."
                                   + "m."
                                   + "pt."
                                   + "phi."]

#===========================================
# COLLECTIONS WITH All VARIABLES
#===========================================
TOPQExtraContainersStandard = ["CombinedMuonTrackParticles",
                               "egammaClusters",
                               "ExtrapolatedMuonTrackParticles",
                               "ForwardElectrons",
                               # "GSFTrackParticles",
                               "InDetForwardTrackParticles",
                               "MuonSpectrometerTrackParticles",
                               "MET_Track"]

TOPQExtraContainersTrigger = []

TOPQExtraContainersTruth = ["AntiKt10TruthJets",
                            "AntiKt4TruthJets",
                            "AntiKt4TruthWZJets",
                            "MET_Truth",
                            "MET_TruthRegions",
                            "TruthEvents",
                            "TruthParticles",
                            "TruthVertices"]

#================================
# CREATED ON-THE-FLY COLLECTIONS
#================================
TOPQStaticContent      = ['xAOD::JetContainer#AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets',
                          'xAOD::JetAuxContainer#AntiKt10LCTopoTrimmedPtFrac5SmallR20JetsAux.',
                          'xAOD::JetContainer#AntiKt10TruthTrimmedPtFrac5SmallR20Jets',
                          'xAOD::JetAuxContainer#AntiKt10TruthTrimmedPtFrac5SmallR20JetsAux.']

TOPQStaticContentTruth = ['xAOD::TruthParticleContainer#TruthElectrons',
                          'xAOD::TruthParticleAuxContainer#TruthElectronsAux.',
                          'xAOD::TruthParticleContainer#TruthMuons',
                          'xAOD::TruthParticleAuxContainer#TruthMuonsAux.',
                          'xAOD::TruthParticleContainer#TruthPhotons',
                          'xAOD::TruthParticleAuxContainer#TruthPhotonsAux.',
                          'xAOD::TruthParticleContainer#TruthTaus',
                          'xAOD::TruthParticleAuxContainer#TruthTausAux.',
                          'xAOD::TruthParticleContainer#TruthNeutrinos',
                          'xAOD::TruthParticleAuxContainer#TruthNeutrinosAux.']
