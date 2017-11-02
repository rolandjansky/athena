# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# Content included in addition to the smart slimming content

# ==========================================================================================================================
# Extra content
# ==========================================================================================================================

ExtraContentPhotons              = ["Photons.Loose.Medium.Tight.author.OQ"]
ExtraContentPhotonsTruth         = ["Photons.truthOrigin.truthParticleLink.truthType"]

ExtraContentElectrons            = ["Electrons.etcone20.etcone30.etcone40.ptcone20.ptcone30.ptcone40.Loose.Medium.Tight.DFCommonElectronsLHLoose.DFCommonElectronsLHMedium.DFCommonElectronsLHTight.DFCommonElectronsML.author.OQ"]
ExtraContentElectronsTruth       = ["Electrons.truthOrigin.truthType.truthParticleLink"]

ExtraContentMuons                = ["Muons.DFCommonGoodMuon.ptcone20.ptcone30.ptcone40.etcone20.etcone30.etcone40"]
ExtraContentMuonsTruth           = ["MuonTruthParticles.truthOrigin.truthType"]

ExtraContentTaus                 = [   "TauJets.pt.eta.phi.m.charge.jetLink.isTauFlags.BDTEleScore.BDTJetScore."
                                       + "caloIso."
                                       + "isolFrac."
                                       + "IsTruthMatched."
                                       + "truthJetLink."
                                       + "truthParticleLink."
                                       + "ptDetectorAxis."
                                       + "etaDetectorAxis."
                                       + "phiDetectorAxis."
                                       + "mDetectorAxis."
                                       + "ptIntermediateAxis."
                                       + "etaIntermediateAxis."
                                       + "phiIntermediateAxis."
                                       + "mIntermediateAxis."
                                       + "leadTrkPt."
                                       + "massTrkSys."
                                       + "trFlightPathSig."
                                       + "centFrac."
                                       #+ "centFracCorrected."
                                       + "ChPiEMEOverCaloEME."
                                       #+ "ChPiEMEOverCaloEMECorrected."
                                       + "dRmax."
                                       #+ "dRmaxCorrected."
                                       + "etOverPtLeadTrk."
                                       #+ "etOverPtLeadTrkCorrected."
                                       + "EMPOverTrkSysP."
                                       #+ "EMPOverTrkSysPCorrected."
                                       + "innerTrkAvgDist."
                                       #+ "innerTrkAvgDistCorrected."
                                       + "ipSigLeadTrk.absipSigLeadTrk."
                                       #+ "ipSigLeadTrkCorrected."
                                       + "massTrkSys."
                                       #+ "massTrkSysCorrected."
                                       + "mEflowApprox."
                                       #+ "mEflowApproxCorrected."
                                       + "ptRatioEflowApprox."
                                       #+ "ptRatioEflowApproxCorrected."
                                       + "SumPtTrkFrac."
                                       #+ "SumPtTrkFracCorrected."
                                       + "trFlightPathSig."
                                       #+ "trFlightPathSigCorrected."
                                       + "PSSFraction."
                                       + "etEMAtEMScale."
                                       + "etHadAtEMScale."
                                       + "sumEMCellEtOverLeadTrkPt."
                                       + "hadLeakEt."
                                       + "secMaxStripEt."
                                       + "ABS_ETA_LEAD_TRACK.TAU_ABSDELTAETA.TAU_ABSDELTAPHI."
                                       + "EMFracFixed."
                                       + "etHotShotWinOverPtLeadTrk."
                                       + "hadLeakFracFixed."
                                       + "leadTrackProbHT."
                                       + "ClustersMeanCenterLambda."
                                       + "ClustersMeanFirstEngDens."
                                       + "ClustersMeanSecondLambda."
                                       + "ClustersMeanPresamplerFrac."
                                       + "ClustersMeanEMProbability."
                                       + "PFOEngRelDiff."
                                       + "GhostMuonSegmentCount."
                                       + "ptPanTauCellBased."
                                       + "ptPanTauCellBasedProto."
                                       + "etaPanTauCellBased."
                                       + "etaPanTauCellBasedProto."
                                       + "phiPanTauCellBased."
                                       + "phiPanTauCellBasedProto."
                                       + "mPanTauCellBased."
                                       + "mPanTauCellBasedProto."
                                       + "PanTau_isPanTauCandidate.PanTau_DecayModeProto.PanTau_BDTValue_1p0n_vs_1p1n.PanTau_BDTValue_1p1n_vs_1pXn.PanTau_BDTValue_3p0n_vs_3pXn.PanTau_BDTVar_Basic_NNeutralConsts.PanTau_BDTVar_Charged_JetMoment_EtDRxTotalEt.PanTau_BDTVar_Charged_StdDev_Et_WrtEtAllConsts.PanTau_BDTVar_Neutral_HLV_SumM.PanTau_BDTVar_Neutral_PID_BDTValues_BDTSort_1.PanTau_BDTVar_Neutral_PID_BDTValues_BDTSort_2.PanTau_BDTVar_Neutral_Ratio_1stBDTEtOverEtAllConsts.PanTau_BDTVar_Neutral_Ratio_EtOverEtAllConsts.PanTau_BDTVar_Neutral_Shots_NPhotonsInSeed.PanTau_BDTVar_Combined_DeltaR1stNeutralTo1stCharged.PanTau_DecayModeExtended",
                                       "TauChargedParticleFlowObjects",
                                       "TauNeutralParticleFlowObjects",
                                       "TauHadronicParticleFlowObjects",
                                       "TauShotParticleFlowObjects",
                                       "TauPi0Clusters"   ]
