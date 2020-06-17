# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

#Content included in addition to the Smart Slimming Content

import HIGG5Common
ExtraContent=HIGG5Common.getHIGG5Common()
ExtraContent+=[
    "GSFTrackParticles.expectNextToInnermostPixelLayerHit.numberOfNextToInnermostPixelLayerHits.numberOfNextToInnermostPixelLayerOutliers",
    "Muons.quality.etcone20.etcone30.etcone40",
    "ExtrapolatedMuonTrackParticles.numberOfTRTHits.numberOfTRTOutliers",
    "BTagging_AntiKtVR30Rmax4Rmin02Track_201810.MV2c10_discriminant.MV2c10mu_discriminant.MV2c10rnn_discriminant.MV2cl100_discriminant.DL1_pu.DL1_pc.DL1_pb.DL1mu_pu.DL1mu_pc.DL1mu_pb.DL1rnn_pu.DL1rnn_pc.DL1rnn_pb",
    "BTagging_AntiKtVR30Rmax4Rmin02Track_201903.MV2c10_discriminant.MV2c10mu_discriminant.MV2c10rnn_discriminant.MV2cl100_discriminant.DL1_pu.DL1_pc.DL1_pb.DL1mu_pu.DL1mu_pc.DL1mu_pb.DL1rnn_pu.DL1rnn_pc.DL1rnn_pb",
    "AntiKt4EMTopoJets.DFCommonJets_QGTagger_NTracks.DFCommonJets_QGTagger_NTracks.DFCommonJets_QGTagger_TracksWidth.DFCommonJets_QGTagger_TracksC1.DFCommonJets_QGTagger_truthjet_nCharged.DFCommonJets_QGTagger_truthjet_pt.DFCommonJets_QGTagger_truthjet_eta.NumTrkPt500PV.PartonTruthLabelID",
    "AntiKt4EMPFlowJets.DFCommonJets_QGTagger_NTracks.DFCommonJets_QGTagger_NTracks.DFCommonJets_QGTagger_TracksWidth.DFCommonJets_QGTagger_TracksC1.DFCommonJets_QGTagger_truthjet_nCharged.DFCommonJets_QGTagger_truthjet_pt.DFCommonJets_QGTagger_truthjet_eta.NumTrkPt500PV.PartonTruthLabelID"
]

ExtraContentTruth=HIGG5Common.getHIGG5CommonTruth()

ExtraContainers=[]

ExtraContainersTruth=HIGG5Common.getHIGG5CommonTruthContainers()
