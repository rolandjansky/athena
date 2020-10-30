"""Define methods to configure SCT SiProperties

Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
"""
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory
from SCT_ConditionsTools.SCT_SiliconConditionsConfig import SCT_SiliconConditionsCfg
from SCT_GeoModel.SCT_GeoModelConfig import SCT_GeometryCfg
SiPropertiesTool=CompFactory.SiPropertiesTool
SCTSiPropertiesCondAlg=CompFactory.SCTSiPropertiesCondAlg

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
    acc.merge(SCT_GeometryCfg(flags))
    alg = SCTSiPropertiesCondAlg(name, **algkwargs)
    acc.addCondAlgo(alg)

    # Condition tool
    toolkwargs = {}
    toolkwargs["DetectorName"] = "SCT"
    toolkwargs["ReadKey"] = "SCTSiliconPropertiesVector"
    acc.setPrivateTools(SiPropertiesTool(name=name, **toolkwargs))

    return acc
