"""Define methods to configure SCT_Sensors

Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
"""
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory
from IOVDbSvc.IOVDbSvcConfig import addFolders

def SCT_SensorsCfg(flags, name="SCT_Sensors", **kwargs):
    """Configure necessary condition folder, condition algorithm
       for SCT_MajorityConditionsTool and return it.
    """
    acc = ComponentAccumulator()

    # Condition folder
    folder = "/SCT/Sensors"
    dbInstance = "SCT_OFL"

    if "folderTag" in kwargs:
        acc.merge(addFolders(flags, folder, dbInstance, className="CondAttrListCollection", tag=kwargs["folderTag"]))
    else:
        acc.merge(addFolders(flags, folder, dbInstance, className="CondAttrListCollection"))
    
    # Condition algorithm
    acc.addCondAlgo(CompFactory.SCT_SensorsCondAlg())

    # Condition tool
    acc.setPrivateTools(CompFactory.SCT_SensorsTool(name=name+"Tool"))

    return acc
