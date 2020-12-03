"""Define methods to configure SCT_ConditionsParameter

Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
"""
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory
from IOVDbSvc.IOVDbSvcConfig import addFolders
from AtlasGeoModel.GeoModelConfig import GeoModelCfg
from InDetConfig.InDetRecToolConfig import SCT_CablingToolCfg

def SCT_ConditionsParameterCfg(flags, name="SCT_ConditionsParameter", **kwargs):
    """Configure necessary condition folder, condition algorithm 
       for SCT_ConditionsParameterTool and return it.
    """
    acc = ComponentAccumulator()

    # For SCT_ID used in SCT_CablingTool
    acc.merge(GeoModelCfg(flags))

    # Condition folder
    folder = "/SCT/DAQ/Configuration/Chip"
    dbInstance = "SCT"
    acc.merge(addFolders(flags, folder, dbInstance, className="CondAttrListVec"))

    # Condition algorithm
    alg = CompFactory.SCT_ConditionsParameterCondAlg(name=name+"CondAlg",
                                                     ReadKey=folder,
                                                     SCT_CablingTool=acc.popToolsAndMerge(SCT_CablingToolCfg(flags)))
    acc.addCondAlgo(alg)

    # Condition tool
    acc.setPrivateTools(CompFactory.SCT_ConditionsParameterTool(name=name+"Tool"))

    return acc
