# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

#Content included in addition to the Smart Slimming Content

import HIGG5Common
ExtraContent=HIGG5Common.getHIGG5Common()
ExtraContent+=[
    "GSFTrackParticles.expectNextToInnermostPixelLayerHit.numberOfNextToInnermostPixelLayerHits.numberOfNextToInnermostPixelLayerOutliers",
    "Muons.quality.etcone20.etcone30.etcone40",
    "ExtrapolatedMuonTrackParticles.numberOfTRTHits.numberOfTRTOutliers",
    "BTagging_AntiKtVR30Rmax4Rmin02Track.MV2c10_discriminant.MV2c10mu_discriminant.MV2c10rnn_discriminant.MV2cl100_discriminant.DL1_pu.DL1_pc.DL1_pb.DL1mu_pu.DL1mu_pc.DL1mu_pb.DL1rnn_pu.DL1rnn_pc.DL1rnn_pb"
]

ExtraContentTruth=HIGG5Common.getHIGG5CommonTruth()

ExtraContainers=[]

ExtraContainersTruth=[]
