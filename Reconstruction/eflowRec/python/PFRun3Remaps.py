# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

def ListRemaps():
    #function to get all of the remapped names needed in PFRun3Config.py
    from SGComps.AddressRemappingConfig import InputRenameCfg
    list_remaps=[
        InputRenameCfg ('xAOD::FlowElementContainer','JetETMissChargedFlowElements.FE_ElectronLinks','JetETMissChargedFlowElements.FE_ElectronLinks_renamed'),
        InputRenameCfg ('xAOD::FlowElementContainer','JetETMissChargedFlowElements.FE_PhotonLinks','JetETMissChargedFlowElements.FE_PhotonLinks_renamed'),
        InputRenameCfg ('xAOD::FlowElementContainer','JetETMissNeutralFlowElements.FE_ElectronLinks','JetETMissNeutralFlowElements.FE_ElectronLinks_renamed'),
        InputRenameCfg ('xAOD::FlowElementContainer','JetETMissNeutralFlowElements.FE_PhotonLinks','JetETMissNeutralFlowElements.FE_PhotonLinks_renamed'),
        InputRenameCfg ('xAOD::PFOContainer','JetETMissChargedParticleFlowObjects.pfo_ElectronLinks','JetETMissChargedParticleFlowObjects.pfo_ElectronLinks_renamed'),
        InputRenameCfg ('xAOD::PFOContainer','JetETMissChargedParticleFlowObjects.pfo_PhotonLinks','JetETMissChargedParticleFlowObjects.pfo_PhotonLinks_renamed'),
        InputRenameCfg ('xAOD::PFOContainer','JetETMissNeutralParticleFlowObjects.pfo_ElectronLinks','JetETMissNeutralParticleFlowObjects.pfo_ElectronLinks_renamed'),
        InputRenameCfg ('xAOD::PFOContainer','JetETMissNeutralParticleFlowObjects.pfo_PhotonLinks','JetETMissNeutralParticleFlowObjects.pfo_PhotonLinks_renamed'),
        InputRenameCfg ('xAOD::ElectronContainer','Electrons.chargedFELinks','Electrons.chargedFELinks_renamed'),
        InputRenameCfg ('xAOD::ElectronContainer','Electrons.neutralFELinks','Electrons.neutralFELinks_renamed'),
        InputRenameCfg ('xAOD::PhotonContainer','Photons.chargedFELinks','Photons.chargedFELinks_renamed'),
        InputRenameCfg ('xAOD::PhotonContainer','Photons.neutralFELinks','Photons.neutralFELinks_renamed'),
        InputRenameCfg ('xAOD::ElectronContainer','Electrons.neutralpfoLinks','Electrons.neutralpfoLinks_renamed'),
        InputRenameCfg ('xAOD::ElectronContainer','Electrons.chargedpfoLinks','Electrons.chargedpfoLinks_renamed'),
        InputRenameCfg ('xAOD::PhotonContainer','Photons.neutralpfoLinks','Photons.neutralpfoLinks_renamed'),
        InputRenameCfg ('xAOD::PhotonContainer','Photons.chargedpfoLinks','Photons.chargedpfoLinks_renamed'),
        #Remap the Muon decorations for FE
        InputRenameCfg ('xAOD::MuonContainer','Muons.chargedFELinks','Muons.chargedFELinks_renamed'),
        InputRenameCfg ('xAOD::MuonContainer','Muons.neutralFELinks','Muons.neutralFELinks_renamed'),
        InputRenameCfg ('xAOD::MuonContainer','Muons.muon_efrac_matched_FE','Muons.muon_efrac_matched_FE_renamed'),
        InputRenameCfg ('xAOD::MuonContainer','Muons.ClusterInfo_deltaR','Muons.ClusterInfo_deltaR_renamed'),
        InputRenameCfg ('xAOD::FlowElementContainer','JetETMissChargedFlowElements.FE_MuonLinks','JetETMissChargedFlowElements.FE_MuonLinks_renamed'),
        InputRenameCfg ('xAOD::FlowElementContainer','JetETMissNeutralFlowElements.FE_MuonLinks','JetETMissNeutralFlowElements.FE_MuonLinks_renamed'),
        InputRenameCfg ('xAOD::FlowElementContainer','JetETMissNeutralFlowElements.FE_efrac_matched_muon_MuonLinks','JetETMissNeutralFlowElements.FE_efrac_matched_muon_renamed'),
        InputRenameCfg ('xAOD::FlowElementContainer','JetETMissNeutralFlowElements.FE_nMatchedMuons','JetETMissNeutralFlowElements.FE_nMatchedMuons_renamed'),
        #Remap the Tau decorations for FE
        InputRenameCfg ('xAOD::TauJetContainer','TauJets.neutralFELinks','TauJets.neutralFELinks_renamed'),
        InputRenameCfg ('xAOD::TauJetContainer','TauJets.chargedFELinks','TauJets.chargedFELinks_renamed'),
        InputRenameCfg ('xAOD::FlowElementContainer','JetETMissChargedFlowElements.FE_TauLinks','JetETMissChargedFlowElements.FE_TauLinks_renamed'),
        InputRenameCfg ('xAOD::FlowElementContainer','JetETMissNeutralFlowElements.FE_TauLinks','JetETMissNeutralFlowElements.FE_TauLinks_renamed'),
        #Remap the calibrated and origin corrected topoclusters
        InputRenameCfg ('xAOD::CaloClusterContainer','CaloCalTopoClusters','CaloCalTopoClusters_renamed'),
        InputRenameCfg ('xAOD::CaloClusterContainer','LCOriginTopoClusters','LCOriginTopoClusters_renamed'),
        InputRenameCfg ('xAOD::CaloClusterContainer','EMOriginTopoClusters','EMOriginTopoClusters_renamed'),
        #Remap the muon cluster links to topoclusters
        InputRenameCfg ('xAOD::CaloClusterContainer','MuonClusterCollection.constituentClusterLinks','MuonClusterCollection.constituentClusterLinks_renamed')
    ]
    
    return list_remaps
