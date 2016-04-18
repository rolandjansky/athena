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

from AthenaCommon.JobProperties import jobproperties
jobproperties.Global.DetDescrVersion = 'ATLAS-GEO-10-00-00'
#noMuon = True
#noID = True

include ('AthenaPoolUtilities/TPCnvTest.py')