ExtraContentTausTruth               = [ ]
ExtraContentJetsTruth               = ["AntiKt4TruthJets.eta.m.phi.pt.TruthLabelDeltaR_B.TruthLabelDeltaR_C.TruthLabelDeltaR_T.TruthLabelID.ConeTruthLabelID.PartonTruthLabelID.HadronConeExclTruthLabelID"]
# ==========================================================================================================================

ExtraContentTAUP1              =   ExtraContentPhotons                  \
                                 + ExtraContentElectrons                \
                                 + ExtraContentMuons                    \
                                 + ExtraContentTaus

# ExtraContentTAUP2              =   ExtraContentTaus

ExtraContentTAUP3              =   ExtraContentPhotons                  \
                                 + ExtraContentElectrons                \
                                 + ExtraContentMuons                    \
                                 + ExtraContentTaus

# ExtraContentTAUP4              =   ExtraContentPhotons                  \
#                                  + ExtraContentElectrons                \
#                                  + ExtraContentMuons
#                                  + ExtraContentTaus

ExtraContentTruthTAUP1         =   ExtraContentPhotonsTruth             \
                                 + ExtraContentElectronsTruth           \
                                 + ExtraContentMuonsTruth               \
                                 + ExtraContentTausTruth                \
                                 + ExtraContentJetsTruth

ExtraContentTruthTAUP2         =   ExtraContentElectronsTruth           \
                                 + ExtraContentMuonsTruth               \
                                 + ExtraContentTausTruth                \
                                 + ExtraContentJetsTruth

ExtraContentTruthTAUP3         =   ExtraContentPhotonsTruth             \
                                 + ExtraContentElectronsTruth           \
                                 + ExtraContentMuonsTruth               \
                                 + ExtraContentTausTruth                \
                                 + ExtraContentJetsTruth

# ExtraContentTruthTAUP4       =   ExtraContentPhotonsTruth             \
#                                + ExtraContentElectronsTruth           \
#                                + ExtraContentMuonsTruth               \
#                                + ExtraContentTausTruth

# ==========================================================================================================================
# Extra containers
# ==========================================================================================================================

ExtraContainersElectrons         = []  #    ["ForwardElectrons"] -- removed for rel21, not needed

ExtraContainersJets              = ["CaloCalTopoClusters"]

