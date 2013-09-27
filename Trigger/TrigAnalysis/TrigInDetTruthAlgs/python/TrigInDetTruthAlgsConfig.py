# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from TrigInDetTruthAlgs.TrigInDetTruthAlgsConf import TrigInDetTrackTruthMaker

# configurable class
class TrigIDTruthMaker ( TrigInDetTrackTruthMaker ):
    __slots__ = []
    def __init__(self, name="TrigInDetTrackTruthMaker"):
        super( TrigInDetTrackTruthMaker, self ).__init__( name )

    def setDefaults(self, handle):
        TrigIDTrackTruthMapName = "TrigInDetTrackTruthMap"
        PixelSDO_MapName = "PixelSDO_Map"
        SCT_SDO_MapName = "SCT_SDO_Map"
        TRT_SDO_MapName = "TRT_SDO_Map"
        MinNrMatchingHits = 2
        TrigInDetTrackCollectionKeys= ["HLT",
                                       "HLT_TrigIDSCAN_Jet",
                                       "HLT_TrigIDSCAN_Tau", 
                                       "HLT_TrigIDSCAN_eGamma",
                                       "HLT_TrigIDSCAN_Muon", 
                                       "HLT_TrigIDSCAN_muonIso",
                                       "HLT_TrigIDSCAN_Bphysics", 
                                       "HLT_TrigIDSCAN_FullScan",
                                       #"HLT_TrigIDSCAN_BeamSpot",
                                       "HLT_TrigIDSCAN_Cosmics", 
                                       "HLT_TrigIDSCAN_eGamma_Brem",
                                       "HLT_TrigIDSCAN_Tile", 
                                       "HLT_TrigSiTrack_eGamma",
                                       "HLT_TrigSiTrack_Muon", 
                                       "HLT_TrigSiTrack_muonIso",
                                       "HLT_TrigSiTrack_Tau", 
                                       "HLT_TrigSiTrack_Jet",
                                       "HLT_TrigSiTrack_Bphysics", 
                                       "HLT_TrigSiTrack_FullScan",
                                       #"HLT_TrigSiTrack_BeamSpot",
                                       "HLT_TrigSiTrack_Tile",
                                       "HLT_TrigSiTrack_Cosmics",
                                       "HLT_TrigSiTrack_eGamma_robust",
                                       "HLT_TrigSiTrack_Muon_robust", 
                                       "HLT_TrigSiTrack_muonIso_robust",
                                       "HLT_TrigSiTrack_Tau_robust", 
                                       "HLT_TrigSiTrack_Jet_robust",
                                       "HLT_TrigSiTrack_Bphysics_robust", 
                                       "HLT_TrigSiTrack_FullScan_robust",
                                       "HLT_TrigSiTrack_Tile_robust",
                                       "HLT_TrigL2SiTrackFinder_MuonA",
                                       "HLT_TRTSegmentFinder", 
                                       "HLT_TRTxK"]
        
