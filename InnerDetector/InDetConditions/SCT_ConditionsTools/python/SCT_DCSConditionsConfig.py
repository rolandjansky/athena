"""Define methods to configure SCT_DCSConditions

Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
"""
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory
from AtlasGeoModel.GeoModelConfig import GeoModelCfg
from IOVDbSvc.IOVDbSvcConfig import addFolders
SCT_DCSConditionsTool=CompFactory.SCT_DCSConditionsTool
SCT_DCSConditionsStatCondAlg=CompFactory.SCT_DCSConditionsStatCondAlg
SCT_DCSConditionsHVCondAlg=CompFactory.SCT_DCSConditionsHVCondAlg
SCT_DCSConditionsTempCondAlg=CompFactory.SCT_DCSConditionsTempCondAlg

def SCT_DCSConditionsToolCfg(flags, name="InDetSCT_DCSConditionsTool", **kwargs):
    """Return a configured SCT_DCSConditionsTool"""
    kwargs.setdefault("ReadAllDBFolders", True)
    kwargs.setdefault("ReturnHVTemp", True)
    return SCT_DCSConditionsTool(name, **kwargs)

def SCT_DCSConditionsCfg(flags, name="InDetSCT_DCSConditions", **kwargs):
    """Return configured ComponentAccumulator and tool for SCT_DCSConditions

    DCSConditionsTool may be provided in kwargs
    """
    acc = ComponentAccumulator()
    acc.merge(GeoModelCfg(flags)) # For SCT_ID used in SCT_DCSConditionsTool
    tool = kwargs.get("DCSConditionsTool", SCT_DCSConditionsToolCfg(flags))
    # folder arguments
    dbInstance = kwargs.get("dbInstance", "DCS_OFL")
    hvFolder = kwargs.get("hvFolder", "/SCT/DCS/HV")
    tempFolder = kwargs.get("tempFolder", "/SCT/DCS/MODTEMP")
    stateFolder = kwargs.get("stateFolder", "/SCT/DCS/CHANSTAT")
    if tool.ReadAllDBFolders == tool.ReturnHVTemp:
        acc.merge(addFolders(flags, stateFolder, dbInstance, className="CondAttrListCollection"))
        # algo
        statArgs = {
            "name": name + "StatCondAlg",
            "ReturnHVTemp": tool.ReturnHVTemp,
            "ReadKeyHV": hvFolder,
            "ReadKeyState": stateFolder,
        }
        statAlg = SCT_DCSConditionsStatCondAlg(**statArgs)
        acc.addCondAlgo(statAlg)
    if tool.ReturnHVTemp:
        acc.merge(addFolders(flags, [hvFolder, tempFolder], dbInstance, className="CondAttrListCollection"))
        hvAlg = SCT_DCSConditionsHVCondAlg(name=name + "HVCondAlg", ReadKey=hvFolder)
        acc.addCondAlgo(hvAlg)
        tempAlg = SCT_DCSConditionsTempCondAlg(name=name + "TempCondAlg", ReadKey=tempFolder)
        acc.addCondAlgo(tempAlg)
    acc.setPrivateTools(tool)
    return acc

