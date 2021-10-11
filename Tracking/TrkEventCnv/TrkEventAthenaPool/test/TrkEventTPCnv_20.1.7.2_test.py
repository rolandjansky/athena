#!/usr/bin/env python
"""
Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
"""

from AthenaPoolUtilities.TPCnvTestConfig import TPCnvTest

if __name__ == "__main__":

    infile = 'esd/ESD-20.1.7.2.pool.root'
    keys = [
        #Trk::TrackCollection_tlp5
        'HLT_TrackCollection_InDetTrigTrackSlimmer_Bjet_EFID',
        'HLT_TrackCollection_InDetTrigTrackSlimmer_Photon_EFID',
        'HLT_TrackCollection_InDetTrigTrackSlimmer_Bphysics_EFID',
        'HLT_TrackCollection_InDetTrigTrackSlimmer_CosmicsN_EFID',
        'HLT_TrackCollection_InDetTrigTrackSlimmer_FullScan_EFID',
        'HLT_TrackCollection_InDetTrigTrackSlimmer_Muon_EFID',
        'ResolvedForwardTracks',
        'MuonSpectrometerTracks',
        'ExtrapolatedMuonTracks',
        'CombinedMuonTracks',
        'GSFTracks',
        'HLT_TrackCollection_InDetTrigTrackSlimmer_Tau_EFID',
        'HLT_TrackCollection_InDetTrigTrackSlimmer_Electron_EFID',
        'Tracks@10',

        #Trk::SegmentCollection_tlp3
        'MuonSegments',
    ]

    TPCnvTest(infile, keys, useGeoModelSvc=True, doPixel=True, doSCT=True, doTRT=True, doMuon=True)
