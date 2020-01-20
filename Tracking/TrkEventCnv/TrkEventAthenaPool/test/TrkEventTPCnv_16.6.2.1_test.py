#!/usr/bin/env python
"""
Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
"""

from AthenaPoolUtilities.TPCnvTestConfig import TPCnvTest

if __name__ == "__main__":

    infile = 'aod/AOD-16.6.2.1/AOD-16.6.2.1-full.pool.root'
    keys = [
        #MVFVxContainer_tlp1
        #43.479 kb        7.172 kb        0.717 kb        0.164       20  (T) MVFVxContainer_tlp1???

        #Trk::VxContainer_tlp2
        'SecVertices',
        'AllPhotonsVxCandidates',
        'VxPrimaryCandidate',
        'HLT_ConversionVx',
        'HLT_PrimVx',
        'ConversionCandidate',

        #Trk::TrackCollection_tlp3
        'MuonSlimmedTrackCollection',
        'Tracks',

        #Trk::SegmentCollection_tlp1
        'ConvertedMBoySegments',
        'MuGirlSegments',
        'MooreSegments',

        #V0Container_tlp2
        'V0Candidates',
    ]

    TPCnvTest(infile, keys, useGeoModelSvc=True, doPixel=True, doSCT=True, doTRT=True, doMuon=True)

