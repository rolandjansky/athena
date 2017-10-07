excludedAuxData = "-caloExtension.-cellAssociation.-clusterAssociation.-trackParameterCovarianceMatrices.-parameterX.-parameterY.-parameterZ.-parameterPX.-parameterPY.-parameterPZ.-parameterPosition"
StreamESD.ItemList+=['xAOD::TrackParticleContainer#InDetTwoPlusPVTrackParticles']
StreamESD.ItemList+=['xAOD::TrackParticleAuxContainer#InDetTwoPlusPVTrackParticlesAux.' + excludedAuxData]
print "TestTestTest"
print StreamESD
