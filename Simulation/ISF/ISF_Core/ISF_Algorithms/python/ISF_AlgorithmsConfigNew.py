# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory

from ISF_HepMC_Tools.ISF_HepMC_ToolsConfigNew import ParticleSimWhiteList_ExtraParticlesCfg, ParticleFinalStateFilterCfg, GenParticleInteractingFilterCfg, EtaPhiFilterCfg, ParticlePositionFilterDynamicCfg
AthSequencer=CompFactory.AthSequencer

def SimEventFilterCfg(flags, name="ISF_SimEventFilter", sequenceName='SimSequence', **kwargs):
    result = ComponentAccumulator()
    result.addSequence(AthSequencer(sequenceName), parentName='AthAlgSeq')
    kwargs.setdefault( "InputHardScatterCollection", "BeamTruthEvent")
    kwargs.setdefault( "GenParticleCommonFilters", [result.popToolsAndMerge(ParticlePositionFilterDynamicCfg(flags)),
                                                    result.popToolsAndMerge(EtaPhiFilterCfg(flags)),
                                                    result.popToolsAndMerge(GenParticleInteractingFilterCfg(flags))] )
    kwargs.setdefault( "GenParticleOldFilters", [result.popToolsAndMerge(ParticleFinalStateFilterCfg(flags))] )
    kwargs.setdefault( "GenParticleNewFilters", [result.popToolsAndMerge(ParticleSimWhiteList_ExtraParticlesCfg(flags))] )

    result.addEventAlgo(CompFactory.ISF.SimEventFilter(name, **kwargs), sequenceName) # TODO Need to ensure that sequence has been created
    return result

def InvertedSimEventFilterCfg(flags, name="ISF_InvertedSimEventFilter", sequenceName='CopyHitSequence', **kwargs):
    kwargs.setdefault("FilterKey", "ISF_InvertedSimEventFilter")
    kwargs.setdefault("InvertFilter", True)
    return SimEventFilterCfg(flags, name, sequenceName, **kwargs)

def RenameHitCollectionsCfg(flags, name="RenameHitCollections", sequenceName='CopyHitSequence', **kwargs):
    result = ComponentAccumulator()
    result.addSequence(AthSequencer(sequenceName), parentName='AthAlgSeq')
    kwargs.setdefault( "InputMcEventCollection",    "TruthEventOLD" )
    kwargs.setdefault( "OutputMcEventCollection",   "TruthEvent"          )
    kwargs.setdefault( "InputCaloEntryLayer",       "CaloEntryLayerOLD" )
    kwargs.setdefault( "InputMuonEntryLayer",       "MuonEntryLayerOLD" )
    kwargs.setdefault( "InputMuonExitLayer",        "MuonExitLayerOLD" )
    kwargs.setdefault( "OutputCaloEntryLayer",      "CaloEntryLayer" )
    kwargs.setdefault( "OutputMuonEntryLayer",      "MuonEntryLayer" )
    kwargs.setdefault( "OutputMuonExitLayer",       "MuonExitLayer" )

    if flags.Detector.EnableBCM:
        kwargs.setdefault( "InputBCMHits",              "BCMHitsOLD" )
        kwargs.setdefault( "InputBLMHits",              "BLMHitsOLD" )
        kwargs.setdefault( "OutputBCMHits",             "BCMHits"             )
        kwargs.setdefault( "OutputBLMHits",             "BLMHits"             )
    if flags.Detector.EnablePixel:
        kwargs.setdefault( "InputPixelHits",            "PixelHitsOLD" )
        kwargs.setdefault( "OutputPixelHits",           "PixelHits"           )
    if flags.Detector.EnableSCT:
        kwargs.setdefault( "InputSCTHits",              "SCT_HitsOLD" )
        kwargs.setdefault( "OutputSCTHits",             "SCT_Hits"            )
    if flags.Detector.EnableTRT:
        kwargs.setdefault( "InputTRTUncompressedHits",  "TRTUncompressedHitsOLD" )
        kwargs.setdefault( "OutputTRTUncompressedHits", "TRTUncompressedHits" )
    if flags.Detector.EnableITkPixel:
        kwargs.setdefault( "InputITkPixelHits",            "ITkPixelHitsOLD" )
        kwargs.setdefault( "OutputITkPixelHits",          "ITkPixelHits"           )
    if flags.Detector.EnableITkStrip:
        kwargs.setdefault( "InputITkStripHits",              "ITkStripHitsOLD" )
        kwargs.setdefault( "OutputITkStripHits",             "ITkStripHits"            )

    if flags.Detector.EnableLAr:
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

    if flags.Detector.EnableTile:
        kwargs.setdefault( "InputTileHits",             "TileHitVecOLD" )
        kwargs.setdefault( "OutputTileHits",            "TileHitVec"          )
        #TODO: proper treatment of calibration hits
        #kwargs.setdefault( "InputTileActiveCalibHits", "TileCalibHitActiveCellOLD")
        #kwargs.setdefault( "InputTileInactiveCalibHits", "TileCalibHitInactiveCellOLD")
        #kwargs.setdefault( "InputTileDeadCalibHits", "TileCalibHitDeadMaterialOLD")
        kwargs.setdefault( "OutputTileActiveCalibHits", "TileCalibHitActiveCell")
        kwargs.setdefault( "OutputTileInactiveCalibHits", "TileCalibHitInactiveCell")
        kwargs.setdefault( "OutputTileDeadCalibHits", "TileCalibHitDeadMaterial")
    if flags.Detector.EnableMBTS:
        kwargs.setdefault( "InputMBTSHits",             "MBTSHitsOLD" )
        kwargs.setdefault( "OutputMBTSHits",          "MBTSHits"        )
    if flags.Detector.EnableHGTD:
        kwargs.setdefault( "InputHGTDHits",             "HGTD_HitsOLD" )
        kwargs.setdefault( "OutputHGTDHits",          "HGTD_Hits"        )

    if flags.Detector.EnableCSC:
        kwargs.setdefault( "InputCSCHits",              "CSC_HitsOLD" )
        kwargs.setdefault( "OutputCSCHits",             "CSC_Hits"            )
    if flags.Detector.EnableMDT:
        kwargs.setdefault( "InputMDTHits",              "MDT_HitsOLD" )
        kwargs.setdefault( "OutputMDTHits",             "MDT_Hits"            )
    if flags.Detector.EnableRPC:
        kwargs.setdefault( "InputRPCHits",              "RPC_HitsOLD" )
        kwargs.setdefault( "OutputRPCHits",             "RPC_Hits"            )
    if flags.Detector.EnableTGC:
        kwargs.setdefault( "InputTGCHits",              "TGC_HitsOLD" )
        kwargs.setdefault( "OutputTGCHits",             "TGC_Hits"            )
    if flags.Detector.EnableMM:
        kwargs.setdefault( "InputMMHits",              "MicromegasSensitiveDetectorOLD" )
        kwargs.setdefault( "OutputMMHits",              "MicromegasSensitiveDetector"  )
    if flags.Detector.EnablesTGC:
        kwargs.setdefault( "InputsTGCHits",              "sTGCSensitiveDetectorOLD" )
        kwargs.setdefault( "OutputsTGCHits",           "sTGCSensitiveDetector"           )

    result.addEventAlgo(CompFactory.ISF.RenameHitCollectionsAlg(name, **kwargs), sequenceName) # TODO Need to ensure that sequence has been created
    return result
