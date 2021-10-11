#!/usr/bin/env python
"""
Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
"""

from AthenaPoolUtilities.TPCnvTestConfig import TPCnvTest

if __name__ == "__main__":

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

    TPCnvTest(infile, keys, useGeoModelSvc=True, doPixel=True, doSCT=True, doTRT=True, doMuon=True)

