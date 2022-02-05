# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory


def SCT_CablingFoldersCfg(flags):
    cfg = ComponentAccumulator()
    path = "/SCT/DAQ/Config/"
    if flags.IOVDb.DatabaseInstance == "COMP200":
        path = "/SCT/DAQ/Configuration/"
    instance = "SCT"
    if flags.Input.isMC:
        instance = "SCT_OFL"
    from IOVDbSvc.IOVDbSvcConfig import addFolders
    cfg.merge(addFolders(flags, [path+"ROD", path+"RODMUR", path+"MUR", path+"Geog"], instance, className="CondAttrListVec"))
    return cfg, path


def SCT_CablingCondAlgCfg(flags, name="SCT_CablingCondAlgFromCoraCool"):
    cfg = ComponentAccumulator()
    foldersCfg, path = SCT_CablingFoldersCfg(flags)
    cfg.merge(foldersCfg)
    SCT_CablingCondAlgFromCoraCool = CompFactory.SCT_CablingCondAlgFromCoraCool
    cfg.addCondAlgo(SCT_CablingCondAlgFromCoraCool(name,
                                                   ReadKeyRod=path+"ROD",
                                                   ReadKeyRodMur=path+"RODMUR",
                                                   ReadKeyMur=path+"MUR",
                                                   ReadKeyGeo=path+"Geog"))
    return cfg


def SCT_CablingToolCfg(flags, name="SCT_CablingTool"):
    cfg = ComponentAccumulator()

    # For SCT_ID used in SCT_CablingTool
    from AtlasGeoModel.GeoModelConfig import GeoModelCfg
    cfg.merge(GeoModelCfg(flags))

    cfg.merge(SCT_CablingCondAlgCfg(flags))

    cfg.setPrivateTools(CompFactory.SCT_CablingTool(name))
    return cfg
