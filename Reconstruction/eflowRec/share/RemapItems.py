from SGComps import AddressRemappingSvc
#Remap the EGamma Decorations for PFO and FE
AddressRemappingSvc.addInputRename ('xAOD::FlowElementContainer','JetETMissChargedFlowElements.FE_ElectronLinks','JetETMissChargedFlowElements.FE_ElectronLinks_renamed')
AddressRemappingSvc.addInputRename ('xAOD::FlowElementContainer','JetETMissChargedFlowElements.FE_PhotonLinks','JetETMissChargedFlowElements.FE_PhotonLinks_renamed')
AddressRemappingSvc.addInputRename ('xAOD::FlowElementContainer','JetETMissNeutralFlowElements.FE_ElectronLinks','JetETMissNeutralFlowElements.FE_ElectronLinks_renamed')
AddressRemappingSvc.addInputRename ('xAOD::FlowElementContainer','JetETMissNeutralFlowElements.FE_PhotonLinks','JetETMissNeutralFlowElements.FE_PhotonLinks_renamed')
AddressRemappingSvc.addInputRename ('xAOD::PFOContainer','JetETMissChargedParticleFlowObjects.pfo_ElectronLinks','JetETMissChargedParticleFlowObjects.pfo_ElectronLinks_renamed')
AddressRemappingSvc.addInputRename ('xAOD::PFOContainer','JetETMissChargedParticleFlowObjects.pfo_PhotonLinks','JetETMissChargedParticleFlowObjects.pfo_PhotonLinks_renamed')
AddressRemappingSvc.addInputRename ('xAOD::PFOContainer','JetETMissNeutralParticleFlowObjects.pfo_ElectronLinks','JetETMissNeutralParticleFlowObjects.pfo_ElectronLinks_renamed')
AddressRemappingSvc.addInputRename ('xAOD::PFOContainer','JetETMissNeutralParticleFlowObjects.pfo_PhotonLinks','JetETMissNeutralParticleFlowObjects.pfo_PhotonLinks_renamed')

AddressRemappingSvc.addInputRename ('xAOD::ElectronContainer','Electrons.chargedFELinks','Electrons.chargedFELinks_renamed')
AddressRemappingSvc.addInputRename ('xAOD::ElectronContainer','Electrons.neutralFELinks','Electrons.neutralFELinks_renamed')
AddressRemappingSvc.addInputRename ('xAOD::PhotonContainer','Photons.chargedFELinks','Photons.chargedFELinks_renamed')
AddressRemappingSvc.addInputRename ('xAOD::PhotonContainer','Photons.neutralFELinks','Photons.neutralFELinks_renamed')
AddressRemappingSvc.addInputRename ('xAOD::ElectronContainer','Electrons.neutralpfoLinks','Electrons.neutralpfoLinks_renamed')
AddressRemappingSvc.addInputRename ('xAOD::ElectronContainer','Electrons.chargedpfoLinks','Electrons.chargedpfoLinks_renamed')
AddressRemappingSvc.addInputRename ('xAOD::PhotonContainer','Photons.neutralpfoLinks','Photons.neutralpfoLinks_renamed')
AddressRemappingSvc.addInputRename ('xAOD::PhotonContainer','Photons.chargedpfoLinks','Photons.chargedpfoLinks_renamed')
#Remap the Muon decorations for FE
AddressRemappingSvc.addInputRename ('xAOD::MuonContainer','Muons.chargedFELinks','Muons.chargedFELinks_renamed')
AddressRemappingSvc.addInputRename ('xAOD::MuonContainer','Muons.neutralFELinks','Muons.neutralFELinks_renamed')
AddressRemappingSvc.addInputRename ('xAOD::MuonContainer','Muons.muon_efrac_matched_FE','Muons.muon_efrac_matched_FE_renamed')
AddressRemappingSvc.addInputRename ('xAOD::MuonContainer','Muons.ClusterInfo_deltaR','Muons.ClusterInfo_deltaR_renamed')
AddressRemappingSvc.addInputRename ('xAOD::FlowElementContainer','JetETMissChargedFlowElements.FE_MuonLinks','JetETMissChargedFlowElements.FE_MuonLinks_renamed')
AddressRemappingSvc.addInputRename ('xAOD::FlowElementContainer','JetETMissNeutralFlowElements.FE_MuonLinks','JetETMissNeutralFlowElements.FE_MuonLinks_renamed')
AddressRemappingSvc.addInputRename ('xAOD::FlowElementContainer','JetETMissNeutralFlowElements.FE_efrac_matched_muon_MuonLinks','JetETMissNeutralFlowElements.FE_efrac_matched_muon_renamed')
AddressRemappingSvc.addInputRename ('xAOD::FlowElementContainer','JetETMissNeutralFlowElements.FE_nMatchedMuons','JetETMissNeutralFlowElements.FE_nMatchedMuons_renamed')
#Remap the Tau decorations for FE
AddressRemappingSvc.addInputRename ('xAOD::TauJetContainer','TauJets.neutralFELinks','TauJets.neutralFELinks_renamed')
AddressRemappingSvc.addInputRename ('xAOD::TauJetContainer','TauJets.chargedFELinks','TauJets.chargedFELinks_renamed') 
AddressRemappingSvc.addInputRename ('xAOD::FlowElementContainer','JetETMissChargedFlowElements.FE_TauLinks','JetETMissChargedFlowElements.FE_TauLinks_renamed')
AddressRemappingSvc.addInputRename ('xAOD::FlowElementContainer','JetETMissNeutralFlowElements.FE_TauLinks','JetETMissNeutralFlowElements.FE_TauLinks_renamed')
#Remap the origin corrected topoclusters
AddressRemappingSvc.addInputRename ('xAOD::CaloClusterContainer','LCOriginTopoClusters','LCOriginTopoClusters_renamed')
AddressRemappingSvc.addInputRename ('xAOD::CaloClusterContainer','EMOriginTopoClusters','EMOriginTopoClusters_renamed')
#Remap the muon cluster links to topoclusters
AddressRemappingSvc.addInputRename ('xAOD::CaloClusterContainer','MuonClusterCollection.constituentClusterLinks','MuonClusterCollection.constituentClusterLinks_renamed')