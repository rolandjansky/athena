"""Define methods to configure SCT_LinkMaskingTool

Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
"""
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory
from AtlasGeoModel.GeoModelConfig import GeoModelCfg
from IOVDbSvc.IOVDbSvcConfig import addFolders

def SCT_LinkMaskingCfg(flags, name="SCT_LinkMasking", **kwargs):
    """Configure necessary condition folder, condition algorithm
       for SCT_LinkMaskingTool and return it.
    """
    acc = ComponentAccumulator()

    # For SCT_ID used in SCT_LinkMaskingTool
    acc.merge(GeoModelCfg(flags))

    # Condition folders
    folder = "/purple/pants"
    # mycool.db is hard coded in Database/IOVDbSvc/python/IOVDbSvcConfig.py
    dbConnection = "<dbConnection>sqlite://;schema=mycool.db;dbname=CONDBR2</dbConnection>"
    acc.merge(addFolders(configFlags=flags, folderstrings=folder, className="CondAttrListCollection", db=dbConnection))

    # Condition algorithm
    kwargs.setdefault("ReadKey", folder)
    alg = CompFactory.SCT_LinkMaskingCondAlg(name=name+"CondAlg", **kwargs)
    acc.addCondAlgo(alg)

    # Condition tool
    acc.merge(GeoModelCfg(flags)) # For SCT_ID used in SCT_DCSConditionsTool
    acc.setPrivateTools(CompFactory.SCT_LinkMaskingTool(name=name+"Tool"))

    return acc