ExtraContainersTrigger           = ["LVL1EmTauRoIs",
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

# ==========================================================================================================================

ExtraContainersTAUP1            =   ExtraContainersElectrons    \
                                  + ExtraContainersJets         \
                                  + ExtraContainersTrigger 

ExtraContainersTAUP2            =   ExtraContainersTrigger      \
                                  + ["HLT_xAOD__TauJetContainer_TrigTauRecMerged",
#                                     "TauJets", # do not add this, it is in TauJetsCPContent and duplication causes merging to crash
                                     "TauNeutralParticleFlowObjects",
                                     "TauHadronicParticleFlowObjects",
                                     "TauShotParticleFlowObjects",
                                     "TauChargedParticleFlowObjects",
                                     "PrimaryVertices",
                                     "AntiKt4LCTopoJets",
                                     "CaloCalTopoClusters",
                                     "InDetTrackParticles"]

ExtraContainersTAUP3            =   ExtraContainersElectrons   \
                                  + ExtraContainersJets        \
                                  + ExtraContainersTrigger    \
                                  + ["HLT_xAOD__TauJetContainer_TrigTauRecMerged",
                                     "HLT_xAOD__MuonRoIContainer_L1TopoMuon",
                                     "HLT_xAOD__JetRoIContainer_L1TopoJet",
                                     "HLT_xAOD__TrigCompositeContainer_L1TopoMET",
                                     "HLT_xAOD__TrigCompositeContainer_L1TopoComposite",
                                     "HLT_xAOD__TrackParticleContainer_InDetTrigTrackingxAODCnv_Tau_FTK",
                                     "HLT_xAOD__TrackParticleContainer_InDetTrigTrackingxAODCnv_Tau_FTK_IDTrig",
                                     "HLT_xAOD__TrackParticleContainer_InDetTrigTrackingxAODCnv_Tau_FTKRefit",
                                     "HLT_xAOD__TrackParticleContainer_InDetTrigTrackingxAODCnv_Tau_FTKRefit_IDTrig",
                                     "HLT_xAOD__VertexContainer_PrimVertexFTK",
                                     "HLT_xAOD__VertexContainer_PrimVertexFTKRaw",
                                     "HLT_xAOD__VertexContainer_PrimVertexFTKRefit"]

ExtraContainersTAUP4            =   ExtraContainersTrigger      \
                                  + ["TauJets",
                                     "TauNeutralParticleFlowObjects",
                                     "TauHadronicParticleFlowObjects",
                                     "TauShotParticleFlowObjects",
                                     "TauChargedParticleFlowObjects",
                                     "PrimaryVertices",
                                     "AntiKt4LCTopoJets",
                                     "CaloCalTopoClusters",
                                     "InDetTrackParticles",
                                     "HLT_xAOD__TauJetContainer_TrigTauRecMerged",
                                     "HLT_xAOD__JetRoIContainer_L1TopoJet",
                                     "HLT_xAOD__TrigCompositeContainer_L1TopoComposite",
                                     "HLT_xAOD__TrackParticleContainer_InDetTrigTrackingxAODCnv_Tau_FTK",
                                     "HLT_xAOD__TrackParticleContainer_InDetTrigTrackingxAODCnv_Tau_FTK_IDTrig",
                                     "HLT_xAOD__TrackParticleContainer_InDetTrigTrackingxAODCnv_Tau_FTKRefit",
                                     "HLT_xAOD__TrackParticleContainer_InDetTrigTrackingxAODCnv_Tau_FTKRefit_IDTrig"]

ExtraContainersTruthTAUP1      = ["TruthEvents",
                                  "TruthParticles",
                                  "TruthVertices"
                                 #"AntiKt4TruthJets",
                                 #"BTagging_AntiKt4Truth", JRC TEMPORARILY COMMENTED
                                 #"AntiKt4TruthWZJets"
                                 ]

ExtraContainersTruthTAUP2      = ["TruthEvents",
                                  "TruthParticles",
                                  "TruthVertices"]
#                                 "AntiKt4TruthJets",
                                 #"BTagging_AntiKt4Truth", JRC TEMPORARILY COMMENTED
#                                 "AntiKt4TruthWZJets"]

ExtraContainersTruthTAUP3      = ["TruthEvents",
                                  "TruthParticles",
                                  "TruthVertices"
                                 #"AntiKt4TruthJets",
                                 #"BTagging_AntiKt4Truth", JRC TEMPORARILY COMMENTED
                                 #"AntiKt4TruthWZJets"
                                 ]

# ExtraContainersTruthTAUP4    = ["TruthEvents",
#                                 "TruthParticles",
#                                 "TruthVertices",
#                                 "AntiKt4TruthJets",
#                                #"BTagging_AntiKt4Truth", JRC TEMPORARILY COMMENTED
#                                 "AntiKt4TruthWZJets"]
