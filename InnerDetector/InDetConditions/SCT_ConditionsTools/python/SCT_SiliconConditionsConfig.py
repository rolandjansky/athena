"""Define methods to configure SiPropertiesTool

Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
"""
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory
from AtlasGeoModel.GeoModelConfig import GeoModelCfg
from SCT_ConditionsTools.SCT_DCSConditionsConfig import SCT_DCSConditionsCfg, ITkStripDCSConditionsCfg
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
        algkwargs["UseState"] = not flags.Common.isOnline
        algkwargs["DCSConditionsTool"] = acc.popToolsAndMerge(SCT_DCSConditionsCfg(flags))
    # For SCT_ID used in SCT_SiliconHVCondAlg,
    # SCT_SiliconTempCondAlg and SCT_SiliconConditionsTool
    acc.merge(GeoModelCfg(flags))
    acc.addCondAlgo(SCT_SiliconHVCondAlg(name=f"{name}HVCondAlg", **algkwargs))
    acc.addCondAlgo(SCT_SiliconTempCondAlg(name=f"{name}TempCondAlg", **algkwargs))

    # Condition tool
    toolkwargs = {}
    toolkwargs["UseDB"] = kwargs.get("UseDB", True)
    toolkwargs["ForceUseGeoModel"] = kwargs.get("ForceUseGeoModel", False)
    acc.setPrivateTools(SCT_SiliconConditionsTool(name=f"{name}ConditionsTool", **toolkwargs))

    return acc


def ITkStripSiliconConditionsCfg(flags, name="ITkStripSilicon", **kwargs):
    """Return a ComponentAccumulator configured for ITk Strip SiliconConditions"""
    acc = ComponentAccumulator()
    kwargs["DCSConditionsTool"] = acc.popToolsAndMerge(ITkStripDCSConditionsCfg(flags))
    tool = acc.popToolsAndMerge(SCT_SiliconConditionsCfg(flags, name, **kwargs))
    acc.setPrivateTools(tool)
    return acc
