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
                                "BTagging_AntiKt4EMTopo",
                                "Electrons",
                                "InDetTrackParticles",
                                "MET_Reference_AntiKt4EMTopo",
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
                               + "OQ"]

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

TOPQExtraVariablesTaus       = ["TauJets."
                               + "BDTEleScore."
                               + "BDTJetScore."
                               + "caloIso."
                               + "centFrac."
                               + "centFracCorrected."
                               + "charge."
                               + "ChPiEMEOverCaloEME."
                               + "ChPiEMEOverCaloEMECorrected."
                               + "dRmax."
                               + "dRmaxCorrected."
                               + "EMPOverTrkSysP."
                               + "EMPOverTrkSysPCorrected."
                               + "eta."
                               + "etaDetectorAxis."
                               + "etaIntermediateAxis."
                               + "etaPanTauCellBased."
                               + "etaPanTauCellBasedProto."
                               + "etOverPtLeadTrk."
                               + "etOverPtLeadTrkCorrected."
                               + "innerTrkAvgDist."
                               + "innerTrkAvgDistCorrected."
                               + "ipSigLeadTrk."
                               + "ipSigLeadTrkCorrected."
                               + "isolFrac."
                               + "isTauFlags."
                               + "jetLink."
                               + "leadTrkPt."
                               + "m."
                               + "massTrkSys."
                               + "massTrkSys."
                               + "massTrkSysCorrected."
                               + "mDetectorAxis."
                               + "mEflowApprox."
                               + "mEflowApproxCorrected."
                               + "mIntermediateAxis."
                               + "mPanTauCellBased."
                               + "mPanTauCellBasedProto."
                               + "pantau_CellBasedInput_BDTValue_1p0n_vs_1p1n."
                               + "pantau_CellBasedInput_BDTValue_1p1n_vs_1pXn."
                               + "pantau_CellBasedInput_BDTValue_3p0n_vs_3pXn."
                               + "pantau_CellBasedInput_BDTVar_Basic_NNeutralConsts."
                               + "pantau_CellBasedInput_BDTVar_Charged_HLV_SumM."
                               + "pantau_CellBasedInput_BDTVar_Charged_JetMoment_EtDRxTotalEt."
                               + "pantau_CellBasedInput_BDTVar_Charged_StdDev_Et_WrtEtAllConsts."
                               + "pantau_CellBasedInput_BDTVar_Combined_DeltaR1stNeutralTo1stCharged."
                               + "pantau_CellBasedInput_BDTVar_Neutral_HLV_SumM."
                               + "pantau_CellBasedInput_BDTVar_Neutral_PID_BDTValues_BDTSort_1."
                               + "pantau_CellBasedInput_BDTVar_Neutral_PID_BDTValues_BDTSort_2."
                               + "pantau_CellBasedInput_BDTVar_Neutral_Ratio_1stBDTEtOverEtAllConsts."
                               + "pantau_CellBasedInput_BDTVar_Neutral_Ratio_EtOverEtAllConsts."
                               + "pantau_CellBasedInput_BDTVar_Neutral_Shots_NPhotonsInSeed."
                               + "pantau_CellBasedInput_DecayMode."
                               + "pantau_CellBasedInput_DecayModeProto."
                               + "pantau_CellBasedInput_isPanTauCandidate."
                               + "phi."
                               + "phiDetectorAxis."
                               + "phiIntermediateAxis."
                               + "phiPanTauCellBased."
                               + "phiPanTauCellBasedProto."
                               + "pt."
                               + "ptDetectorAxis."
                               + "ptIntermediateAxis."
                               + "ptPanTauCellBased."
                               + "ptPanTauCellBasedProto."
                               + "ptRatioEflowApprox."
                               + "ptRatioEflowApproxCorrected."
                               + "SumPtTrkFrac."
                               + "SumPtTrkFracCorrected."
                               + "trFlightPathSig."
                               + "trFlightPathSig."
                               + "trFlightPathSigCorrected",
                                 "TauChargedParticleFlowObjects",
                                 "TauNeutralParticleFlowObjects",
                                 "TauHadronicParticleFlowObjects",
                                 "TauShotParticleFlowObjects",
                                 "TauPi0Clusters"]

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
                                       + "Jvt.JvtJvfcorr."
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
                                           + "JetVertexCharge_discriminant"]
                                            

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

#===========================================
# COLLECTIONS WITH All VARIABLES
#===========================================
TOPQExtraContainersStandard = ["CombinedMuonTrackParticles",
                               "egammaClusters",
                               "ExtrapolatedMuonTrackParticles",
                               "ForwardElectrons",
                               "GSFTrackParticles",
                               "InDetForwardTrackParticles",
                               "MuonSpectrometerTrackParticles"]

TOPQExtraContainersTrigger  = ["LVL1EmTauRoIs",
                               "HLT_TrigRoiDescriptorCollection_forID",
                               "HLT_TrigRoiDescriptorCollection_forID1",
                               "HLT_TrigRoiDescriptorCollection_forID2",
                               "HLT_TrigRoiDescriptorCollection_forID3",
                               "HLT_TrigRoiDescriptorCollection_forMS",
                               "HLT_TrigRoiDescriptorCollection_initialRoI",
                               "HLT_TrigRoiDescriptorCollection_secondaryRoI_EF",
                               "HLT_TrigRoiDescriptorCollection_secondaryRoI_HLT",
                               "HLT_TrigRoiDescriptorCollection_secondaryRoI_L2",
                               "HLT_xAOD__EmTauRoIContainer_L1TopoEM",
                               "HLT_xAOD__EmTauRoIContainer_L1TopoTau",
                               "HLT_xAOD__JetContainer_TrigTauJet",
                               "HLT_xAOD__TauJetContainer_TrigTauRecPreselection",
                               "HLT_xAOD__TrackParticleContainer_InDetTrigTrackingxAODCnv_Tau_EFID",
                               "HLT_xAOD__TrackParticleContainer_InDetTrigTrackingxAODCnv_Tau_FTF",
                               "HLT_xAOD__TrackParticleContainer_InDetTrigTrackingxAODCnv_Tau_IDTrig",
                               "HLT_xAOD__TrackParticleContainer_InDetTrigTrackingxAODCnv_TauCore_FTF",
                               "HLT_xAOD__TrackParticleContainer_InDetTrigTrackingxAODCnv_TauIso_FTF"]

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
