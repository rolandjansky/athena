infile = 'aod/AOD-14.5.0/AOD-14.5.0-full.pool.root'
keys = [
    #Trk::VxContainer_tlp1
    'VxPrimaryCandidate',
    'ConversionCandidate',
    'HLT_PrimVx',

    #Trk::TrackCollection_tlp2
    'Tracks',
    'MuonSlimmedTrackCollection',

    #Trk::SegmentCollection_tlp1
    'ConvertedMBoySegments',
    'MuGirlSegments',
    'MooreSegments',

    #V0Container_tlp1
    'V0Candidates',
         ]

include ('AthenaPoolUtilities/TPCnvTest.py')
