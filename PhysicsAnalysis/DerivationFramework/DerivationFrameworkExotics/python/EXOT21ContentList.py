# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

EXOT21SmartContent = [
    "PrimaryVertices",
    "Electrons",
    "Muons",
    "InDetTrackParticles",
    "AntiKt4EMTopoJets",
    "HLT_xAOD__JetContainer_a4tcemsubjesFS",
    "MET_Reference_AntiKt4EMTopo"
]

EXOT21AllVariablesContent = [
    "METAssoc_AntiKt4EMTopo",
    "MET_Core_AntiKt4EMTopo"
]

EXOT21ExtraVariables = [
    "InDetTrackParticles.numberOfTRTHighThresholdHits.numberOfInnermostPixelLayerHits.numberOfNextToInnermostPixelLayerHits",
    "Electrons.etcone30.etcone40.e255.e335.e333.e337.e377.widths1.widths2.poscs1.poscs2.asy1.pos.pos7.barys1.r33over37allcalo.ecore.deltaEta0.deltaEta2.deltaEta3.deltaPhi0.deltaPhi3.deltaPhiRescaled0.deltaPhiRescaled1.deltaPhiRescaled3",
    "Muons.etcone20.etcone30.etcone40",
    "egammaClusters.rawE",
    "Electrons.truthOrigin.truthType.truthParticleLink.px.py.pz.e.pdgId.barcode.status",
    "Muons.rpcHitTime.truthOrigin.truthType.truthParticleLink.px.py.pz.e.pdgId.barcode.status",
    "Photons.truthOrigin.truthType.truthParticleLink.px.py.pz.e.pdgId.barcode.status",
    "TruthEvents",
    "TruthEventsAux.truthParticleLinks",
    "TruthParticles",
    "TruthParticlesAux.prodVtxLink.decayVtxLink.px.py.pz.e.pdgId.barcode.status",
    "TruthVertices",
    "TruthVerticesAux.incomingParticleLinks.outgoingParticleLinks.x.y"
]

EXOT21UnslimmedContent = []

EXOT21ExtraTruth = [
    'xAOD::TruthParticleContainer#TruthElectrons',
    'xAOD::TruthParticleAuxContainer#TruthElectronsAux.',
    'xAOD::TruthParticleContainer#TruthMuons',
    'xAOD::TruthParticleAuxContainer#TruthMuonsAux.',
    'xAOD::TruthParticleContainer#TruthPhotons',
    'xAOD::TruthParticleAuxContainer#TruthPhotonsAux.',
    "MuonTruthParticles.truthOrigin.truthType.truthParticleLink.px.py.pz.e.pdgId.barcode.status",
    "ElectronTruthParticles.truthOrigin.truthType.truthParticleLink.px.py.pz.e.pdgId.barcode.status",
    "PhotonTruthParticles.truthOrigin.truthType.truthParticleLink.px.py.pz.e.pdgId.barcode.status"
]
