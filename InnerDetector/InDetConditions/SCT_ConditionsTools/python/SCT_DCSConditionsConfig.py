"""Define methods to configure SCT_DCSConditions

Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
"""
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory
from AtlasGeoModel.GeoModelConfig import GeoModelCfg
from IOVDbSvc.IOVDbSvcConfig import addFolders
SCT_DCSConditionsTool=CompFactory.SCT_DCSConditionsTool
SCT_DCSConditionsStatCondAlg=CompFactory.SCT_DCSConditionsStatCondAlg
SCT_DCSConditionsHVCondAlg=CompFactory.SCT_DCSConditionsHVCondAlg
SCT_DCSConditionsTempCondAlg=CompFactory.SCT_DCSConditionsTempCondAlg

def SCT_DCSConditionsCfg(flags, name="InDetSCT_DCSConditions", **kwargs):
    """Configure necessary condition folders, condition algorithms 
       for SCT_DCSConditionsTool and return it.
    """
    acc = ComponentAccumulator()

    # Condition folders
    isDataHLT = flags.Common.isOnline and not flags.Input.isMC
    dcsFolder = "/SCT/HLT/DCS" if isDataHLT else "/SCT/DCS"
    dbInstance = kwargs.get("dbInstance", "SCT" if isDataHLT else "DCS_OFL")
    hvFolder = kwargs.get("hvFolder", dcsFolder + "/HV")
    tempFolder = kwargs.get("tempFolder", dcsFolder + "/MODTEMP")
    stateFolder = kwargs.get("stateFolder", dcsFolder + "/CHANSTAT")
    ReadAllDBFolders = kwargs.get("ReadAllDBFolders", True)
    ReturnHVTemp = kwargs.get("ReturnHVTemp", True)

    # Condition algorithms
    if ReadAllDBFolders == ReturnHVTemp:
        acc.merge(addFolders(flags, stateFolder, dbInstance, className="CondAttrListCollection"))
        # algo
        statArgs = {
            "name": name + "StatCondAlg",
            "ReturnHVTemp": ReturnHVTemp,
            "ReadKeyHV": hvFolder,
            "ReadKeyState": stateFolder,
        }
        statAlg = SCT_DCSConditionsStatCondAlg(**statArgs)
        acc.addCondAlgo(statAlg)
    if ReturnHVTemp:
        acc.merge(addFolders(flags, [hvFolder, tempFolder], dbInstance, className="CondAttrListCollection"))
        hvAlg = SCT_DCSConditionsHVCondAlg(name=name + "HVCondAlg", ReadKey=hvFolder)
        acc.addCondAlgo(hvAlg)
        tempAlg = SCT_DCSConditionsTempCondAlg(name=name + "TempCondAlg", ReadKey=tempFolder)
        acc.addCondAlgo(tempAlg)

    # Condition tool
    acc.merge(GeoModelCfg(flags)) # For SCT_ID used in SCT_DCSConditionsTool
    toolkwargs = {}
    toolkwargs["ReadAllDBFolders"] = ReadAllDBFolders
    toolkwargs["ReturnHVTemp"] = ReturnHVTemp
    acc.setPrivateTools(SCT_DCSConditionsTool(name="InDetSCT_DCSConditionsTool", **toolkwargs))

    return acc
