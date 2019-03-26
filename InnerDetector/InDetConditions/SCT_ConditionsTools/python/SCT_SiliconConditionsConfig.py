"""Define methods to configure SiPropertiesTool

Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
"""
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from SCT_ConditionsAlgorithms.SCT_ConditionsAlgorithmsConf import SCT_SiliconHVCondAlg
from SCT_ConditionsAlgorithms.SCT_ConditionsAlgorithmsConf import SCT_SiliconTempCondAlg
from SCT_ConditionsTools.SCT_ConditionsToolsConf import SCT_SiliconConditionsTool

def SCT_SiliconConditionsToolCfg(flags, name="SCT_SiliconConditionsTool", **kwargs):
    """Return a configured SCT_SiliconConditionsTool (a.k.a. SiConditionsTool)"""
    kwargs.setdefault("UseDB", True)
    kwargs.setdefault("ForceUseGeoModel", False)
    return SCT_SiliconConditionsTool(name, **kwargs)

def SCT_SiliconConditionsCfg(flags, name="SCT_Silicon", **kwargs):
    """Return configured ComponentAccumulator and tool for SCT_SiliconConditions

    SiConditionsTool and/or DCSConditionsTool may be provided in kwargs
    """
    acc = ComponentAccumulator()
    tool = kwargs.get("SiConditionsTool", SCT_SiliconConditionsToolCfg(flags))
    if tool.UseDB:
        CondArgs = {}
        if "DCSConditionsTool" in kwargs:
            DCSConditionsTool = kwargs["DCSConditionsTool"]
            CondArgs["UseState"] = DCSConditionsTool.ReadAllDBFolders
            CondArgs["DCSConditionsTool"] = DCSConditionsTool
        hvAlg = SCT_SiliconHVCondAlg(name=name + "HVCondAlg", **CondArgs)
        tempAlg = SCT_SiliconTempCondAlg(name=name + "TempCondAlg", **CondArgs)
        acc.addCondAlgo(hvAlg)
        acc.addCondAlgo(tempAlg)
    acc.setPrivateTools(tool)
    return acc

