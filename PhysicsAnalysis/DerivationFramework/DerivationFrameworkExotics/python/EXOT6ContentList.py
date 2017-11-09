# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

EXOT6SmartContent = [
    "Electrons",
    "Muons",
    "Photons",
    "InDetTrackParticles",
    "PrimaryVertices",
    "MET_Reference_AntiKt4EMTopo",
    "MET_Reference_AntiKt4LCTopo",
    "AntiKt4LCTopoJets",
    "AntiKt4EMTopoJets",
    "AntiKt4TruthJets",
    "TauJets",
    "BTagging_AntiKt4EMTopo"
]

EXOT6AllVariablesContent = [
    "TruthVertices",
    "MuonTruthParticles",
    "TruthParticles",
    "MET_Truth",
    "MET_LocHadTopo"
]

EXOT6ExtraVariables = [ 
    "Electrons.author.Medium.Tight.Loose.charge.maxEcell_time.maxEcell_energy.maxEcell_gain.maxEcell_onlId.maxEcell_x.maxEcell_y.maxEcell_z.isEMLoose.zvertex.errz.etap.depth", 
    "Photons.maxEcell_time.maxEcell_energy.maxEcell_gain.maxEcell_onlId.maxEcell_x.maxEcell_y.maxEcell_z.isEMLoose.zvertex.errz.etap.depth",
    "egammaClusters.time",
    "TruthEvents.PDGID1.PDGID2.PDFID1.PDFID2.X1.X2.Q.XF1.XF2",
    "TauJets.ptDetectorAxis,phiDetectorAxis,etaDetectorAxis,mDetectorAxis",
    "TauNeutralParticleFlowObjects.pt.eta.phi.m",
    "TauChargedParticleFlowObjects.pt.eta.phi.m"
]

EXOT6UnslimmedContent = [
    'xAOD::TruthParticleContainer#TruthTaus',
    'xAOD::TruthParticleAuxContainer#TruthTausAux.'
]
