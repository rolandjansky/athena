# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

#====================================================================
# Common file used by TOPQCommonSlimming.py
# Access variables with:
#   from DerivationFrameworkTop.TOPQCommonExtraContent import *
# Available variables (arrays):
#   TOPQSmartSlimmingCollections
#   TOPQExtraVariables_Photons
#   TOPQExtraVariables_Electrons
#   TOPQExtraVariables_ForwardElectrons
#   TOPQExtraVariables_Muons
#   TOPQExtraVariables_Taus
#   TOPQExtraVariables_TauJets
#   TOPQExtraVariables_CaloCalTopoClusters
#   TOPQExtraVariables_egammaClusters
#   TOPQExtraVariables_AntiKt4EMTopoJets
#   TOPQExtraVariables_AntiKt4EMPFlowJets
#   TOPQExtraVariables_AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets
#   TOPQExtraVariables_AntiKt2PV0TrackJets 
#   TOPQExtraVariables_BTagging_AntiKt4EMPFlow
#   TOPQExtraVariables_BTagging_AntiKt4EMTopo
#   TOPQExtraVariables_CombinedMuonTrackParticles
#   TOPQExtraVariables_ExtrapolatedMuonTrackParticles
#   TOPQExtraVariables_InDetForwardTrackParticles
#   TOPQExtraVariables_MET_Track
#
#   TOPQExtraVariables_BTag_HLT
#   TOPQExtraVariables_HLT_EFJet
#   TOPQExtraVariables_HLT_GSCJet
#   TOPQExtraVariables_HLT_SplitJet
#   TOPQExtraVariables_HLT_TrigEFMissingET
#   TOPQExtraVariables_HLT_TrigEFMissingET_mht
#   TOPQExtraVariables_HLT_TrigEFMissingET_topocl_PUC
#   TOPQExtraVariables_HLT_a10r_tcemsubjesFS
#   TOPQExtraVariables_HLT_a10r_tcemsubjesISFS
#   TOPQExtraVariables_HLT_a10tclcwsubjesFS
#   TOPQExtraVariables_HLT_a10ttclcwjesFS
#   TOPQExtraVariables_HLT_a4tcemsubjesFS
#   TOPQExtraVariables_HLT_a4tcemsubjesISFS
#   TOPQExtraVariables_LVL1EnergySumRoI
#
#   TOPQExtraVariables_Photons_Truth
#   TOPQExtraVariables_Electrons_Truth
#   TOPQExtraVariables_ForwardElectrons_Truth
#   TOPQExtraVariables_Muons_Truth
#   TOPQExtraVariables_CombinedMuonTrackParticles_Truth
#   TOPQExtraVariables_MuonSpectrometerTrackParticles_Truth
#   TOPQExtraVariables_ExtrapolatedMuonTrackParticles_Truth
#   TOPQExtraVariables_InDetForwardTrackParticles_Truth
#   TOPQExtraVariables_Taus_Truth
#   TOPQExtraVariables_AntiKt4EMTopoJets_Truth
#   TOPQExtraVariables_AntiKt4EMPFlowJets_Truth
#   TOPQExtraVariables_AntiKt2PV0TrackJets_Truth
#   TOPQExtraVariables_AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets_Truth
#
#   TOPQExtraVariables_TruthEvents
#   TOPQExtraVariables_TruthParticles
#   TOPQExtraVariables_TruthVertices
#   TOPQExtraVariables_MET_Truth
#   TOPQExtraVariables_MET_TruthRegions
#
#   TOPQStaticContent
#   TOPQStaticContentTruth
#====================================================================

