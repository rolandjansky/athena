# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory


def ITkStripCablingFoldersCfg(flags):
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


def ITkStripCablingCondAlgCfg(flags, name="ITkStripCablingCondAlg"):
    cfg = ComponentAccumulator()
    foldersCfg, path = ITkStripCablingFoldersCfg(flags)
    cfg.merge(foldersCfg)
    SCT_CablingCondAlgFromCoraCool = CompFactory.SCT_CablingCondAlgFromCoraCool
    cfg.addCondAlgo(SCT_CablingCondAlgFromCoraCool(name,
                                                   ReadKeyRod=path+"ROD",
                                                   ReadKeyRodMur=path+"RODMUR",
                                                   ReadKeyMur=path+"MUR",
                                                   ReadKeyGeo=path+"Geog"))
    return cfg


def ITkStripCablingToolCfg(flags, name="ITkStripCablingTool"):
    cfg = ComponentAccumulator()

    # For SCT_ID used in SCT_CablingTool
    from AtlasGeoModel.GeoModelConfig import GeoModelCfg
    cfg.merge(GeoModelCfg(flags))

    cfg.merge(ITkStripCablingCondAlgCfg(flags))

    cfg.setPrivateTools(CompFactory.SCT_CablingTool(name))
    return cfg
