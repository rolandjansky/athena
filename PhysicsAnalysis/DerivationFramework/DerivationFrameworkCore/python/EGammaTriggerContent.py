# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

EGammaTriggerContent = [
    "HLT_xAOD__PhotonContainer_egamma_Photons",
    "HLT_xAOD__PhotonContainer_egamma_PhotonsAux.",
    "HLT_xAOD__ElectronContainer_egamma_Electrons",
    "HLT_xAOD__ElectronContainer_egamma_ElectronsAux.",
    "LVL1EmTauRoIs",
    "LVL1EmTauRoIsAux."
]

# added for rel22 because of Trigger naming changes for Run3 reconstruction
EGammaTriggerContent += [
    "HLT_egamma_Electrons", "HLT_egamma_ElectronsAux.",
    "HLT_egamma_Electrons_GSF", "HLT_egamma_Electrons_GSFAux.",
    "HLT_egamma_Photons", "HLT_egamma_PhotonsAux.",
    # cluster/track containers
    "HLT_CaloEMClusters_Photon", "HLT_CaloEMClusters_PhotonAux.",
    "HLT_CaloEMClusters_Electron", "HLT_CaloEMClusters_ElectronAux.",
    "HLT_IDTrack_Electron_IDTrig", "HLT_IDTrack_Electron_IDTrigAux.",
    "HLT_IDTrack_Electron_GSF", "HLT_IDTrack_Electron_GSFAux.",
    "HLT_TrigEMClusters", "HLT_TrigEMClustersAux."
]
