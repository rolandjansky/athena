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

#from AthenaCommon.JobProperties import jobproperties
#jobproperties.Global.DetDescrVersion = 'ATLAS-GEO-10-00-00'

include ('AthenaPoolUtilities/TPCnvTest.py')
