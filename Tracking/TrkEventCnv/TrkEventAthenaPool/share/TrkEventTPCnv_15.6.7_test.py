infile = 'aod/AOD-15.6.7/AOD-15.6.7-full.pool.root'
keys = [
    #MVFVxContainer_tlp1
    #43.479 kb        7.172 kb        0.717 kb        0.164       20  (T) MVFVxContainer_tlp1???

    #Trk::VxContainer_tlp1
    'HLT_ConversionVx',
    'AllPhotonsVxCandidates',
    'VxPrimaryCandidate',
    'ConversionCandidate',
    'HLT_PrimVx',

    #Trk::TrackCollection_tlp3
    'Tracks',
    'MuonSlimmedTrackCollection',

    #Trk::SegmentCollection_tlp1
    # Actually MdtTrackSegment, which was removed from the release.
    #'HLT_MDTsegments',
    'MuGirlSegments',
    'ConvertedMBoySegments',
    'MooreSegments',

    #V0Container_tlp1
    'V0Candidates',
         ]

include ('AthenaPoolUtilities/TPCnvTest.py')
