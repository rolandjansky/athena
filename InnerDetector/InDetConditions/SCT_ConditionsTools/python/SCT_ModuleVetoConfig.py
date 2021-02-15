"""Define methods to configure SCT_ModuleVetoTool

Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
"""
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory
from AtlasGeoModel.GeoModelConfig import GeoModelCfg
from IOVDbSvc.IOVDbSvcConfig import addFolders

def SCT_ModuleVetoCfg(flags, name="InDetSCT_ModuleVeto", **kwargs):
    """Return a ComponentAccumulator configured for SCT_ModuleVetoTool
    """
    acc = ComponentAccumulator()

    # For SCT_ID used in SCT_ModuleVetoTool
    acc.merge(GeoModelCfg(flags))

    if kwargs["useDB"]:
        # Condition folder
        acc.merge(addFolders(configFlags=flags,
                             folderstrings="/SCT/Manual/BadModules",
                             detDb="SCT_OFL",
                             className="AthenaAttributeList",
                             tag=kwargs["folderTag"]))
        # Condition algorithm
        acc.addCondAlgo(CompFactory.SCT_ModuleVetoCondAlg())

    # Condition tool
    toolArgs = {}
    toolArgs["BadModuleIdentifiers"] = kwargs["BadModuleIdentifiers"]
    acc.setPrivateTools(CompFactory.SCT_ModuleVetoTool(name=name+"Tool", **toolArgs))

    return acc
