# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

from AthenaCommon import CfgMgr

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
    from AthenaCommon.DetFlags import DetFlags
    from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
    hardscatterSG = ""
    if athenaCommonFlags.DoFullChain() and DetFlags.pileup.any_on():
        hardscatterSG = "OriginalEvent_SG+"
    kwargs.setdefault( "OutputBCMHits",             hardscatterSG+"BCMHits"             )
    kwargs.setdefault( "OutputBLMHits",             hardscatterSG+"BLMHits"             )
    kwargs.setdefault( "OutputPixelHits",           hardscatterSG+"PixelHits"           )
    kwargs.setdefault( "OutputSCTHits",             hardscatterSG+"SCT_Hits"            )
    kwargs.setdefault( "OutputTRTUncompressedHits", hardscatterSG+"TRTUncompressedHits" )

    kwargs.setdefault( "OutputLArEMBHits",          hardscatterSG+"LArHitEMB"           )
    kwargs.setdefault( "OutputLArEMECHits",         hardscatterSG+"LArHitEMEC"          )
    kwargs.setdefault( "OutputLArFCALHits",         hardscatterSG+"LArHitFCAL"          )
    kwargs.setdefault( "OutputLArHECHits",          hardscatterSG+"LArHitHEC"           )

    kwargs.setdefault( "OutputTileHits",            hardscatterSG+"TileHitVec"          )
    kwargs.setdefault( "OutputMBTSHits",            hardscatterSG+"MBTSHits"            )

    kwargs.setdefault( "OutputCSCHits",             hardscatterSG+"CSC_Hits"            )
    kwargs.setdefault( "OutputMDTHits",             hardscatterSG+"MDT_Hits"            )
    kwargs.setdefault( "OutputRPCHits",             hardscatterSG+"RPC_Hits"            )
    kwargs.setdefault( "OutputTGCHits",             hardscatterSG+"TGC_Hits"            )
    return CfgMgr.ISF__CollectionMerger(name, **kwargs)

def getSimHitTreeCreator(name="ISF_SimHitTreeCreator", **kwargs):
    return CfgMgr.ISF__SimHitTreeCreator(name, **kwargs)

def getSimEventFilter(name="ISF_SimEventFilter", **kwargs):
    kwargs.setdefault( "InputHardScatterCollection", "BeamTruthEvent")
    kwargs.setdefault( "GenParticleCommonFilters", ['ISF_ParticlePositionFilterDynamic','ISF_EtaPhiFilter','ISF_GenParticleInteractingFilter'] )
    kwargs.setdefault( "GenParticleOldFilters", ['ISF_ParticleFinalStateFilter'] )
    kwargs.setdefault( "GenParticleNewFilters", ['ISF_ParticleSimWhiteList_ExtraParticles'] )
    return CfgMgr.ISF__SimEventFilter(name, **kwargs)

def getInvertedSimEventFilter(name="ISF_InvertedSimEventFilter", **kwargs):
    kwargs.setdefault("FilterKey", "ISF_InvertedSimEventFilter")
    kwargs.setdefault("InvertFilter", True)
    return getSimEventFilter(name, **kwargs)

