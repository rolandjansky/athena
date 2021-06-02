# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

def getCollectionMerger(name="ISF_CollectionMerger", **kwargs):
    kwargs.setdefault( "InputBCMHits",              [ ] )
    kwargs.setdefault( "InputBLMHits",              [ ] )
    kwargs.setdefault( "InputPixelHits",            [ ] )
    kwargs.setdefault( "InputSCTHits",              [ ] )
    kwargs.setdefault( "InputTRTUncompressedHits",  [ ] )

    kwargs.setdefault( "InputLArEMBHits",           [ ] )
    kwargs.setdefault( "InputLArEMECHits",          [ ] )
    kwargs.setdefault( "InputLArFCALHits",          [ ] )
    kwargs.setdefault( "InputLArHECHits",           [ ] )

    kwargs.setdefault( "InputTileHits",             [ ] )
    kwargs.setdefault( "InputMBTSHits",             [ ] )

    kwargs.setdefault( "InputCSCHits",              [ ] )
    kwargs.setdefault( "InputMDTHits",              [ ] )
    kwargs.setdefault( "InputRPCHits",              [ ] )
    kwargs.setdefault( "InputTGCHits",              [ ] )

    kwargs.setdefault( "OutputBCMHits",             "BCMHits"             )
    kwargs.setdefault( "OutputBLMHits",             "BLMHits"             )
    kwargs.setdefault( "OutputPixelHits",           "PixelHits"           )
    kwargs.setdefault( "OutputSCTHits",             "SCT_Hits"            )
    kwargs.setdefault( "OutputTRTUncompressedHits", "TRTUncompressedHits" )

    kwargs.setdefault( "OutputLArEMBHits",          "LArHitEMB"           )
    kwargs.setdefault( "OutputLArEMECHits",         "LArHitEMEC"          )
    kwargs.setdefault( "OutputLArFCALHits",         "LArHitFCAL"          )
    kwargs.setdefault( "OutputLArHECHits",          "LArHitHEC"           )

    kwargs.setdefault( "OutputTileHits",            "TileHitVec"          )
    kwargs.setdefault( "OutputMBTSHits",            "MBTSHits"            )

    kwargs.setdefault( "OutputCSCHits",             "CSC_Hits"            )
    kwargs.setdefault( "OutputMDTHits",             "MDT_Hits"            )
    kwargs.setdefault( "OutputRPCHits",             "RPC_Hits"            )
    kwargs.setdefault( "OutputTGCHits",             "TGC_Hits"            )

    from ISF_Algorithms.ISF_AlgorithmsConf import ISF__CollectionMerger
    collectionMerger = ISF__CollectionMerger(name, **kwargs)
    return collectionMerger

def getSimEventFilter(name="ISF_SimEventFilter", **kwargs):
    kwargs.setdefault( "InputHardScatterCollection", "BeamTruthEvent")
    kwargs.setdefault( "GenParticleCommonFilters", ['ISF_ParticlePositionFilterDynamic','ISF_EtaPhiFilter','ISF_GenParticleInteractingFilter'] )
    kwargs.setdefault( "GenParticleOldFilters", ['ISF_ParticleFinalStateFilter'] )
    kwargs.setdefault( "GenParticleNewFilters", ['ISF_ParticleSimWhiteList_ExtraParticles'] )

    from ISF_Algorithms.ISF_AlgorithmsConf import ISF__SimEventFilter
    simEventFilter = ISF__SimEventFilter(name, **kwargs)
    return simEventFilter

def getInvertedSimEventFilter(name="ISF_InvertedSimEventFilter", **kwargs):
    kwargs.setdefault("InvertFilter", True)
    return getSimEventFilter(name, **kwargs)

