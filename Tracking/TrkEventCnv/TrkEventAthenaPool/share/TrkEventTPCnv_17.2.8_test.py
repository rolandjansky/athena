infile = 'aod/AOD-17.2.8/AOD-17.2.8-full.pool.root'
keys = [
    #Trk::VxContainer_tlp2
    'HLT_ConversionVx',
    'SecVertices',
    'AllPhotonsVxCandidates',
    'VxPrimaryCandidate',
    'ConversionCandidate',
    'HLT_PrimVx',

    #Trk::TrackCollection_tlp3
    'Tracks',
    'MuonSlimmedTrackCollection',
    'GSFTracks',

    #Trk::SegmentCollection_tlp1
    'ConvertedMBoySegments',
    'MuonSegments',
    'MooreSegments',
    'MuGirlSegments',
         ]

include ('AthenaPoolUtilities/TPCnvTest.py')
