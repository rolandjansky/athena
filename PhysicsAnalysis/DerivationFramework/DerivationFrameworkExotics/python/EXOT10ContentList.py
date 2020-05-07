# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

EXOT10SmartContent = [
        "Photons",
        "Muons",
        "Electrons",
        "PrimaryVertices",
        "InDetTrackParticles",
        "BTagging_AntiKt4EMTopo",
        "TauJets",
        "AntiKt4EMTopoJets",
        "AntiKt4LCTopoJets"
]

EXOT10AllVariablesContent = [
        "egammaClusters",
        "GSFConversionVertices",
        "TruthParticles",
        "egammaTruthParticles",
        "MET_Truth",
        "egammaTopoSeededClusters",
        "CaloCalTopoClusters",
        "TruthEvents",
        "TruthVertices",
        "AntiKt4TruthJets",
        "MET_LocHadTopo",
        "METAssoc_AntiKt4EMTopo",
        "MET_Core_AntiKt4EMTopo",
        "MET_Reference_AntiKt4EMTopo",
        "METAssoc_AntiKt4LCTopo",
        "MET_Core_AntiKt4LCTopo",
        "MET_Reference_AntiKt4LCTopo",
        "AntiKt4PV0TrackJets",
        "BTagging_AntiKt4Track"
]

EXOT10Content = [
]

EXOT10ExtraVariables = [
        "Electrons.author.Medium.Tight.Loose.charge",
        "BTagging_AntiKt4EMTopo.MV1_discriminant.MV1c_discriminant",
        "Muons.ptcone30.ptcone20.charge.quality.InnerDetectorPt.MuonSpectrometerPt.CaloLRLikelihood.CaloMuonIDTag",
        "AntiKt4EMTopoJets.NumTrkPt1000.TrackWidthPt1000.NumTrkPt500",
        "GSFTrackParticles.z0.d0.vz.definingParametersCovMatrix",
        "CombinedMuonTrackParticles.d0.z0.vz.definingParametersCovMatrix.truthOrigin.truthType",
        "ExtrapolatedMuonTrackParticles.d0.z0.vz.definingParametersCovMatrix.truthOrigin.truthType",
        "TauJets.TruthCharge.TruthProng.IsTruthMatched.TruthPtVis.truthOrigin.truthType",
        "MuonTruthParticles.barcode.decayVtxLink.e.m.pdgId.prodVtxLink.px.py.pz.recoMuonLink.status.truthOrigin.truthType",
        #     "AntiKt4LCTopoJets.AverageLArQF"
]
