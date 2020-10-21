"""Define methods to configure ReadCalibChipDataTool

Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
"""
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory
from AtlasGeoModel.GeoModelConfig import GeoModelCfg
from IOVDbSvc.IOVDbSvcConfig import addFoldersSplitOnline
SCT_ReadCalibChipDataTool=CompFactory.SCT_ReadCalibChipDataTool
SCT_ReadCalibChipNoiseCondAlg=CompFactory.SCT_ReadCalibChipNoiseCondAlg
SCT_ReadCalibChipGainCondAlg=CompFactory.SCT_ReadCalibChipGainCondAlg

def SCT_ReadCalibChipDataCfg(flags, name="SCT_ReadCalibChip", **kwargs):
    """Return configured ComponentAccumulator with SCT_ReadCalibChipDataCfg tool

    Accepts optional noiseFolder and gainFolder keyword arguments
    """
    acc = ComponentAccumulator()

    # Folders
    noiseFolder = kwargs.get("noiseFolder", "/SCT/DAQ/Calibration/ChipNoise")
    gainFolder = kwargs.get("gainFolder", "/SCT/DAQ/Calibration/ChipGain")
    if flags.Overlay.DataOverlay:
        forceDb="OFLP200"
        noiseTag="SctDaqCalibrationChipNoise-Apr10-01"
        gainTag="SctDaqCalibrationChipGain-Apr10-01"
    else:
        forceDb=None
        noiseTag=None
        gainTag=None

    acc.merge(addFoldersSplitOnline(flags, "SCT", noiseFolder, noiseFolder, "CondAttrListCollection",
                                    forceDb=forceDb, tag=noiseTag))
    acc.merge(addFoldersSplitOnline(flags, "SCT", gainFolder, gainFolder, "CondAttrListCollection",
                                    forceDb=forceDb, tag=gainTag))

    # Algorithms
    acc.merge(GeoModelCfg(flags)) # For SCT_ID used in SCT_ReadCalibChipNoiseCondAlg, SCT_ReadCalibChipGainCondAlg, SCT_ReadCalibChipDataTool
    noiseAlg = SCT_ReadCalibChipNoiseCondAlg(name=name + "NoiseCondAlg", ReadKey=noiseFolder)
    acc.addCondAlgo(noiseAlg)
    gainAlg = SCT_ReadCalibChipGainCondAlg(name=name + "GainCondAlg", ReadKey=gainFolder)
    acc.addCondAlgo(gainAlg)

    # Tool
    acc.setPrivateTools(SCT_ReadCalibChipDataTool(name="InDetSCT_ReadCalibChipDataTool"))

    return acc
