"""Define methods to configure SCT_RODVeto

Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
"""
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory
from AtlasGeoModel.GeoModelConfig import GeoModelCfg
from InDetConfig.InDetRecToolConfig import SCT_CablingToolCfg

def SCT_RODVetoCfg(flags, name="SCT_RODVeto", **kwargs):
    """Configure necessary condition folders, condition algorithms 
       for SCT_DCSConditionsTool and return it.
    """
    acc = ComponentAccumulator()

    # For SCT_ID used in SCT_RODVetoCondAlg and SCT_RODVetoTool
    acc.merge(GeoModelCfg(flags))

    # For SCT_CablingTool used in SCT_RODVetoCondAlg
    kwargs.setdefault("SCT_CablingTool", acc.popToolsAndMerge(SCT_CablingToolCfg(flags)))

    # Condition algorithm (actually event algorithm)
    alg = CompFactory.SCT_RODVetoCondAlg(name=name+"CondAlg", **kwargs)
    acc.addEventAlgo(alg)

    # Condition tool
    acc.setPrivateTools(CompFactory.SCT_RODVetoTool(name=name+"Tool"))

    return acc
