infile = 'aod/AOD-18.0.0/AOD-18.0.0-full.pool.root'
keys = [
    #Rec::TrackParticleTruthCollection_p1
    'RefittedExtrapolatedMuonSpectrometerParticlesTruth',
    'CombinedFitMuonParticlesTruth',
    'MuGirlRefittedTrackParticlesTruth',
    'StatCombinedMuonParticlesTruth',
    'ExtrapolatedMuonSpectrometerParticlesTruth',
    'GSFTrackParticleTruthCollection',
    'TrackParticleTruthCollection',
         ]

#noMuon = True
#noID = True

include ('AthenaPoolUtilities/TPCnvTest.py')

