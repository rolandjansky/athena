"""Define methods to configure SCT_MajorityConditions

Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
"""
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory
from IOVDbSvc.IOVDbSvcConfig import addFolders

def SCT_MajorityConditionsCfg(flags, name="InDetSCT_MajorityConditions", **kwargs):
    """Configure necessary condition folder, condition algorithm
       for SCT_MajorityConditionsTool and return it.
    """
    acc = ComponentAccumulator()

    # Condition folder
    folder = "/SCT/DCS/MAJ"
    dbInstance = "DCS_OFL"
    
    # Condition algorithm
    acc.merge(addFolders(flags, folder, dbInstance, className="CondAttrListCollection"))
    acc.addCondAlgo(CompFactory.SCT_MajorityCondAlg())

    # Condition tool
    acc.setPrivateTools(CompFactory.SCT_MajorityConditionsTool(name=name+"Tool"))

    return acc
