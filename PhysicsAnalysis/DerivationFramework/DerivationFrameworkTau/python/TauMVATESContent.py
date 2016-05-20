# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration


from DerivationFrameworkTau.TauJetsCPContent import TauJetsCPContent as taucore

TauMVATESContent=taucore
TauMVATESContent += [ 
    "TauJets",
    "TauJetsAux.ptDetectorAxis.phiDetectorAxis.etaDetectorAxis.mDetectorAxis.ptPanTauCellBased.etaPanTauCellBased.phiPanTauCellBased.mPanTauCellBased.pantau_CellBasedInput_DecayMode.pantau_CellBasedInput_DecayModeProto",
    "TauNeutralParticleFlowObjects",
    "TauNeutralParticleFlowObjectsAux.pt.eta.phi.m",
    "TauChargedParticleFlowObjects",
    "TauChargedParticleFlowObjectsAux.pt.eta.phi.m",
    "CaloCalTopoClusters",
    "CaloCalTopoClustersAux.CENTER_LAMBDA.FIRST_ENG_DENS.EM_PROBABILITY.SECOND_LAMBDA.e_sampl.eta0.phi0.rawE.rawEta.rawPhi.rawM.altE.altEta.altPhi.altM.calE.calEta.calPhi.calM",
                      ]

