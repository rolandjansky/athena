"""Define methods to configure ITkStrip SiProperties

Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
"""
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory
from SCT_ConditionsTools.ITkStripConditionsToolsConfig import ITkStripSiliconConditionsCfg
from StripGeoModelXml.ITkStripGeoModelConfig import ITkStripReadoutGeometryCfg


def ITkStripSiPropertiesCfg(flags, name="ITkStripSiPropertiesCondAlg", **kwargs):
    """Return configured ComponentAccumulator and tool for ITkStripSiProperties

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
        algkwargs["SiConditionsTool"] = acc.popToolsAndMerge(ITkStripSiliconConditionsCfg(flags, **kwargs))
    # For SCT_ID and SCT_DetectorElementCollection
    # used in SCTSiPropertiesCondAlg and SiPropertiesTool
    #Specify correct DetElCollection for ITkStrip
    algkwargs["SCTDetEleCollKey"] = "ITkStripDetectorElementCollection"
    acc.merge(ITkStripReadoutGeometryCfg(flags))
    acc.addCondAlgo(CompFactory.SCTSiPropertiesCondAlg(name, **algkwargs))

    # Condition tool
    toolkwargs = {}
    #Eventually update to ITkStrip - tool assumes Pixel or SCT for now
    toolkwargs["DetectorName"] = "SCT"
    toolkwargs["ReadKey"] = "SCTSiliconPropertiesVector"
    acc.setPrivateTools(CompFactory.SiPropertiesTool(name="ITkStripSiPropertiesTool", **toolkwargs))

    return acc
