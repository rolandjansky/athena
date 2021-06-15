#!/usr/bin/env python
"""
Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
"""

from AthenaPoolUtilities.TPCnvTestConfig import TPCnvTest

if __name__ == "__main__":

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

    TPCnvTest(infile, keys, useGeoModelSvc=True, doPixel=True, doSCT=True, doTRT=True, doMuon=True)

