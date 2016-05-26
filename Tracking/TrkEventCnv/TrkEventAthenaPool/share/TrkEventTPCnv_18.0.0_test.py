infile = 'aod/AOD-18.0.0/AOD-18.0.0-full.pool.root'
keys = [
    #Trk::VxContainer_tlp2
    'HLT_ConversionVx',
    'SecVertices',
    'AllPhotonsVxCandidates',
    'VxPrimaryCandidate',
    'ConversionCandidate',
    'HLT_PrimVx',

    #Trk::TrackCollection_tlp4
    'Tracks',
    'MuonSlimmedTrackCollection',
    'GSFTracks',

    #Trk::SegmentCollection_tlp2
    'MuonSegments',
    'MuGirlSegments',
         ]

#from AthenaCommon.JobProperties import jobproperties
#jobproperties.Global.DetDescrVersion = 'ATLAS-GEO-10-00-00'

include ('AthenaPoolUtilities/TPCnvTest.py')
