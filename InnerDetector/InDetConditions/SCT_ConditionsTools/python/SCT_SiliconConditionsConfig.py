"""Define methods to configure SiPropertiesTool

Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
"""
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory
from AtlasGeoModel.GeoModelConfig import GeoModelCfg
from SCT_ConditionsTools.SCT_DCSConditionsConfig import SCT_DCSConditionsCfg
SCT_SiliconHVCondAlg=CompFactory.SCT_SiliconHVCondAlg
SCT_SiliconTempCondAlg=CompFactory.SCT_SiliconTempCondAlg
SCT_SiliconConditionsTool=CompFactory.SCT_SiliconConditionsTool

def SCT_SiliconConditionsCfg(flags, name="SCT_Silicon", **kwargs):
    """Return a ComponentAccumulator configured for SiliconConditions DB

    DCSConditionsTool may be provided in kwargs
    """
    acc = ComponentAccumulator()

    # Condition algorithms
    algkwargs = {}
    DCSConditionsTool = kwargs.get("DCSConditionsTool")
    if DCSConditionsTool:
        algkwargs["UseState"] = DCSConditionsTool.ReadAllDBFolders
        algkwargs["DCSConditionsTool"] = DCSConditionsTool
    else:
        algkwargs["UseState"] = True
        algkwargs["DCSConditionsTool"] = acc.popToolsAndMerge(SCT_DCSConditionsCfg(flags))
    # For SCT_ID used in SCT_SiliconHVCondAlg,
    # SCT_SiliconTempCondAlg and SCT_SiliconConditionsTool
    acc.merge(GeoModelCfg(flags))
    acc.addCondAlgo(SCT_SiliconHVCondAlg(name=name + "HVCondAlg", **algkwargs))
    acc.addCondAlgo(SCT_SiliconTempCondAlg(name=name + "TempCondAlg", **algkwargs))

    # Condition tool
    toolkwargs = {}
    toolkwargs["UseDB"] = kwargs.get("UseDB", True)
    toolkwargs["ForceUseGeoModel"] = kwargs.get("ForceUseGeoModel", False)
    acc.setPrivateTools(SCT_SiliconConditionsTool(name="SCT_SiliconConditionsTool", **toolkwargs))

    return acc
