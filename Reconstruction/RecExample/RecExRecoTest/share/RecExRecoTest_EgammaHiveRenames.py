from SGComps.AddressRemappingSvc import addInputRename
addInputRename ('xAOD::CaloClusterContainer','CaloCalTopoClusters', 'old_CaloCalTopoClusters')
addInputRename ('xAOD::CaloClusterAuxContainer','CaloCalTopoClustersAux.', 'old_CaloCalTopoClustersAux.')

addInputRename ('xAOD::ElectronContainer','Electrons', 'old_Electrons')
addInputRename ('xAOD::PhotonContainer','Photons', 'old_Photons')
addInputRename ('xAOD::ElectronAuxContainer','ElectronsAux.', 'old_ElectronsAux.')
addInputRename ('xAOD::PhotonAuxContainer','PhotonsAux.', 'old_PhotonsAux.')

addInputRename ('xAOD::CaloClusterContainer','ForwardElectronClusters', 'old_ForwardElectronClusters')
addInputRename ('xAOD::CaloClusterContainer','egammaClusters', 'old_egammaClusters')
addInputRename ('xAOD::CaloClusterAuxContainer','ForwardElectronClustersAux.', 'old_ForwardElectronClustersAux.')
addInputRename ('xAOD::CaloClusterAuxContainer','egammaClustersAux.', 'old_egammaClustersAux.')

addInputRename ('xAOD::ElectronContainer','ForwardElectrons', 'old_ForwardElectrons')
addInputRename ('xAOD::ElectronAuxContainer','ForwardElectronsAux.', 'old_ForwardElectronsAux.')
addInputRename ('xAOD::TruthParticleContainer','egammaTruthParticles', 'old_egammaTruthParticles')
addInputRename ('xAOD::TruthParticleAuxContainer','egammaTruthParticlesAux.', 'old_egammaTruthParticlesAux.')

addInputRename ('CaloClusterCellLinkContainer','ForwardElectronClusters_links', 'old_ForwardElectronClusters_links')
addInputRename ('CaloClusterCellLinkContainer','egammaClusters_links', 'old_egammaClusters_links')