def getRenameHitCollections(name="RenameHitCollections", **kwargs):
    kwargs.setdefault( "InputMcEventCollection",    "TruthEventOLD" )
    kwargs.setdefault( "InputBCMHits",              "BCMHitsOLD" )
    kwargs.setdefault( "InputBLMHits",              "BLMHitsOLD" )
    kwargs.setdefault( "InputPixelHits",            "PixelHitsOLD" )
    kwargs.setdefault( "InputSCTHits",              "SCT_HitsOLD" )
    kwargs.setdefault( "InputTRTUncompressedHits",  "TRTUncompressedHitsOLD" )

    kwargs.setdefault( "InputLArEMBHits",           "LArHitEMBOLD" )
    kwargs.setdefault( "InputLArEMECHits",          "LArHitEMECOLD" )
    kwargs.setdefault( "InputLArFCALHits",          "LArHitFCALOLD" )
    kwargs.setdefault( "InputLArMiniFCALHits",      "LArHitMiniFCALOLD" )
    kwargs.setdefault( "InputLArHECHits",           "LArHitHECOLD" )
    #TODO: proper treatment of calibration hits
    #kwargs.setdefault( "InputLArActiveCalibHits", "LArCalibrationHitActive");
    #kwargs.setdefault( "InputLArInactiveCalibHits", "LArCalibrationHitInactive");
    #kwargs.setdefault( "InputLArDeadCalibHits", "LArCalibrationHitDeadMaterial");

    kwargs.setdefault( "InputTileHits",             "TileHitVecOLD" )
    kwargs.setdefault( "InputMBTSHits",             "MBTSHitsOLD" )
    #TODO: proper treatment of calibration hits
    #kwargs.setdefault( "InputTileActiveCalibHits", "TileCalibHitActiveCellOLD");
    #kwargs.setdefault( "InputTileInactiveCalibHits", "TileCalibHitInactiveCellOLD");
    #kwargs.setdefault( "InputTileDeadCalibHits", "TileCalibHitDeadMaterialOLD");

    kwargs.setdefault( "InputCSCHits",              "CSC_HitsOLD" )
    kwargs.setdefault( "InputMDTHits",              "MDT_HitsOLD" )
    kwargs.setdefault( "InputRPCHits",              "RPC_HitsOLD" )
    kwargs.setdefault( "InputTGCHits",              "TGC_HitsOLD" )

    kwargs.setdefault( "InputCaloEntryLayer",       "CaloEntryLayerOLD" )
    kwargs.setdefault( "InputMuonEntryLayer",       "MuonEntryLayerOLD" )
    kwargs.setdefault( "InputMuonExitLayer",        "MuonExitLayerOLD" )

    kwargs.setdefault( "OutputMcEventCollection",   "TruthEvent"          )
    kwargs.setdefault( "OutputBCMHits",             "BCMHits"             )
    kwargs.setdefault( "OutputBLMHits",             "BLMHits"             )
    kwargs.setdefault( "OutputPixelHits",           "PixelHits"           )
    kwargs.setdefault( "OutputSCTHits",             "SCT_Hits"            )
    kwargs.setdefault( "OutputTRTUncompressedHits", "TRTUncompressedHits" )

    kwargs.setdefault( "OutputLArEMBHits",          "LArHitEMB"           )
    kwargs.setdefault( "OutputLArEMECHits",         "LArHitEMEC"          )
    kwargs.setdefault( "OutputLArFCALHits",         "LArHitFCAL"          )
    kwargs.setdefault( "OutputLArMiniFCALHits",     "LArHitMiniFCAL"      )
    kwargs.setdefault( "OutputLArHECHits",          "LArHitHEC"           )
    kwargs.setdefault( "OutputLArActiveCalibHits", "LArCalibrationHitActive");
    kwargs.setdefault( "OutputLArInactiveCalibHits", "LArCalibrationHitInactive");
    kwargs.setdefault( "OutputLArDeadCalibHits", "LArCalibrationHitDeadMaterial");

    kwargs.setdefault( "OutputTileHits",            "TileHitVec"          )
    kwargs.setdefault( "OutputMBTSHits",            "MBTSHits"            )
    kwargs.setdefault( "OutputTileActiveCalibHits", "TileCalibHitActiveCell");
    kwargs.setdefault( "OutputTileInactiveCalibHits", "TileCalibHitInactiveCell");
    kwargs.setdefault( "OutputTileDeadCalibHits", "TileCalibHitDeadMaterial");

    kwargs.setdefault( "OutputCSCHits",             "CSC_Hits"            )
    kwargs.setdefault( "OutputMDTHits",             "MDT_Hits"            )
    kwargs.setdefault( "OutputRPCHits",             "RPC_Hits"            )
    kwargs.setdefault( "OutputTGCHits",             "TGC_Hits"            )

    kwargs.setdefault( "OutputCaloEntryLayer",      "CaloEntryLayer" )
    kwargs.setdefault( "OutputMuonEntryLayer",      "MuonEntryLayer" )
    kwargs.setdefault( "OutputMuonExitLayer",       "MuonExitLayer" )

    from ISF_Algorithms.ISF_AlgorithmsConf import ISF__RenameHitCollectionsAlg
    return ISF__RenameHitCollectionsAlg(name, **kwargs)
