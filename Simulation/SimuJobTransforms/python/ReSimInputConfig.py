# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator

def RenameHitCollectionsOnReadCfg(ConfigFlags):

    result = ComponentAccumulator()
    from SGComps.AddressRemappingConfig import InputRenameCfg
    result.merge(InputRenameCfg("McEventCollection","TruthEvent","TruthEventOLD"))
    result.merge(InputRenameCfg("RecoTimingObj","EVNTtoHITS_timings","EVNTtoHITS_timingsOLD"))

    if ConfigFlags.Detector.EnableID or  ConfigFlags.Detector.EnableITk:
        if ConfigFlags.Detector.EnableBCM:
            result.merge(InputRenameCfg("SiHitCollection","BCMHits","BCMHitsOLD"))
            result.merge(InputRenameCfg("SiHitCollection","BLMHits","BLMHitsOLD"))
        if ConfigFlags.Detector.EnablePixel:
            result.merge(InputRenameCfg("SiHitCollection","PixelHits","PixelHitsOLD"))
        if ConfigFlags.Detector.EnableSCT:
            result.merge(InputRenameCfg("SiHitCollection","SCT_Hits","SCT_HitsOLD"))
        if ConfigFlags.Detector.EnableTRT:
            result.merge(InputRenameCfg("TRTUncompressedHitCollection","TRTUncompressedHits","TRTUncompressedHitsOLD"))
        if ConfigFlags.Detector.EnableBCMPrime:
            pass #TODO
        if ConfigFlags.Detector.EnableITkPixel:
            result.merge(InputRenameCfg("SiHitCollection","ITkPixelHits","ITkPixelHitsOLD"))
        if ConfigFlags.Detector.EnableITkStrip:
            result.merge(InputRenameCfg("SiHitCollection","ITkStripHits","ITkStripHitsOLD"))
        if ConfigFlags.Detector.EnableHGTD:
            result.merge(InputRenameCfg("SiHitCollection","HGTD_Hits","HGTD_HitsOLD"))
        result.merge(InputRenameCfg("TrackRecordCollection","CaloEntryLayer","CaloEntryLayerOLD"))

    if ConfigFlags.Detector.EnableCalo:
        if ConfigFlags.Detector.EnableLAr:
            result.merge(InputRenameCfg("LArHitContainer","LArHitEMB","LArHitEMBOLD"))
            result.merge(InputRenameCfg("LArHitContainer","LArHitEMEC","LArHitEMECOLD"))
            result.merge(InputRenameCfg("LArHitContainer","LArHitFCAL","LArHitFCALOLD"))
            result.merge(InputRenameCfg("LArHitContainer","LArHitHEC","LArHitHECOLD"))
            result.merge(InputRenameCfg("LArHitContainer","LArHitMiniFCAL","LArHitMiniFCALOLD"))
            result.merge(InputRenameCfg("CaloCalibrationHitContainer","LArCalibrationHitActive","LArCalibrationHitActiveOLD"))
            result.merge(InputRenameCfg("CaloCalibrationHitContainer","LArCalibrationHitDeadMaterial","LArCalibrationHitDeadMaterialOLD"))
            result.merge(InputRenameCfg("CaloCalibrationHitContainer","LArCalibrationHitInactive","LArCalibrationHitInactiveOLD"))
        if ConfigFlags.Detector.EnableTile:
            result.merge(InputRenameCfg("TileHitVector","TileHitVec","TileHitVecOLD"))
            result.merge(InputRenameCfg("CaloCalibrationHitContainer","TileCalibHitActiveCell","TileCalibHitActiveCellOLD"))
            result.merge(InputRenameCfg("CaloCalibrationHitContainer","TileCalibHitInactiveCell","TileCalibHitInactiveCellOLD"))
            result.merge(InputRenameCfg("CaloCalibrationHitContainer","TileCalibHitDeadMaterial","TileCalibHitDeadMaterialOLD"))
        if ConfigFlags.Detector.EnableMBTS:
            result.merge(InputRenameCfg("TileHitVector","MBTSHits","MBTSHitsOLD"))
        result.merge(InputRenameCfg("TrackRecordCollection","MuonEntryLayer","MuonEntryLayerOLD"))

    if ConfigFlags.Detector.EnableMuon:
        if ConfigFlags.Detector.EnableCSC:
            result.merge(InputRenameCfg("CSCSimHitCollection","CSC_Hits","CSC_HitsOLD"))
        if ConfigFlags.Detector.EnableMDT:
            result.merge(InputRenameCfg("MDTSimHitCollection","MDT_Hits","MDT_HitsOLD"))
        if ConfigFlags.Detector.EnableRPC:
            result.merge(InputRenameCfg("RPCSimHitCollection","RPC_Hits","RPC_HitsOLD"))
        if ConfigFlags.Detector.EnableTGC:
            result.merge(InputRenameCfg("TGCSimHitCollection","TGC_Hits","TGC_HitsOLD"))
        if ConfigFlags.Detector.EnablesTGC:
            result.merge(InputRenameCfg("sTGCSimHitCollection", "sTGCSensitiveDetector" "sTGCSensitiveDetectorOLD"))
        if ConfigFlags.Detector.EnableMM:
            result.merge(InputRenameCfg("MMSimHitCollection", "MicromegasSensitiveDetector", "MicromegasSensitiveDetectorOLD"))
        result.merge(InputRenameCfg("TrackRecordCollection","MuonExitLayer","MuonExitLayerOLD"))

    #FIXME Add Renaming for Fwd Detector sim hits

    return result
