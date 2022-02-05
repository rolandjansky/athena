
# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory
from IOVDbSvc.IOVDbSvcConfig import addFoldersSplitOnline
from StripGeoModelXml.ITkStripGeoModelConfig import ITkStripGeoModelCfg


def ITkStripAlignCondAlgCfg(flags, name="ITkStripAlignCondAlg", **kwargs):
    """Return a configured SCT_AlignCondAlg for ITk"""
    acc = ITkStripGeoModelCfg(flags)
    if flags.GeoModel.Align.Dynamic:
        raise RuntimeError("Dynamic alignment not supported for ITk yet")
    else:
        acc.merge(addFoldersSplitOnline(flags, "INDET", "/Indet/Onl/Align", "/Indet/Align", className="AlignableTransformContainer"))

    kwargs.setdefault("DetManagerName", "ITkStrip")
    kwargs.setdefault("WriteKey", "ITkStripAlignmentStore")
    kwargs.setdefault("UseDynamicAlignFolders", flags.GeoModel.Align.Dynamic)

    sctAlignCondAlg = CompFactory.SCT_AlignCondAlg(name, **kwargs)
    acc.addCondAlgo(sctAlignCondAlg)
    return acc


def getITkStripDAQConfigFolder(flags) :
    return "/ITkStrip/DAQ/Config/" if flags.IOVDb.DatabaseInstance != "COMP200" else "/ITkStrip/DAQ/Configuration/"


def ITkStripConfigurationCondAlgCfg(flags, name="ITkStripConfigurationCondAlg", **kwargs):
    acc = ComponentAccumulator()
    folder_prefix = getITkStripDAQConfigFolder(flags)
    channelFolder = folder_prefix + ("Chip" if flags.IOVDb.DatabaseInstance == "COMP200" else "ChipSlim")
    kwargs.setdefault("ReadKeyChannel", channelFolder)
    kwargs.setdefault("ReadKeyModule", f"{folder_prefix}Module")
    kwargs.setdefault("ReadKeyMur", f"{folder_prefix}MUR")

    acc.merge(addFoldersSplitOnline(flags,
                                    detDb="ITkStrip",
                                    online_folders=channelFolder,
                                    offline_folders=channelFolder,
                                    className="CondAttrListVec",
                                    splitMC=True))
    acc.merge(addFoldersSplitOnline(flags,
                                    detDb="ITkStrip",
                                    online_folders=f"{folder_prefix}Module",
                                    offline_folders=f"{folder_prefix}Module",
                                    className="CondAttrListVec",
                                    splitMC=True))
    acc.merge(addFoldersSplitOnline(flags,
                                    detDb="ITkStrip",
                                    online_folders=f"{folder_prefix}MUR",
                                    offline_folders=f"{folder_prefix}MUR",
                                    className="CondAttrListVec",
                                    splitMC=True))

    from SCT_Cabling.ITkStripCablingConfig import ITkStripCablingToolCfg
    kwargs.setdefault("SCT_CablingTool", acc.popToolsAndMerge(ITkStripCablingToolCfg(flags)))

    from SCT_ConditionsTools.ITkStripConditionsToolsConfig import ITkStripReadoutToolCfg
    kwargs.setdefault("SCT_ReadoutTool", acc.popToolsAndMerge(ITkStripReadoutToolCfg(flags)))

    acc.addCondAlgo(CompFactory.SCT_ConfigurationCondAlg(name, **kwargs))
    return acc


def ITkStripDetectorElementCondAlgCfg(flags, name="ITkStripDetectorElementCondAlg", **kwargs):
    kwargs.setdefault("DetManagerName", "ITkStrip")
    kwargs.setdefault("ReadKey", "ITkStripAlignmentStore")
    kwargs.setdefault("WriteKey", "ITkStripDetectorElementCollection")

    acc = ITkStripAlignCondAlgCfg(flags)
    acc.addCondAlgo(CompFactory.SCT_DetectorElementCondAlg(name, **kwargs))
    return acc
