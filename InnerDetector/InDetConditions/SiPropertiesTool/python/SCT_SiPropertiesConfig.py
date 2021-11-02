"""Define methods to configure SCT SiProperties

Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
"""
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory
from SCT_ConditionsTools.SCT_ConditionsToolsConfig import SCT_SiliconConditionsCfg
from SCT_GeoModel.SCT_GeoModelConfig import SCT_ReadoutGeometryCfg


def SCT_SiPropertiesCfg(flags, name="SCTSiPropertiesCondAlg", **kwargs):
    """Return configured ComponentAccumulator and tool for SCT_SiProperties

    SiConditionsTool and/or DCSConditionsTool may be provided in kwargs
    """
    acc = ComponentAccumulator()

    # Condition algorithm
    # SCTSiPropertiesCondAlg needs outputs of SCT_SiliconConditions algorithms
    algkwargs = {}
    SiConditionsTool = kwargs.get("SiConditionsTool")
    if SiConditionsTool:
        algkwargs["SiConditionsTool"] = SiConditionsTool
    else:
        algkwargs["SiConditionsTool"] = acc.popToolsAndMerge(SCT_SiliconConditionsCfg(flags, **kwargs))
    # For SCT_ID and SCT_DetectorElementCollection
    # used in SCTSiPropertiesCondAlg and SiPropertiesTool
    acc.merge(SCT_ReadoutGeometryCfg(flags))
    acc.addCondAlgo(CompFactory.SCTSiPropertiesCondAlg(name, **algkwargs))

    # Condition tool
    toolkwargs = {}
    toolkwargs["DetectorName"] = "SCT"
    toolkwargs["ReadKey"] = "SCTSiliconPropertiesVector"
    acc.setPrivateTools(CompFactory.SiPropertiesTool(name="SCT_SiPropertiesTool", **toolkwargs))

    return acc
