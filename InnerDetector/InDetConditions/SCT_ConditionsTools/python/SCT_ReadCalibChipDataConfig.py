"""Define methods to configure ReadCalibChipDataTool

Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
"""
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from IOVDbSvc.IOVDbSvcConfig import addFoldersSplitOnline
from SCT_ConditionsTools.SCT_ConditionsToolsConf import SCT_ReadCalibChipDataTool
from SCT_ConditionsAlgorithms.SCT_ConditionsAlgorithmsConf import SCT_ReadCalibChipNoiseCondAlg
from SCT_ConditionsAlgorithms.SCT_ConditionsAlgorithmsConf import SCT_ReadCalibChipGainCondAlg

def SCT_ReadCalibChipDataToolCfg(flags, name="InDetSCT_ReadCalibChipDataTool", **kwargs):
    """Return a ReadCalibChipDataTool configured for SCT"""
    return SCT_ReadCalibChipDataTool(name, **kwargs)

def SCT_ReadCalibChipDataCfg(flags, name="SCT_ReadCalibChip", **kwargs):
    """Return configured ComponentAccumulator with SCT_ReadCalibChipDataCfg tool

    Accepts optional noiseFolder and gainFolder keyword arguments
    """
    acc = ComponentAccumulator()
    # folders
    noiseFolder = kwargs.get("noiseFolder", "/SCT/DAQ/Calibration/ChipNoise")
    acc.merge(addFoldersSplitOnline(flags, "SCT", noiseFolder, noiseFolder, "CondAttrListCollection"))
    gainFolder = kwargs.get("gainFolder", "/SCT/DAQ/Calibration/ChipGain")
    acc.merge(addFoldersSplitOnline(flags, "SCT", gainFolder, gainFolder, "CondAttrListCollection"))
    # Algorithms
    noiseAlg = SCT_ReadCalibChipNoiseCondAlg(name=name + "NoiseCondAlg", ReadKey=noiseFolder)
    acc.addCondAlgo(noiseAlg)
    gainAlg = SCT_ReadCalibChipGainCondAlg(name=name + "GainCondAlg", ReadKey=gainFolder)
    acc.addCondAlgo(gainAlg)
    tool = kwargs.get("ReadCalibChipDataTool", SCT_ReadCalibChipDataToolCfg(flags))
    acc.setPrivateTools(tool)
    return acc