#================================
# SMART SLIMMING COLLECTIONS
#================================
# list of existing containers with a smart list is available here : 
# https://gitlab.cern.ch/atlas/athena/blob/21.2/PhysicsAnalysis/DerivationFramework/DerivationFrameworkCore/python/FullListOfSmartContainers.py
TOPQSmartSlimmingCollections = ["AntiKt4EMPFlowJets",
                                "AntiKt4EMTopoJets",
                                "AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets",
                                "BTagging_AntiKt2Track",
                                "BTagging_AntiKt4EMTopo",
                                "BTagging_AntiKt4EMPFlow",
                                "Electrons",
                                "HLT_xAOD__TrigMissingETContainer_TrigEFMissingET",
                                "HLT_xAOD__JetContainer_a4tcemsubjesFS",
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
# default list for Photons : https://gitlab.cern.ch/atlas/athena/blob/21.2/PhysicsAnalysis/DerivationFramework/DerivationFrameworkEGamma/python/PhotonsCPContent.py
TOPQExtraVariables_Photons    = ["Photons."
                                 ]

# default list for Electrons : https://gitlab.cern.ch/atlas/athena/blob/21.2/PhysicsAnalysis/DerivationFramework/DerivationFrameworkEGamma/python/ElectronsCPContent.py
TOPQExtraVariables_Electrons  = ["Electrons."
                                 + "etcone30."
                                 + "etcone40"]

TOPQExtraVariables_ForwardElectrons  = ["ForwardElectrons."
                                        + "DeltaE."
                                        + "Eratio."
                                        + "Loose."
                                        + "Medium."
                                        + "OQ."
                                        + "Reta."
                                        + "Rhad."
                                        + "Rhad1."
                                        + "Rphi."
                                        + "Tight."
                                        + "author."
                                        + "caloClusterLinks."
                                        + "charge."
                                        + "deltaEta0."
                                        + "deltaEta1."
                                        + "deltaEta2."
                                        + "deltaEta3."
                                        + "deltaPhi0."
                                        + "deltaPhi1."
                                        + "deltaPhi2."
                                        + "deltaPhi3."
                                        + "deltaPhiFromLastMeasurement."
                                        + "deltaPhiRescaled0."
                                        + "deltaPhiRescaled1."
                                        + "deltaPhiRescaled2."
                                        + "deltaPhiRescaled3."
                                        + "e277."
                                        + "eta."
                                        + "f1."
                                        + "f1core."
                                        + "f3."
                                        + "f3core."
                                        + "fracs1."
                                        + "m."
                                        + "phi."
                                        + "pt."
                                        + "topoetcone20."
                                        + "topoetcone30."
                                        + "topoetcone40."
                                        + "topoetconeCorrBitset."
                                        + "topoetconecoreConeEnergyCorrection."
                                        + "trackParticleLinks."
                                        + "weta1."
                                        + "weta2."
                                        + "wtots1"]

# default list for Muons : https://gitlab.cern.ch/atlas/athena/blob/21.2/PhysicsAnalysis/DerivationFramework/DerivationFrameworkMuons/python/MuonsCPContent.py
TOPQExtraVariables_Muons      = ["Muons."
                                 + "DFCommonMuonsLoose."
                                 + "DFCommonMuonsMedium."
                                 + "DFCommonMuonsTight."
                                 + "etcone20."
                                 + "etcone30."
                                 + "etcone40."
                                 + "EnergyLoss."
                                 + "PromptLeptonInput_DL1mu."
                                 + "PromptLeptonInput_DRlj."
                                 + "PromptLeptonInput_LepJetPtFrac."
                                 + "PromptLeptonInput_PtFrac."
                                 + "PromptLeptonInput_PtRel."
                                 + "PromptLeptonInput_TrackJetNTrack."
                                 + "PromptLeptonInput_ip2."
                                 + "PromptLeptonInput_ip3."
                                 + "PromptLeptonInput_rnnip."
                                 + "PromptLeptonInput_sv1_jf_ntrkv."
                                 + "PromptLeptonIso."
                                 + "PromptLeptonVeto."
                                 + "energyLossType"]

TOPQExtraVariables_Taus   = []

TOPQExtraVariables_TauJets   = ["TauJets."
                                + "BDTEleScore."
                                + "BDTEleScoreSigTrans."
                                + "BDTJetScore."
                                + "BDTJetScoreSigTrans."
                                + "EleMatchLikelihoodScore."
                                + "PanTau_DecayMode."
                                + "PromptTauInput_DRlj."
                                + "PromptTauInput_JetF."
                                + "PromptTauInput_LepJetPtFrac."
                                + "PromptTauInput_MV2c10rnn."
                                + "PromptTauInput_SV1."
                                + "PromptTauInput_TrackJetNTrack."
                                + "PromptTauInput_ip2."
                                + "PromptTauInput_ip3."
                                + "PromptTauInput_rnnip."
                                + "PromptTauVeto."
                                + "charge."
                                + "chargedPFOLinks."
                                + "electronLink."
                                + "eta."
                                + "etaFinalCalib."
                                + "eta_combined."
                                + "hadronicPFOLinks."
                                + "isTauFlags."
                                + "jetLink."
                                + "m."
                                + "mFinalCalib."
                                + "m_combined."
                                + "neutralPFOLinks."
                                + "phi."
                                + "phiFinalCalib."
                                + "phi_combined."
                                + "pi0PFOLinks."
                                + "protoChargedPFOLinks."
                                + "protoNeutralPFOLinks."
                                + "protoPi0PFOLinks."
                                + "pt."
                                + "ptFinalCalib."
                                + "pt_combined."
                                + "secondaryVertexLink."
                                + "shotPFOLinks."
                                + "tauTrackLinks."
                                + "vertexLink"]

TOPQExtraVariables_TauTracks   = ["TauTracks."
                                  + "bdtScores."
                                  + "eta."
                                  + "flagSet."
                                  + "phi."
                                  + "pt."
                                  + "trackLinks"]


# additional variables for clusters
# default list for CaloCalTopoClusters : #https://gitlab.cern.ch/atlas/athena/blob/21.2/PhysicsAnalysis/DerivationFramework/DerivationFrameworkJetEtMiss/python/ExtendedJetCommon.py#L412
TOPQExtraVariables_CaloCalTopoClusters = ["CaloCalTopoClusters."
                                          + "CENTER_MAG"]

# default list for egammaclusters through Electrons and Photons : https://gitlab.cern.ch/atlas/athena/blob/21.2/PhysicsAnalysis/DerivationFramework/DerivationFrameworkEGamma/python/ElectronsCPContent.py, https://gitlab.cern.ch/atlas/athena/blob/21.2/PhysicsAnalysis/DerivationFramework/DerivationFrameworkEGamma/python/PhotonsCPContent.py
TOPQExtraVariables_egammaClusters = ["egammaClusters."
                                     + "BadChannelList."
                                     + "CellLink."
                                     + "ETA1CALOFRAME."
                                     + "PHI1CALOFRAME."
                                     + "PHICALOFRAME."
                                     + "altE."
                                     + "altEta."
                                     + "altM."
                                     + "altPhi."
                                     + "calM."
                                     + "clusterSize."
                                     + "emax_sampl."
                                     + "eta0."
                                     + "etamax_sampl."
                                     + "phi0."
                                     + "phi_sampl."
                                     + "phimax_sampl."
                                     + "rawE."
                                     + "rawEta."
                                     + "rawM."
                                     + "rawPhi."
                                     + "time"]


# additional variables for jets
# list of default variables : https://gitlab.cern.ch/atlas/athena/blob/21.2/PhysicsAnalysis/DerivationFramework/DerivationFrameworkJetEtMiss/python/AntiKt4EMTopoJetsCPContent.py
TOPQExtraVariables_AntiKt4EMTopoJets =  ["AntiKt4EMTopoJets."
                                         + "DFCommonJets_Calib_pt."
                                         + "DFCommonJets_Calib_eta."
                                         + "DFCommonJets_Calib_phi."
                                         + "DFCommonJets_Calib_m."
                                         + "ActiveArea."
                                         + "AlgorithmType."
                                         + "BchCorrCell."
                                         + "CentroidR."
                                         + "Charge."
                                         + "ConstituentScale."
                                         + "DFCommonJets_Calib_eta."
                                         + "DFCommonJets_Calib_m."
                                         + "DFCommonJets_Calib_phi."
                                         + "DFCommonJets_Calib_pt."
                                         + "DFCommonJets_FixedCutBEff_30."
                                         + "DFCommonJets_FixedCutBEff_50."
                                         + "DFCommonJets_FixedCutBEff_60."
                                         + "DFCommonJets_FixedCutBEff_70."
                                         + "DFCommonJets_FixedCutBEff_77."
                                         + "DFCommonJets_FixedCutBEff_80."
                                         + "DFCommonJets_FixedCutBEff_85."
                                         + "DFCommonJets_FixedCutBEff_90."
                                         + "DFCommonJets_FlatBEff_30."
                                         + "DFCommonJets_FlatBEff_50."
                                         + "DFCommonJets_FlatBEff_60."
                                         + "DFCommonJets_FlatBEff_70."
                                         + "DFCommonJets_FlatBEff_77."
                                         + "DFCommonJets_FlatBEff_85."
                                         + "DFCommonJets_Jvt."
                                         + "DFCommonJets_passJvt."
                                         + "DFCommonJets_passOR."
                                         + "DetectorPhi."
                                         + "ECPSFraction."
                                         + "GhostMuonSegment."
                                         + "GhostTrackCount."
                                         + "GhostTrackPt."
                                         + "HighestJVFVtx."
                                         + "InputType."
                                         + "JVF."
                                         + "JetEMScaleMomentum_eta."
                                         + "JetEMScaleMomentum_m."
                                         + "JetEMScaleMomentum_phi."
                                         + "JetEMScaleMomentum_pt."
                                         + "JetEtaJESScaleMomentum_eta."
                                         + "JetEtaJESScaleMomentum_m."
                                         + "JetEtaJESScaleMomentum_phi."
                                         + "JetEtaJESScaleMomentum_pt."
                                         + "JetGhostArea."
                                         + "JetLCScaleMomentum_eta."
                                         + "JetLCScaleMomentum_m."
                                         + "JetLCScaleMomentum_phi."
                                         + "JetLCScaleMomentum_pt."
                                         + "JetPileupScaleMomentum_eta."
                                         + "JetPileupScaleMomentum_m."
                                         + "JetPileupScaleMomentum_phi."
                                         + "JetPileupScaleMomentum_pt."
                                         + "LArBadHVEnergyFrac."
                                         + "LArBadHVNCell."
                                         + "LeadingClusterCenterLambda."
                                         + "LeadingClusterPt."
                                         + "LeadingClusterSecondLambda."
                                         + "LeadingClusterSecondR."
                                         + "N90Constituents."
                                         + "OotFracClusters10."
                                         + "OotFracClusters5."
                                         + "OriginCorrected."
                                         + "OriginVertex."
                                         + "PileupCorrected."
                                         + "SizeParameter."
                                         + "SumPtTrkPt1000."
                                         + "TrackSumMass."
                                         + "TrackSumPt."
                                         + "TrackWidthPt500."
                                         + "Width."
                                         + "WidthPhi."
                                         + "btaggingLink."
                                         + "isBJet."
                                         + "selected"]

# list of default variables : https://gitlab.cern.ch/atlas/athena/blob/21.2/PhysicsAnalysis/DerivationFramework/DerivationFrameworkJetEtMiss/python/AntiKt4EMPFlowJetsCPContent.py
TOPQExtraVariables_AntiKt4EMPFlowJets = ["AntiKt4EMPFlowJets."
                                         + "AntiKt2TrackJet."
                                         + "AverageLArQF."
                                         + "BchCorrCell."
                                         + "btagging."
                                         + "btaggingLink."
                                         + "constituentLinks."
                                         + "GhostAntiKt2TrackJet."
                                         + "GhostAntiKt3TrackJet."
                                         + "GhostAntiKt4TrackJet."
                                         + "GhostMuonSegment."
                                         + "GhostTrackCount."
                                         + "GhostTrackPt."
                                         + "HighestJVFLooseVtx."
                                         + "HighestJVFVtx."
                                         + "HighestJVFVtxGhost."
                                         + "JetLCScaleMomentum_eta."
                                         + "JetLCScaleMomentum_m."
                                         + "JetLCScaleMomentum_phi."
                                         + "JetLCScaleMomentum_pt."
                                         + "NegativeE."
                                         + "originalObjectLink."
                                         + "OriginVertex."
                                         + "ActiveArea."
                                         + "AlgorithmType."
                                         + "BchCorrCell."
                                         + "CentroidR."
                                         + "Charge."
                                         + "ChargedPFOWidthPt1000."
                                         + "ChargedPFOWidthPt500."
                                         + "ConstituentScale."
                                         + "DetectorPhi."
                                         + "GhostTrackCount."
                                         + "GhostTrackPt."
                                         + "HighestJVFVtx."
                                         + "InputType."
                                         + "JVF."
                                         + "JetEMScaleMomentum_eta."
                                         + "JetEMScaleMomentum_m."
                                         + "JetEMScaleMomentum_phi."
                                         + "JetEMScaleMomentum_pt."
                                         + "JetEtaJESScaleMomentum_eta."
                                         + "JetEtaJESScaleMomentum_m."
                                         + "JetEtaJESScaleMomentum_phi."
                                         + "JetEtaJESScaleMomentum_pt."
                                         + "JetGhostArea."
                                         + "JetPileupScaleMomentum_eta."
                                         + "JetPileupScaleMomentum_m."
                                         + "JetPileupScaleMomentum_phi."
                                         + "JetPileupScaleMomentum_pt."
                                         + "Jvt."
                                         + "JvtRpt."
                                         + "LArBadHVEnergyFrac."
                                         + "LArBadHVNCell."
                                         + "LArQuality."
                                         + "N90Constituents."
                                         + "NegativeE."
                                         + "OotFracClusters10."
                                         + "OotFracClusters5."
                                         + "OriginCorrected."
                                         + "OriginVertex."
                                         + "PileupCorrected."
                                         + "SizeParameter."
                                         + "SumPtChargedPFOPt1000."
                                         + "SumPtChargedPFOPt500."
                                         + "SumPtTrkPt1000."
                                         + "SumPtTrkPt500."
                                         + "TrackSumMass."
                                         + "TrackSumPt."
                                         + "TrackWidthPt1000."
                                         + "TrackWidthPt500."
                                         + "Width."
                                         + "WidthPhi."
                                         + "btaggingLink"]


# heavy flavour tagging
# default variables are in : https://gitlab.cern.ch/atlas/athena/blob/21.2/PhysicsAnalysis/DerivationFramework/DerivationFrameworkFlavourTag/python/BTaggingContent.py
TOPQExtraVariables_BTagging_AntiKt4EMPFlow = ["BTagging_AntiKt4EMPFlow."
                                              + "BTagTrackToJetAssociator."
                                              + "BTagTrackToJetAssociatorBB."
                                              + "IP3D_pb."
                                              + "IP3D_pu."
                                              + "JetFitter_JFvertices."
                                              + "JetFitter_tracksAtPVlinks."
                                              + "MSV_badTracksIP."
                                              + "MSV_vertices."
                                              + "SV1_badTracksIP."
                                              + "SV1_pb."
                                              + "SV1_pu."
                                              + "SV1_vertices"]

TOPQExtraVariables_BTagging_AntiKt4EMTopo = ["BTagging_AntiKt4EMTopo."
                                             + "JetVertexCharge_discriminant."
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
                                             + "MSV_vtxntrk."
                                             + "MultiSVbb1_discriminant."
                                             + "MultiSVbb2_discriminant."
                                             + "SV1_TrackParticleLinks"]

# https://gitlab.cern.ch/atlas/athena/blob/21.2/PhysicsAnalysis/DerivationFramework/DerivationFrameworkJetEtMiss/python/AntiKt10LCTopoTrimmedPtFrac5SmallR20JetsCPContent.py
TOPQExtraVariables_AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets = ["AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets."
                                                               + "AntiKt10LCTopoTrimmedPtFrac5SmallR20JetsAux."
                                                               + "ActiveArea."
                                                               + "ActiveArea4vec_eta."
                                                               + "ActiveArea4vec_m."
                                                               + "ActiveArea4vec_phi."
                                                               + "ActiveArea4vec_pt."
                                                               + "AlgorithmType."
                                                               + "Angularity."
                                                               + "Aplanarity."
                                                               + "Charge."
                                                               + "ConstituentScale."
                                                               + "DFCommonJets_Calib_eta."
                                                               + "DFCommonJets_Calib_m."
                                                               + "DFCommonJets_Calib_phi."
                                                               + "DFCommonJets_Calib_pt."
                                                               + "DetectorPhi."
                                                               + "Dip12."
                                                               + "Dip13."
                                                               + "Dip23."
                                                               + "DipExcl12."
                                                               + "FoxWolfram0."
                                                               + "FoxWolfram1."
                                                               + "FoxWolfram2."
                                                               + "FoxWolfram3."
                                                               + "FoxWolfram4."
                                                               + "GhostAntiKt2TrackJetCount."
                                                               + "GhostAntiKt2TrackJetPt."
                                                               + "GhostAntiKt4TrackJet."
                                                               + "GhostAntiKt4TrackJetCount."
                                                               + "GhostAntiKt4TrackJetPt."
                                                               + "GhostMuonSegment."
                                                               + "GhostMuonSegmentCount."
                                                               + "GhostTrack."
                                                               + "GhostTrackCount."
                                                               + "GhostTrackPt."
                                                               + "InputType."
                                                               + "JetEMScaleMomentum_eta."
                                                               + "JetEMScaleMomentum_m."
                                                               + "JetEMScaleMomentum_phi."
                                                               + "JetEMScaleMomentum_pt."
                                                               + "JetGhostArea."
                                                               + "KtDR."
                                                               + "Mu12."
                                                               + "Parent."
                                                               + "PlanarFlow."
                                                               + "PullMag."
                                                               + "PullPhi."
                                                               + "Pull_C00."
                                                               + "Pull_C01."
                                                               + "Pull_C10."
                                                               + "Pull_C11."
                                                               + "SizeParam."
                                                               + "Sphericity."
                                                               + "Split12."
                                                               + "Split23."
                                                               + "Split34."
                                                               + "Tau1."
                                                               + "Tau2."
                                                               + "Tau3."
                                                               + "ThrustMaj."
                                                               + "ThrustMin."
                                                               + "TransformType."
                                                               + "Width."
                                                               + "WidthPhi."
                                                               + "ZCut12."
                                                               + "ZCut23."
                                                               + "ZCut34"]


#TOPQExtraVariables_AntiKt2PV0TrackJets = ["AntiKt2PV0TrackJets."
#                                          + "pt."
#                                          + "eta."
#                                          + "phi."
#                                          + "m.constituentLinks."
#                                          + "btaggingLink."
#                                          + "OriginVertex."
#                                          + "GhostAntiKt3TrackJet."
#                                          + "GhostAntiKt4TrackJet."
#                                          + "GhostTrack."
#+ additional variables (which seem to be filled by default)
#                                          + "ActiveArea."
#                                          + "ActiveArea4vec_eta."
#                                          + "ActiveArea4vec_m."
#                                          + "ActiveArea4vec_phi."
#                                          + "ActiveArea4vec_pt."
#                                          + "AlgorithmType."
#                                          + "ConstituentScale."
#                                          + "InputType."
#                                          + "JetConstitScaleMomentum_eta."
#                                          + "JetConstitScaleMomentum_m."
#                                          + "JetConstitScaleMomentum_phi."
#                                          + "JetConstitScaleMomentum_pt."
#                                          + "JetGhostArea."
#                                          + "OriginVertex."
#                                          + "SizeParameter."
#                                          + "Width."
#                                          + "WidthPhi."
#                                          + "btaggingLink"]

# default list for CombinedMuonTrackParticles through Muons : https://gitlab.cern.ch/atlas/athena/blob/21.2/PhysicsAnalysis/DerivationFramework/DerivationFrameworkMuons/python/MuonsCPContent.py
TOPQExtraVariables_CombinedMuonTrackParticles = ["CombinedMuonTrackParticles."
                                                 + "pt."
                                                 + "eta."
                                                 + "truthParticleLink."
                                                 + "msInnerMatchChi2."
                                                 + "TRTTrackOccupancy."
                                                 + "TRTdEdx."
                                                 + "TRTdEdxUsedHits."
                                                 + "alignEffectSigmaDeltaTrans."
                                                 + "beamlineTiltX."
                                                 + "beamlineTiltY."
                                                 + "deltaphi_0."
                                                 + "deltaphi_1."
                                                 + "deltatheta_0."
                                                 + "deltatheta_1."
                                                 + "eProbabilityComb."
                                                 + "eProbabilityHT."
                                                 + "expectInnermostPixelLayerHit."
                                                 + "expectNextToInnermostPixelLayerHit."
                                                 + "hitPattern."
                                                 + "identifierOfFirstHit."
                                                 + "numberOfContribPixelLayers."
                                                 + "numberOfDBMHits."
                                                 + "numberOfGangedFlaggedFakes."
                                                 + "numberOfGangedPixels."
                                                 + "numberOfIBLOverflowsdEdx."
                                                 + "numberOfInnermostPixelLayerOutliers."
                                                 + "numberOfInnermostPixelLayerSharedHits."
                                                 + "numberOfInnermostPixelLayerSplitHits."
                                                 + "numberOfNextToInnermostPixelLayerHits."
                                                 + "numberOfNextToInnermostPixelLayerOutliers."
                                                 + "numberOfNextToInnermostPixelLayerSharedHits."
                                                 + "numberOfNextToInnermostPixelLayerSplitHits."
                                                 + "numberOfOutliersOnTrack."
                                                 + "numberOfPixelOutliers."
                                                 + "numberOfPixelSharedHits."
                                                 + "numberOfPixelSplitHits."
                                                 + "numberOfPixelSpoiltHits."
                                                 + "numberOfSCTDoubleHoles."
                                                 + "numberOfSCTOutliers."
                                                 + "numberOfSCTSharedHits."
                                                 + "numberOfSCTSpoiltHits."
                                                 + "numberOfTRTDeadStraws."
                                                 + "numberOfTRTHighThresholdHits."
                                                 + "numberOfTRTHighThresholdHitsTotal."
                                                 + "numberOfTRTHighThresholdOutliers."
                                                 + "numberOfTRTHoles."
                                                 + "numberOfTRTSharedHits."
                                                 + "numberOfTRTTubeHits."
                                                 + "numberOfTRTXenonHits."
                                                 + "numberOfTriggerEtaHoleLayers."
                                                 + "numberOfTriggerEtaLayers."
                                                 + "numberOfUsedHitsdEdx."
                                                 + "parameterPX."
                                                 + "parameterPY."
                                                 + "parameterPZ."
                                                 + "parameterPosition."
                                                 + "parameterX."
                                                 + "parameterY."
                                                 + "parameterZ."
                                                 + "particleHypothesis."
                                                 + "patternRecoInfo."
                                                 + "pixeldEdx."
                                                 + "radiusOfFirstHit."
                                                 + "sigmadeltaphi_0."
                                                 + "sigmadeltaphi_1."
                                                 + "sigmadeltatheta_0."
                                                 + "sigmadeltatheta_1."
                                                 + "standardDeviationOfChi2OS."
                                                 + "trackFitter."
                                                 + "trackLink."
                                                 + "trackParameterCovarianceMatrices."
                                                 + "trackProperties."
                                                 + "vx."
                                                 + "vy"]

# default list for ExtrapolatedMuonTrackParticles through Muons : https://gitlab.cern.ch/atlas/athena/blob/21.2/PhysicsAnalysis/DerivationFramework/DerivationFrameworkMuons/python/MuonsCPContent.py
TOPQExtraVariables_ExtrapolatedMuonTrackParticles = ["ExtrapolatedMuonTrackParticles."
                                                     + "pt."
                                                     + "eta."
                                                     + "d0."
                                                     + "z0."
                                                     + "vz."
                                                     + "vertexLink."
                                                     + "truthParticleLink."
                                                     + "chiSquared."
                                                     + "numberDoF."
                                                     + "numberOfPhiLayers."
                                                     + "numberOfPhiHoleLayers."
                                                     + "TRTTrackOccupancy."
                                                     + "TRTdEdx."
                                                     + "TRTdEdxUsedHits."
                                                     + "beamlineTiltX."
                                                     + "beamlineTiltY."
                                                     + "caloExt_Decorated."
                                                     + "caloExt_eta."
                                                     + "caloExt_phi."
                                                     + "deltaphi_0."
                                                     + "deltaphi_1."
                                                     + "deltatheta_0."
                                                     + "deltatheta_1."
                                                     + "eProbabilityComb."
                                                     + "eProbabilityHT."
                                                     + "expectInnermostPixelLayerHit."
                                                     + "expectNextToInnermostPixelLayerHit."
                                                     + "hitPattern."
                                                     + "identifierOfFirstHit."
                                                     + "numberOfContribPixelLayers."
                                                     + "numberOfDBMHits."
                                                     + "numberOfGangedFlaggedFakes."
                                                     + "numberOfGangedPixels."
                                                     + "numberOfIBLOverflowsdEdx."
                                                     + "numberOfInnermostPixelLayerHits."
                                                     + "numberOfInnermostPixelLayerOutliers."
                                                     + "numberOfInnermostPixelLayerSharedHits."
                                                     + "numberOfInnermostPixelLayerSplitHits."
                                                     + "numberOfNextToInnermostPixelLayerHits."
                                                     + "numberOfNextToInnermostPixelLayerOutliers."
                                                     + "numberOfNextToInnermostPixelLayerSharedHits."
                                                     + "numberOfNextToInnermostPixelLayerSplitHits."
                                                     + "numberOfOutliersOnTrack."
                                                     + "numberOfPixelOutliers."
                                                     + "numberOfPixelSharedHits."
                                                     + "numberOfPixelSplitHits."
                                                     + "numberOfPixelSpoiltHits."
                                                     + "numberOfSCTDoubleHoles."
                                                     + "numberOfSCTOutliers."
                                                     + "numberOfSCTSharedHits."
                                                     + "numberOfSCTSpoiltHits."
                                                     + "numberOfTRTDeadStraws."
                                                     + "numberOfTRTHighThresholdHits."
                                                     + "numberOfTRTHighThresholdHitsTotal."
                                                     + "numberOfTRTHighThresholdOutliers."
                                                     + "numberOfTRTHoles."
                                                     + "numberOfTRTSharedHits."
                                                     + "numberOfTRTTubeHits."
                                                     + "numberOfTRTXenonHits."
                                                     + "numberOfTriggerEtaHoleLayers."
                                                     + "numberOfTriggerEtaLayers."
                                                     + "numberOfUsedHitsdEdx."
                                                     + "parameterPX."
                                                     + "parameterPY."
                                                     + "parameterPZ."
                                                     + "parameterPosition."
                                                     + "parameterX."
                                                     + "parameterY."
                                                     + "parameterZ."
                                                     + "particleHypothesis."
                                                     + "patternRecoInfo."
                                                     + "pixeldEdx."
                                                     + "radiusOfFirstHit."
                                                     + "sigmadeltaphi_0."
                                                     + "sigmadeltaphi_1."
                                                     + "sigmadeltatheta_0."
                                                     + "sigmadeltatheta_1."
                                                     + "standardDeviationOfChi2OS."
                                                     + "trackFitter."
                                                     + "trackLink."
                                                     + "trackParameterCovarianceMatrices."
                                                     + "trackProperties."
                                                     + "vx."
                                                     + "vy"]

# default list for MuonSpectrometerTrackParticles through Muons : https://gitlab.cern.ch/atlas/athena/blob/21.2/PhysicsAnalysis/DerivationFramework/DerivationFrameworkMuons/python/MuonsCPContent.py
TOPQExtraVariables_MuonSpectrometerTrackParticles = ["MuonSpectrometerTrackParticles."
                                                     + "pt."
                                                     + "eta."
                                                     + "numberOfPixelHits."
                                                     + "numberOfPixelDeadSensors."
                                                     + "numberOfPrecisionLayers."
                                                     + "truthParticleLink."
                                                     + "chiSquared."
                                                     + "numberDoF."
                                                     + "numberOfPhiLayers."
                                                     + "numberOfPhiHoleLayers."
                                                     + "numberOfPixelHoles."
                                                     + "numberOfPrecisionHoleLayers."
                                                     + "numberOfSCTDeadSensors."
                                                     + "numberOfSCTHits."
                                                     + "numberOfSCTHoles."
                                                     + "numberOfTRTHits."
                                                     + "numberOfTRTOutliers."
                                                     + "TRTTrackOccupancy."
                                                     + "TRTdEdx."
                                                     + "TRTdEdxUsedHits."
                                                     + "alignEffectSigmaDeltaTrans."
                                                     + "beamlineTiltX."
                                                     + "beamlineTiltY."
                                                     + "deltaphi_0."
                                                     + "deltaphi_1."
                                                     + "deltatheta_0."
                                                     + "deltatheta_1."
                                                     + "eProbabilityComb."
                                                     + "eProbabilityHT."
                                                     + "expectInnermostPixelLayerHit."
                                                     + "expectNextToInnermostPixelLayerHit."
                                                     + "hitPattern."
                                                     + "identifierOfFirstHit."
                                                     + "numberOfContribPixelLayers."
                                                     + "numberOfDBMHits."
                                                     + "numberOfGangedFlaggedFakes."
                                                     + "numberOfGangedPixels."
                                                     + "numberOfIBLOverflowsdEdx."
                                                     + "numberOfInnermostPixelLayerHits."
                                                     + "numberOfInnermostPixelLayerOutliers."
                                                     + "numberOfInnermostPixelLayerSharedHits."
                                                     + "numberOfInnermostPixelLayerSplitHits."
                                                     + "numberOfNextToInnermostPixelLayerHits."
                                                     + "numberOfNextToInnermostPixelLayerOutliers."
                                                     + "numberOfNextToInnermostPixelLayerSharedHits."
                                                     + "numberOfNextToInnermostPixelLayerSplitHits."
                                                     + "numberOfOutliersOnTrack."
                                                     + "numberOfPixelOutliers."
                                                     + "numberOfPixelSharedHits."
                                                     + "numberOfPixelSplitHits."
                                                     + "numberOfPixelSpoiltHits."
                                                     + "numberOfSCTDoubleHoles."
                                                     + "numberOfSCTOutliers."
                                                     + "numberOfSCTSharedHits."
                                                     + "numberOfSCTSpoiltHits."
                                                     + "numberOfTRTDeadStraws."
                                                     + "numberOfTRTHighThresholdHits."
                                                     + "numberOfTRTHighThresholdHitsTotal."
                                                     + "numberOfTRTHighThresholdOutliers."
                                                     + "numberOfTRTHoles."
                                                     + "numberOfTRTSharedHits."
                                                     + "numberOfTRTTubeHits."
                                                     + "numberOfTRTXenonHits."
                                                     + "numberOfTriggerEtaHoleLayers."
                                                     + "numberOfTriggerEtaLayers."
                                                     + "numberOfUsedHitsdEdx."
                                                     + "parameterPX."
                                                     + "parameterPY."
                                                     + "parameterPZ."
                                                     + "parameterPosition."
                                                     + "parameterX."
                                                     + "parameterY."
                                                     + "parameterZ."
                                                     + "particleHypothesis."
                                                     + "patternRecoInfo."
                                                     + "pixeldEdx."
                                                     + "radiusOfFirstHit."
                                                     + "rpcHitIdentifier."
                                                     + "rpcHitPositionX."
                                                     + "rpcHitPositionY."
                                                     + "rpcHitPositionZ."
                                                     + "rpcHitTime."
                                                     + "sigmadeltaphi_0."
                                                     + "sigmadeltaphi_1."
                                                     + "sigmadeltatheta_0."
                                                     + "sigmadeltatheta_1."
                                                     + "standardDeviationOfChi2OS."
                                                     + "trackFitter."
                                                     + "trackLink."
                                                     + "trackParameterCovarianceMatrices."
                                                     + "trackProperties."
                                                     + "vx."
                                                     + "vy"]

# default list for InDetForwardTrackParticles through Muons : https://gitlab.cern.ch/atlas/athena/blob/21.2/PhysicsAnalysis/DerivationFramework/DerivationFrameworkMuons/python/MuonsCPContent.py
TOPQExtraVariables_InDetForwardTrackParticles = ["InDetForwardTrackParticles."
                                                 + "pt."
                                                 + "eta."
                                                 + "vertexLink."
                                                 + "truthParticleLink."
                                                 + "chiSquared."
                                                 + "numberDoF."
                                                 + "numberOfPhiLayers."
                                                 + "numberOfPhiHoleLayers."
                                                 + "numberOfPrecisionHoleLayers."
                                                 + "numberOfSCTDeadSensors."
                                                 + "numberOfSCTHits."
                                                 + "TRTTrackOccupancy."
                                                 + "TRTdEdx."
                                                 + "TRTdEdxUsedHits."
                                                 + "alignEffectSigmaDeltaTrans."
                                                 + "beamlineTiltX."
                                                 + "beamlineTiltY."
                                                 + "caloExt_Decorated."
                                                 + "caloExt_eta."
                                                 + "caloExt_phi."
                                                 + "deltaphi_0."
                                                 + "deltaphi_1."
                                                 + "deltatheta_0."
                                                 + "deltatheta_1."
                                                 + "eProbabilityComb."
                                                 + "eProbabilityHT."
                                                 + "expectInnermostPixelLayerHit."
                                                 + "expectNextToInnermostPixelLayerHit."
                                                 + "hitPattern."
                                                 + "identifierOfFirstHit."
                                                 + "nBC_meas."
                                                 + "numberOfContribPixelLayers."
                                                 + "numberOfDBMHits."
                                                 + "numberOfGangedFlaggedFakes."
                                                 + "numberOfGangedPixels."
                                                 + "numberOfIBLOverflowsdEdx."
                                                 + "numberOfInnermostPixelLayerHits."
                                                 + "numberOfInnermostPixelLayerOutliers."
                                                 + "numberOfInnermostPixelLayerSharedHits."
                                                 + "numberOfInnermostPixelLayerSplitHits."
                                                 + "numberOfNextToInnermostPixelLayerHits."
                                                 + "numberOfNextToInnermostPixelLayerOutliers."
                                                 + "numberOfNextToInnermostPixelLayerSharedHits."
                                                 + "numberOfNextToInnermostPixelLayerSplitHits."
                                                 + "numberOfOutliersOnTrack."
                                                 + "numberOfPixelOutliers."
                                                 + "numberOfPixelSharedHits."
                                                 + "numberOfPixelSplitHits."
                                                 + "numberOfPixelSpoiltHits."
                                                 + "numberOfSCTDoubleHoles."
                                                 + "numberOfSCTOutliers."
                                                 + "numberOfSCTSharedHits."
                                                 + "numberOfSCTSpoiltHits."
                                                 + "numberOfTRTDeadStraws."
                                                 + "numberOfTRTHighThresholdHits."
                                                 + "numberOfTRTHighThresholdHitsTotal."
                                                 + "numberOfTRTHighThresholdOutliers."
                                                 + "numberOfTRTHoles."
                                                 + "numberOfTRTSharedHits."
                                                 + "numberOfTRTTubeHits."
                                                 + "numberOfTRTXenonHits."
                                                 + "numberOfTriggerEtaHoleLayers."
                                                 + "numberOfTriggerEtaLayers."
                                                 + "numberOfUsedHitsdEdx."
                                                 + "parameterPX."
                                                 + "parameterPY."
                                                 + "parameterPZ."
                                                 + "parameterPosition."
                                                 + "parameterX."
                                                 + "parameterY."
                                                 + "parameterZ."
                                                 + "particleHypothesis."
                                                 + "patternRecoInfo."
                                                 + "pixeldEdx."
                                                 + "radiusOfFirstHit."
                                                 + "sigmadeltaphi_0."
                                                 + "sigmadeltaphi_1."
                                                 + "sigmadeltatheta_0."
                                                 + "sigmadeltatheta_1."
                                                 + "standardDeviationOfChi2OS."
                                                 + "trackFitter."
                                                 + "trackLink."
                                                 + "trackParameterCovarianceMatrices."
                                                 + "trackProperties."
                                                 + "vx."
                                                 + "vy"]


# default list for GSFTrackParticles and GSFConversionVertices through Electrons : https://gitlab.cern.ch/atlas/athena/blob/21.2/PhysicsAnalysis/DerivationFramework/DerivationFrameworkEGamma/python/ElectronsCPContent.py and Muons : https://gitlab.cern.ch/atlas/athena/blob/21.2/PhysicsAnalysis/DerivationFramework/DerivationFrameworkMuons/python/MuonsCPContent.py

# TOPQExtraVariables_GSFTrackParticles = ["GSFTrackParticles."]
# TOPQExtraVariables_GSFConversionVertices = ["GSFConversionVertices."]

TOPQExtraVariables_MET_Track = ["MET_Track."
                                + "source."
                                + "sumet"]

############################
# Extra variables HLT
############################
TOPQExtraVariables_BTag_HLT = ["HLT_xAOD__BTaggingContainer_HLTBjetFex."
                               + "MV2c00_discriminant."
                               + "MV2c10_discriminant."
                               + "MV2c20_discriminant."
                               + "BTagBtagToJetAssociator"]

TOPQExtraVariables_HLT_EFJet = ["HLT_xAOD__JetContainer_EFJet."
                                + "eta."
                                + "m."
                                + "pt."
                                + "phi"]

TOPQExtraVariables_HLT_GSCJet = ["HLT_xAOD__JetContainer_GSCJet."
                                 + "eta."
                                 + "m."
                                 + "pt."
                                 + "phi"]

TOPQExtraVariables_HLT_SplitJet = ["HLT_xAOD__JetContainer_SplitJet."
                                   + "eta."
                                   + "m."
                                   + "pt."
                                   + "phi"]

# following ones could certainly be automatized ?
TOPQExtraVariables_HLT_a10r_tcemsubjesFS = ["HLT_xAOD__JetContainer_a10r_tcemsubjesFS."
                                            + "eta."
                                            + "m."
                                            + "pt."
                                            + "phi"]

TOPQExtraVariables_HLT_a10r_tcemsubjesISFS = ["HLT_xAOD__JetContainer_a10r_tcemsubjesISFS."
                                              + "eta."
                                              + "m."
                                              + "pt."
                                              + "phi"]

TOPQExtraVariables_HLT_a10tclcwsubjesFS = ["HLT_xAOD__JetContainer_a10tclcwsubjesFS."
                                           + "eta."
                                           + "m."
                                           + "pt."
                                           + "phi"]

TOPQExtraVariables_HLT_a10ttclcwjesFS = ["HLT_xAOD__JetContainer_a10ttclcwjesFS."
                                         + "eta."
                                         + "m."
                                         + "pt."
                                         + "phi"]

TOPQExtraVariables_HLT_a4tcemsubjesISFS = ["HLT_xAOD__JetContainer_a4tcemsubjesISFS."
                                           + "eta."
                                           + "m."
                                           + "pt."
                                           + "phi"]

TOPQExtraVariables_LVL1EnergySumRoI = ["LVL1EnergySumRoI."
                                       ]
#================================
# MC Only
#================================
TOPQExtraVariables_Photons_Truth   = ["Photons."
                                     + "truthOrigin."
                                     + "truthParticleLink."
                                     + "truthType"]

TOPQExtraVariables_Electrons_Truth = ["Electrons."
                                     + "bkgTruthType."
                                     + "bkgTruthOrigin."
                                     + "bkgTruthParticleLink."
                                     + "bkgMotherPdgId"
                                     + "truthOrigin."
                                     + "truthType."
                                     + "truthParticleLink"]

TOPQExtraVariables_ForwardElectrons_Truth = ["ForwardElectrons."
                                            + "truthOrigin."
                                            + "truthType."
                                            + "truthParticleLink"]

TOPQExtraVariables_Muons_Truth = ["MuonTruthParticles."
                                  + "truthOrigin."
                                  + "truthType"]

TOPQExtraVariables_CombinedMuonTrackParticles_Truth = ["CombinedMuonTrackParticles."
                                                       + "truthOrigin."
                                                       + "truthType"]

TOPQExtraVariables_MuonSpectrometerTrackParticles_Truth = ["MuonSpectrometerTrackParticles."
                                                           + "truthOrigin."
                                                           + "truthType"]

TOPQExtraVariables_ExtrapolatedMuonTrackParticles_Truth = ["ExtrapolatedMuonTrackParticles."
                                                           + "truthOrigin."
                                                           + "truthType"]

TOPQExtraVariables_IndetForwardTrackParticles_Truth = ["InDetForwardTrackParticles."
                                                       + "truthMatchProbability."
                                                       + "truthOrigin."
                                                       + "truthType"]


TOPQExtraVariables_Taus_Truth = ["TauJets."
                                 + "IsTruthMatched."
                                 + "truthJetLink."
                                 + "truthParticleLink"]

TOPQExtraVariables_AntiKt4EMTopoJets_Truth =  ["AntiKt4EMTopoJets."
                                               + "ConeExclTausFinal."         
                                               + "GhostBHadronsFinal."
                                               + "GhostBHadronsFinalCount."
                                               + "GhostBHadronsFinalPt."
                                               + "GhostBHadronsInitial."
                                               + "GhostBHadronsInitialCount."
                                               + "GhostBHadronsInitialPt."
                                               + "GhostBQuarksFinal."
                                               + "GhostBQuarksFinalCount."
                                               + "GhostBQuarksFinalPt."
                                               + "GhostCHadronsFinal."
                                               + "GhostCHadronsFinalCount."
                                               + "GhostCHadronsFinalPt."
                                               + "GhostCHadronsInitial."
                                               + "GhostCHadronsInitialCount."
                                               + "GhostCHadronsInitialPt."
                                               + "GhostCQuarksFinal."
                                               + "GhostCQuarksFinalCount."
                                               + "GhostCQuarksFinalPt."
                                               + "GhostHBosons."
                                               + "GhostHBosonsCount."
                                               + "GhostHBosonsPt."
                                               + "GhostPartons."
                                               + "GhostPartonsCount."
                                               + "GhostPartonsPt."
                                               + "GhostTausFinal."
                                               + "GhostTausFinalCount."
                                               + "GhostTausFinalPt."
                                               + "GhostTQuarksFinal."
                                               + "GhostTQuarksFinalCount."
                                               + "GhostTQuarksFinalPt."
                                               + "GhostTruth."
                                               + "GhostTruthCount."
                                               + "GhostTruthPt."
                                               + "GhostTruthAssociationLink."
                                               + "GhostWBosons."
                                               + "GhostWBosonsCount."
                                               + "GhostWBosonsPt."
                                               + "GhostZBosons."
                                               + "GhostZBosonsCount."
                                               + "GhostZBosonsPt."
                                               + "HadronConeExclTruthLabelID."
                                               + "PartonTruthLabelID."
                                               + "TruthLabelID."
                                               + "TruthLabelDeltaR_B."
                                               + "TruthLabelDeltaR_C."
                                               + "TruthLabelDeltaR_T"]

TOPQExtraVariables_AntiKt4EMPFlowJets_Truth = ["AntiKt4EMPFlowJets."
                                               + "ConeExclBHadronsFinal."
                                               + "ConeExclCHadronsFinal."
                                               + "ConeExclTausFinal."
                                               + "ConeTruthLabelID."
                                               + "GhostBHadronsFinal."
                                               + "GhostBHadronsFinalCount."
                                               + "GhostBHadronsFinalPt."
                                               + "GhostBHadronsInitial."
                                               + "GhostBHadronsInitialCount."
                                               + "GhostBHadronsInitialPt."
                                               + "GhostBQuarksFinal."
                                               + "GhostBQuarksFinalCount."
                                               + "GhostBQuarksFinalPt."
                                               + "GhostCHadronsFinal."
                                               + "GhostCHadronsFinalCount."
                                               + "GhostCHadronsFinalPt."
                                               + "GhostCHadronsInitial."
                                               + "GhostCHadronsInitialCount."
                                               + "GhostCHadronsInitialPt."
                                               + "GhostCQuarksFinal."
                                               + "GhostCQuarksFinalCount."
                                               + "GhostCQuarksFinalPt."
                                               + "GhostHBosons."
                                               + "GhostHBosonsCount."
                                               + "GhostHBosonsPt."
                                               + "GhostPartons."
                                               + "GhostPartonsCount."
                                               + "GhostPartonsPt."
                                               + "GhostTausFinal."
                                               + "GhostTausFinalCount."
                                               + "GhostTausFinalPt."
                                               + "GhostTQuarksFinal."
                                               + "GhostTQuarksFinalCount."
                                               + "GhostTQuarksFinalPt."
                                               + "GhostTruth."
                                               + "GhostTruthCount."
                                               + "GhostTruthPt."
                                               + "GhostTruthAssociationLink."
                                               + "GhostWBosons."
                                               + "GhostWBosonsCount."
                                               + "GhostWBosonsPt."
                                               + "GhostZBosons."
                                               + "GhostZBosonsCount."
                                               + "GhostZBosonsPt."
                                               + "HadronConeExclTruthLabelID."
                                               + "PartonTruthLabelID."
                                               + "TruthLabelID."
                                               + "TruthLabelDeltaR_B."
                                               + "TruthLabelDeltaR_C."
                                               + "TruthLabelDeltaR_T"]

TOPQExtraVariables_AntiKt2PV0TrackJets_Truth = ["AntiKt2PV0TrackJets."
                                                + "GhostBHadronsFinal."
                                                + "GhostBHadronsFinalCount."
                                                + "GhostBHadronsFinalPt."
                                                + "GhostBHadronsInitial."
                                                + "GhostBHadronsInitialCount."
                                                + "GhostBHadronsInitialPt."
                                                + "GhostBQuarksFinal."
                                                + "GhostBQuarksFinalCount."
                                                + "GhostBQuarksFinalPt."
                                                + "GhostCHadronsFinal."
                                                + "GhostCHadronsFinalCount."
                                                + "GhostCHadronsFinalPt."
                                                + "GhostCHadronsInitial."
                                                + "GhostCHadronsInitialCount."
                                                + "GhostCHadronsInitialPt."
                                                + "GhostCQuarksFinal."
                                                + "GhostCQuarksFinalCount."
                                                + "GhostCQuarksFinalPt."
                                                + "GhostHBosons."
                                                + "GhostHBosonsCount."
                                                + "GhostHBosonsPt."
                                                + "GhostMuonSegment."
                                                + "GhostMuonSegmentCount."
                                                + "GhostPartons."
                                                + "GhostPartonsCount."
                                                + "GhostPartonsPt."
                                                + "GhostTQuarksFinal."
                                                + "GhostTQuarksFinalCount."
                                                + "GhostTQuarksFinalPt."
                                                + "GhostTausFinal."
                                                + "GhostTausFinalCount."
                                                + "GhostTausFinalPt."
                                                + "GhostTruth."
                                                + "GhostTruthCount."
                                                + "GhostTruthPt."
                                                + "GhostWBosons."
                                                + "GhostWBosonsCount."
                                                + "GhostWBosonsPt."
                                                + "GhostZBosons."
                                                + "GhostZBosonsCount."
                                                + "GhostZBosonsPt."
                                                + "TruthLabelDeltaR_B."
                                                + "TruthLabelDeltaR_C."
                                                + "TruthLabelDeltaR_T"]

TOPQExtraVariables_AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets_Truth = ["AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets."
                                                                     + "GhostBHadronsFinal."
                                                                     + "GhostBHadronsFinalCount."
                                                                     + "GhostBHadronsFinalPt."
                                                                     + "GhostBHadronsInitial."
                                                                     + "GhostBHadronsInitialCount."
                                                                     + "GhostBHadronsInitialPt."
                                                                     + "GhostBQuarksFinal."
                                                                     + "GhostBQuarksFinalCount."
                                                                     + "GhostBQuarksFinalPt."
                                                                     + "GhostCHadronsFinal."
                                                                     + "GhostCHadronsFinalCount."
                                                                     + "GhostCHadronsFinalPt."
                                                                     + "GhostCHadronsInitial."
                                                                     + "GhostCHadronsInitialCount."
                                                                     + "GhostCHadronsInitialPt."
                                                                     + "GhostCQuarksFinal."
                                                                     + "GhostCQuarksFinalCount."
                                                                     + "GhostCQuarksFinalPt."
                                                                     + "GhostHBosons."
                                                                     + "GhostHBosonsCount."
                                                                     + "GhostHBosonsPt."
                                                                     + "GhostMuonSegment."
                                                                     + "GhostMuonSegmentCount."
                                                                     + "GhostPartons."
                                                                     + "GhostPartonsCount."
                                                                     + "GhostPartonsPt."
                                                                     + "GhostTQuarksFinal."
                                                                     + "GhostTQuarksFinalCount."
                                                                     + "GhostTQuarksFinalPt."
                                                                     + "GhostTausFinal."
                                                                     + "GhostTausFinalCount."
                                                                     + "GhostTausFinalPt."
                                                                     + "GhostTruth."
                                                                     + "GhostTruthCount."
                                                                     + "GhostTruthPt."
                                                                     + "GhostWBosons."
                                                                     + "GhostWBosonsCount."
                                                                     + "GhostWBosonsPt."
                                                                     + "GhostZBosons."
                                                                     + "GhostZBosonsCount."
                                                                     + "GhostZBosonsPt."
                                                                     + "TruthLabelDeltaR_B."
                                                                     + "TruthLabelDeltaR_C."
                                                                     + "TruthLabelDeltaR_T"]

#===========================================
# COLLECTIONS AT TRUTH LEVEL
#===========================================
#TOPQExtraContainersTruth = ["AntiKt10TruthJets",
#                            "AntiKt4TruthJets",
#                            "AntiKt4TruthWZJets",
#                            "MET_Truth",
#                            "MET_TruthRegions",
#                            "TruthEvents",
#                            "TruthParticles",
#                            "TruthVertices"]

TOPQExtraVariables_TruthEvents = ["TruthEvents."
                                  + "PDFID1."
                                  + "PDFID2."
                                  + "PDGID1."
                                  + "PDGID2."
                                  + "Q."
                                  + "X1."
                                  + "X2."
                                  + "XF1."
                                  + "XF2."
                                  + "beamParticle1Link."
                                  + "beamParticle2Link."
                                  + "crossSection."
                                  + "crossSectionError."
                                  + "signalProcessVertexLink."
                                  + "truthParticleLinks."
                                  + "truthVertexLinks."
                                  + "weights"]

TOPQExtraVariables_TruthParticles = ["TruthParticles."
                                     + "TopHadronOriginFlag."
                                     + "barcode."
                                     + "classifierParticleOrigin."
                                     + "classifierParticleOutCome."
                                     + "classifierParticleType."
                                     + "d0."
                                     + "decayVtxLink."
                                     + "dressedPhoton."
                                     + "e."
                                     + "m."
                                     + "pdgId."
                                     + "phi."
                                     + "polarizationPhi."
                                     + "polarizationTheta."
                                     + "prodR."
                                     + "prodVtxLink."
                                     + "prodZ."
                                     + "px."
                                     + "py."
                                     + "pz."
                                     + "qOverP."
                                     + "status."
                                     + "theta."
                                     + "z0."
                                     + "z0st"]

TOPQExtraVariables_TruthVertices = ["TruthVertices."
                                    + "barcode."
                                    + "id."
                                    + "incomingParticleLinks."
                                    + "outgoingParticleLinks."
                                    + "t."
                                    + "x."
                                    + "y."
                                    + "z"]

TOPQExtraVariables_MET_Truth = ["MET_Truth."
                                + "mpx."
                                + "mpy."
                                + "name."
                                + "source."
                                + "sumet"]

TOPQExtraVariables_MET_TruthRegions = ["MET_TruthRegions."
                                       + "mpx."
                                       + "mpy."
                                       + "name."
                                       + "source."
                                       + "sumet"]

#===========================================
# COLLECTIONS WITH All VARIABLES
#===========================================
#TOPQExtraContainersStandard = ["CombinedMuonTrackParticles",
#                               "egammaClusters",
#                               "ExtrapolatedMuonTrackParticles",
#                               "ForwardElectrons",
#                               # "GSFTrackParticles",
#                               "InDetForwardTrackParticles",
#                               "MuonSpectrometerTrackParticles",
#                               "MET_Track"]

#TOPQExtraContainersTrigger = []

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


