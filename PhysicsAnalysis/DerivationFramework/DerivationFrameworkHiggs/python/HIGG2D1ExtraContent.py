# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

#Content included in addition to the Smart Slimming Content

HIGG2D1ExtraContent=[
    "egammaClusters.rawE.phi_sampl.calM",
    "Muons.ptconecoreTrackPtrCorrection",
    "AntiKt4EMTopoJets.JetEMScaleMomentum_pt.JetEMScaleMomentum_eta.JetEMScaleMomentum_phi.JetEMScaleMomentum_m",
    "TauJets.ptDetectorAxis,phiDetectorAxis,etaDetectorAxis,mDetectorAxis.IsTruthMatched.truthJetLink.truthParticleLink",
    "TauNeutralParticleFlowObjects.pt.eta.phi.m",
    "TauChargedParticleFlowObjects.pt.eta.phi.m",
    "MuonClusterCollection.altM.calM.time.eta0.phi0.rawE.rawM.altE.altEta.altPhi.rawEta.rawPhi.clusterSize.CellLink.calE.calPhi.calEta.e_sampl.phi_sampl.eta_sampl.constituentClusterLinks",
    "CaloCalTopoClusters.altM.calM.time.eta0.phi0.rawE.rawM.altE.altEta.altPhi.rawEta.rawPhi.clusterSize.CellLink.calE.calPhi.calEta.e_sampl.phi_sampl.eta_sampl.constituentClusterLinks"
    ]

HIGG2D1ExtraContentTruth=[
    "TruthEvents.PDFID1.PDFID2.PDGID1.PDGID2.Q.X1.X2.XF1.XF2.weights.crossSection.crossSectionError",
    "TruthParticles.barcode.prodVtxLink.decayVtxLink.status.pdgId.e.m.px.py.pz",
    "TruthVertices.barcode.id.incomingParticleLinks.outgoingParticleLinks.x.y.z.t"
    ]

HIGG2D1ExtraContainers=[
    "MET_Track",
    ]

HIGG2D1ExtraContainersTruth=[
    "AntiKt4TruthJets",
    "AntiKt4TruthWZJets",
    "MET_Truth",
    "MET_TruthRegions"
    "TruthElectrons",
    "TruthMuons",
    "TruthTaus",
    "TruthPhotons",
    "TruthNeutrinos",
    "TruthTop",
    "TruthBSM",
    "TruthBoson"
    ]
