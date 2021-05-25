#This file remaps items that we need to recreate when we rerun flavor tagging from ESD
from SGComps import AddressRemappingSvc
#ID Decorations
AddressRemappingSvc.addInputRename ('xAOD::TrackParticleContainer','InDetTrackParticles.btagIp_d0Uncertainty','InDetTrackParticles.btagIp_d0Uncertainty_renamed')
AddressRemappingSvc.addInputRename ('xAOD::TrackParticleContainer','InDetTrackParticles.btagIp_d0','InDetTrackParticles.btagIp_d0_renamed')
AddressRemappingSvc.addInputRename ('xAOD::TrackParticleContainer','InDetTrackParticles.btagIp_z0SinThetaUncertainty','InDetTrackParticles.btagIp_z0SinThetaUncertainty_renamed')
AddressRemappingSvc.addInputRename ('xAOD::TrackParticleContainer','InDetTrackParticles.btagIp_d0','InDetTrackParticles.btagIp_d0_renamed')
AddressRemappingSvc.addInputRename ('xAOD::TrackParticleContainer','InDetTrackParticles.btagIp_z0SinTheta','InDetTrackParticles.btagIp_z0SinTheta_renamed')
AddressRemappingSvc.addInputRename ('xAOD::TrackParticleContainer','InDetTrackParticles.btagIp_trackMomentum','InDetTrackParticles.btagIp_trackMomentum_renamed')
AddressRemappingSvc.addInputRename ('xAOD::TrackParticleContainer','InDetTrackParticles.btagIp_trackDisplacement','InDetTrackParticles.btagIp_trackDisplacement_renamed')
AddressRemappingSvc.addInputRename ('xAOD::TrackParticleContainer','InDetTrackParticles.TrackCompatibility','InDetTrackParticles.TrackCompatibility_renamed')
#Jet Decorations
AddressRemappingSvc.addInputRename ('xAOD::JetContainer','AntiKt4EMTopoJets.BTagTrackToJetAssociator','AntiKt4EMTopoJets.BTagTrackToJetAssociator_renamed')
AddressRemappingSvc.addInputRename ('xAOD::JetContainer','AntiKt4EMTopoJets.JFVtx','AntiKt4EMTopoJets.JFVtx_renamed')
AddressRemappingSvc.addInputRename ('xAOD::JetContainer','AntiKt4EMTopoJets.SecVtx','AntiKt4EMTopoJets.SecVtx_renamed')
AddressRemappingSvc.addInputRename ('xAOD::JetContainer','AntiKt4EMTopoJets.btaggingLink','AntiKt4EMTopoJets.btaggingLink_renamed')