def getRenameHitCollections(name="RenameHitCollections", **kwargs):
    kwargs.setdefault( "InputMcEventCollection",    "TruthEventOLD" )
    kwargs.setdefault( "OutputMcEventCollection",   "TruthEvent"          )
    kwargs.setdefault( "InputCaloEntryLayer",       "CaloEntryLayerOLD" )
    kwargs.setdefault( "InputMuonEntryLayer",       "MuonEntryLayerOLD" )
    kwargs.setdefault( "InputMuonExitLayer",        "MuonExitLayerOLD" )
    kwargs.setdefault( "OutputCaloEntryLayer",      "CaloEntryLayer" )
    kwargs.setdefault( "OutputMuonEntryLayer",      "MuonEntryLayer" )
    kwargs.setdefault( "OutputMuonExitLayer",       "MuonExitLayer" )

    from AthenaCommon.DetFlags import DetFlags
    if DetFlags.simulate.BCM_on():
        kwargs.setdefault( "InputBCMHits",              "BCMHitsOLD" )
        kwargs.setdefault( "InputBLMHits",              "BLMHitsOLD" )
        kwargs.setdefault( "OutputBCMHits",             "BCMHits"             )
        kwargs.setdefault( "OutputBLMHits",             "BLMHits"             )
    if DetFlags.simulate.pixel_on():
        kwargs.setdefault( "InputPixelHits",            "PixelHitsOLD" )
        kwargs.setdefault( "OutputPixelHits",           "PixelHits"           )
    if DetFlags.simulate.SCT_on():
        kwargs.setdefault( "InputSCTHits",              "SCT_HitsOLD" )
        kwargs.setdefault( "OutputSCTHits",             "SCT_Hits"            )
    if DetFlags.simulate.TRT_on():
        kwargs.setdefault( "InputTRTUncompressedHits",  "TRTUncompressedHitsOLD" )
        kwargs.setdefault( "OutputTRTUncompressedHits", "TRTUncompressedHits" )

    if DetFlags.simulate.LAr_on():
        kwargs.setdefault( "InputLArEMBHits",           "LArHitEMBOLD" )
        kwargs.setdefault( "InputLArEMECHits",          "LArHitEMECOLD" )
        kwargs.setdefault( "InputLArFCALHits",          "LArHitFCALOLD" )
        kwargs.setdefault( "InputLArHECHits",           "LArHitHECOLD" )
        kwargs.setdefault( "OutputLArEMBHits",          "LArHitEMB"           )
        kwargs.setdefault( "OutputLArEMECHits",         "LArHitEMEC"          )
        kwargs.setdefault( "OutputLArFCALHits",         "LArHitFCAL"          )
        kwargs.setdefault( "OutputLArHECHits",          "LArHitHEC"           )
        #TODO: proper treatment of calibration hits
        #kwargs.setdefault( "InputLArActiveCalibHits", "LArCalibrationHitActive")
        #kwargs.setdefault( "InputLArInactiveCalibHits", "LArCalibrationHitInactive")
        #kwargs.setdefault( "InputLArDeadCalibHits", "LArCalibrationHitDeadMaterial")
        kwargs.setdefault( "OutputLArActiveCalibHits", "LArCalibrationHitActive")
        kwargs.setdefault( "OutputLArInactiveCalibHits", "LArCalibrationHitInactive")
        kwargs.setdefault( "OutputLArDeadCalibHits", "LArCalibrationHitDeadMaterial")

    if DetFlags.simulate.Tile_on():
        kwargs.setdefault( "InputTileHits",             "TileHitVecOLD" )
        kwargs.setdefault( "OutputTileHits",            "TileHitVec"          )
        #TODO: proper treatment of calibration hits
        #kwargs.setdefault( "InputTileActiveCalibHits", "TileCalibHitActiveCellOLD")
        #kwargs.setdefault( "InputTileInactiveCalibHits", "TileCalibHitInactiveCellOLD")
        #kwargs.setdefault( "InputTileDeadCalibHits", "TileCalibHitDeadMaterialOLD")
        kwargs.setdefault( "OutputTileActiveCalibHits", "TileCalibHitActiveCell")
        kwargs.setdefault( "OutputTileInactiveCalibHits", "TileCalibHitInactiveCell")
        kwargs.setdefault( "OutputTileDeadCalibHits", "TileCalibHitDeadMaterial")
        kwargs.setdefault( "InputMBTSHits",             "MBTSHitsOLD" )
        kwargs.setdefault( "OutputMBTSHits",            "MBTSHits"            )

    from AtlasGeoModel.MuonGMJobProperties import MuonGeometryFlags
    if MuonGeometryFlags.hasCSC() and DetFlags.simulate.CSC_on():
        kwargs.setdefault( "InputCSCHits",              "CSC_HitsOLD" )
        kwargs.setdefault( "OutputCSCHits",             "CSC_Hits"            )
    if DetFlags.simulate.MDT_on():
        kwargs.setdefault( "InputMDTHits",              "MDT_HitsOLD" )
        kwargs.setdefault( "OutputMDTHits",             "MDT_Hits"            )
    if DetFlags.simulate.RPC_on():
        kwargs.setdefault( "InputRPCHits",              "RPC_HitsOLD" )
        kwargs.setdefault( "OutputRPCHits",             "RPC_Hits"            )
    if DetFlags.simulate.TGC_on():
        kwargs.setdefault( "InputTGCHits",              "TGC_HitsOLD" )
        kwargs.setdefault( "OutputTGCHits",             "TGC_Hits"            )
    if MuonGeometryFlags.hasMM() and DetFlags.simulate.Micromegas_on():
        kwargs.setdefault( "InputMMHits",              "MicromegasSensitiveDetectorOLD" )
        kwargs.setdefault( "OutputMMHits",              "MicromegasSensitiveDetector"  )
    if MuonGeometryFlags.hasSTGC() and DetFlags.simulate.sTGC_on():
        kwargs.setdefault( "InputsTGCHits",              "sTGCSensitiveDetectorOLD" )
        kwargs.setdefault( "OutputsTGCHits",           "sTGCSensitiveDetector"           )

    return CfgMgr.ISF__RenameHitCollectionsAlg(name, **kwargs)
