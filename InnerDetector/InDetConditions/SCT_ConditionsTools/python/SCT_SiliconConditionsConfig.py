"""Define methods to configure SiPropertiesTool

Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
"""
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory
from AtlasGeoModel.GeoModelConfig import GeoModelCfg
SCT_SiliconHVCondAlg=CompFactory.SCT_SiliconHVCondAlg
SCT_SiliconTempCondAlg=CompFactory.SCT_SiliconTempCondAlg
SCT_SiliconConditionsTool=CompFactory.SCT_SiliconConditionsTool

def SCT_SiliconConditionsToolCfg(flags, name="SCT_SiliconConditionsTool", **kwargs):
    """Return a configured SCT_SiliconConditionsTool (a.k.a. SiConditionsTool)"""
    kwargs.setdefault("UseDB", True)
    kwargs.setdefault("ForceUseGeoModel", False)
    return SCT_SiliconConditionsTool(name, **kwargs)

def SCT_SiliconConditionsCfg(flags, name="SCT_Silicon", **kwargs):
    """Return a ComponentAccumulator configured for SiliconConditions DB

    DCSConditionsTool may be provided in kwargs
    """
    acc = ComponentAccumulator()
    acc.merge(GeoModelCfg(flags)) # For SCT_ID used in SCT_SiliconConditionsTool
    CondArgs = {}
    DCSConditionsTool = kwargs.get("DCSConditionsTool")
    if DCSConditionsTool:
        CondArgs["UseState"] = DCSConditionsTool.ReadAllDBFolders
        CondArgs["DCSConditionsTool"] = DCSConditionsTool
    acc.addCondAlgo(SCT_SiliconHVCondAlg(name=name + "HVCondAlg", **CondArgs))
    acc.addCondAlgo(SCT_SiliconTempCondAlg(name=name + "TempCondAlg", **CondArgs))
    return acc

