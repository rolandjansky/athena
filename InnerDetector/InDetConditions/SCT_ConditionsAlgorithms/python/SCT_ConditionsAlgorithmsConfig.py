# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory
from IOVDbSvc.IOVDbSvcConfig import addFoldersSplitOnline


def SCT_AlignCondAlgCfg(flags, name="SCT_AlignCondAlg", **kwargs):
    """Return a configured SCT_AlignCondAlg"""
    from SCT_GeoModel.SCT_GeoModelConfig import SCT_GeoModelCfg
    acc = SCT_GeoModelCfg(flags)
    if flags.GeoModel.Align.Dynamic:
        acc.merge(addFoldersSplitOnline(flags, "INDET", "/Indet/Onl/AlignL1/ID", "/Indet/AlignL1/ID", className="CondAttrListCollection"))
        acc.merge(addFoldersSplitOnline(flags, "INDET", "/Indet/Onl/AlignL2/SCT", "/Indet/AlignL2/SCT", className="CondAttrListCollection"))
        acc.merge(addFoldersSplitOnline(flags, "INDET", "/Indet/Onl/AlignL3", "/Indet/AlignL3", className="AlignableTransformContainer"))
    else:
        acc.merge(addFoldersSplitOnline(flags, "INDET", "/Indet/Onl/Align", "/Indet/Align", className="AlignableTransformContainer"))

    kwargs.setdefault("UseDynamicAlignFolders", flags.GeoModel.Align.Dynamic)

    acc.addCondAlgo(CompFactory.SCT_AlignCondAlg(name, **kwargs))
    return acc


def getSCTDAQConfigFolder(flags) :
    if flags.InDet.ForceCoolVectorPayload and flags.InDet.ForceCoraCool:
        raise Exception("SCT DB CONFIGURATION FLAG CONFLICT: Both CVP and CoraCool selected")  
    return "/SCT/DAQ/Config/" if (flags.InDet.ForceCoolVectorPayload
                                  or (flags.IOVDb.DatabaseInstance != "COMP200"
                                  and not flags.InDet.ForceCoraCool)) else "/SCT/DAQ/Configuration/"


def SCT_ConfigurationCondAlgCfg(flags, name="SCT_ConfigurationCondAlg", **kwargs):
    acc = ComponentAccumulator()
    config_folder_prefix = getSCTDAQConfigFolder(flags)
    channelFolder = config_folder_prefix + ("Chip" if flags.IOVDb.DatabaseInstance == "COMP200" else "ChipSlim")
    kwargs.setdefault("ReadKeyChannel", channelFolder)
    kwargs.setdefault("ReadKeyModule", f"{config_folder_prefix}Module")
    kwargs.setdefault("ReadKeyMur", f"{config_folder_prefix}MUR")

    acc.merge(addFoldersSplitOnline(flags,
                                    detDb="SCT",
                                    online_folders=channelFolder,
                                    offline_folders=channelFolder,
                                    className="CondAttrListVec",
                                    splitMC=True))
    acc.merge(addFoldersSplitOnline(flags,
                                    detDb="SCT",
                                    online_folders=f"{config_folder_prefix}Module",
                                    offline_folders=f"{config_folder_prefix}Module",
                                    className="CondAttrListVec",
                                    splitMC=True))
    acc.merge(addFoldersSplitOnline(flags,
                                    detDb="SCT",
                                    online_folders=f"{config_folder_prefix}MUR",
                                    offline_folders=f"{config_folder_prefix}MUR",
                                    className="CondAttrListVec",
                                    splitMC=True))

    from SCT_Cabling.SCT_CablingConfig import SCT_CablingToolCfg
    kwargs.setdefault("SCT_CablingTool", acc.popToolsAndMerge(SCT_CablingToolCfg(flags)))

    from SCT_ConditionsTools.SCT_ConditionsToolsConfig import SCT_ReadoutToolCfg
    kwargs.setdefault("SCT_ReadoutTool", acc.popToolsAndMerge(SCT_ReadoutToolCfg(flags)))

    acc.addCondAlgo(CompFactory.SCT_ConfigurationCondAlg(name, **kwargs))
    return acc



def SCT_DetectorElementCondAlgCfg(flags, name="SCT_DetectorElementCondAlg", **kwargs):
    from SCT_ConditionsAlgorithms.SCT_ConditionsAlgorithmsConfig import SCT_AlignCondAlgCfg
    acc = SCT_AlignCondAlgCfg(flags)

    # FIXME
    # add artifical dependencies to SCT, TRT and Muon
    # conditions algs to ensure that the IOV
    # is identical to the IOV of the tracking geometry
    if flags.Detector.GeometryMuon and flags.Muon.enableAlignment:
        from MuonConfig.MuonGeometryConfig import MuonDetectorCondAlgCfg
        acc.merge(MuonDetectorCondAlgCfg(flags))
        kwargs.setdefault("MuonManagerKey", "MuonDetectorManager")
    if flags.Detector.GeometryTRT:
        from TRT_GeoModel.TRT_GeoModelConfig import TRT_ReadoutGeometryCfg
        acc.merge(TRT_ReadoutGeometryCfg(flags))
        kwargs.setdefault("TRT_DetEltContKey", "TRT_DetElementContainer")
    if not flags.GeoModel.Align.LegacyConditionsAccess and flags.Detector.GeometryPixel:
        from PixelGeoModel.PixelGeoModelConfig import PixelAlignmentCfg
        acc.merge(PixelAlignmentCfg(flags))
        kwargs.setdefault("PixelAlignmentStore", "PixelAlignmentStore")
    # end of hack

    acc.addCondAlgo(CompFactory.SCT_DetectorElementCondAlg(name, **kwargs))
    return acc


if __name__=="__main__":
    from AthenaCommon.Logging import log
    from AthenaCommon.Constants import INFO
    log.setLevel(INFO)

    algorithm = ""

    from AthenaCommon.Configurable import Configurable
    Configurable.configurableRun3Behavior = 1
    
    from AthenaConfiguration.AllConfigFlags import ConfigFlags
    ConfigFlags.Input.isMC = False
    ConfigFlags.Input.Files = ["./myESD.pool.root"]
    ConfigFlags.Input.ProjectName = "data17_13TeV" # q431 input
    ConfigFlags.Input.RunNumber = 330470 # q431 input
    ConfigFlags.IOVDb.GlobalTag = "CONDBR2-BLKPA-2018-03" # q431 setup
    ConfigFlags.GeoModel.AtlasVersion = "ATLAS-R2-2016-01-00-01" # q431 setup
    ConfigFlags.lock()

    from AthenaConfiguration.MainServicesConfig import MainServicesCfg
    cfg = MainServicesCfg(ConfigFlags)

    from AthenaPoolCnvSvc.PoolReadConfig import PoolReadCfg
    cfg.merge(PoolReadCfg(ConfigFlags))

    algkwargs = {}
    algkwargs["OutputLevel"] = INFO
    if algorithm == "AlignCondAlg":
        cfg.merge(SCT_AlignCondAlgCfg(ConfigFlags, **algkwargs))
    elif algorithm == "ConfigurationCondAlg":
        cfg.merge(SCT_ConfigurationCondAlgCfg(ConfigFlags, **algkwargs))
    elif algorithm == "DetectorElementCondAlg":
        cfg.merge(SCT_DetectorElementCondAlgCfg(ConfigFlags, **algkwargs))

    cfg.run(maxEvents=25)
