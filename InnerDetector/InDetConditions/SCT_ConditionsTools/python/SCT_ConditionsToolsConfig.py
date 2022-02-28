# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory
from AtlasGeoModel.GeoModelConfig import GeoModelCfg
from IOVDbSvc.IOVDbSvcConfig import addFolders, addFoldersSplitOnline
from SCT_GeoModel.SCT_GeoModelConfig import SCT_ReadoutGeometryCfg


def SCT_ByteStreamErrorsToolCfg(flags, name="SCT_ByteStreamErrorsTool", **kwargs):
    acc = SCT_ConfigurationConditionsToolCfg(flags)
    kwargs.setdefault("ConfigTool", acc.popPrivateTools())
    acc.setPrivateTools(CompFactory.SCT_ByteStreamErrorsTool(name, **kwargs))
    return acc


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
    from SCT_Cabling.SCT_CablingConfig import SCT_CablingToolCfg
    acc.addCondAlgo(CompFactory.SCT_ConditionsParameterCondAlg(name=f"{name}CondAlg",
                                                               ReadKey=folder,
                                                               SCT_CablingTool=acc.popToolsAndMerge(SCT_CablingToolCfg(flags))))

    # Condition tool
    acc.setPrivateTools(CompFactory.SCT_ConditionsParameterTool(name=f"{name}Tool"))

    return acc


def SCT_ConditionsSummaryToolCfg(flags, name="InDetSCT_ConditionsSummaryTool", **kwargs):
    acc = ComponentAccumulator()

    # Load flagged condition tool
    withFlaggedCondTool = kwargs.pop("withFlaggedCondTool", True)
    withTdaqTool = kwargs.pop("withTdaqTool", True)

    ConditionsTools = []
    SCT_ConfigurationConditionsTool = acc.popToolsAndMerge(SCT_ConfigurationConditionsToolCfg(flags))
    ConditionsTools += [ SCT_ConfigurationConditionsTool ]

    if withFlaggedCondTool:
        ConditionsTools += [ acc.popToolsAndMerge(SCT_FlaggedConditionToolCfg(flags)) ]
    else:
        name += "WithoutFlagged"

    # Load bytestream errors tool (use default instance without "InDet")
    if not flags.Input.isMC :
        ConditionsTools += [ acc.popToolsAndMerge(SCT_ByteStreamErrorsToolCfg(flags, **{"ConfigTool" : SCT_ConfigurationConditionsTool})) ]

    if flags.InDet.useSctDCS:
        ConditionsTools += [ acc.popToolsAndMerge(SCT_DCSConditionsCfg(flags)) ]

    if withTdaqTool and not flags.Input.isMC :
        ConditionsTools += [ acc.popToolsAndMerge(SCT_TdaqEnabledToolCfg(flags)) ]

    # Load calibration conditions tool
    # @TODO or just for data?
    ConditionsTools += [ acc.popToolsAndMerge(SCT_ReadCalibDataToolCfg(flags)) ]

    # Load conditions Monitoring tool
    if not flags.Common.isOnline:
        # @TODO really also for MC?
        ConditionsTools += [ acc.popToolsAndMerge(SCT_MonitorConditionsToolCfg(flags)) ]

    if flags.InDet.doSCTModuleVeto:
        ConditionsTools += [ acc.popToolsAndMerge(SCT_ModuleVetoCfg(flags)) ]

    kwargs.setdefault("ConditionsTools", ConditionsTools)
    acc.setPrivateTools(CompFactory.SCT_ConditionsSummaryTool(name, **kwargs))
    return acc


def SCT_ConfigurationConditionsToolCfg(flags, name="InDetSCT_ConfigurationConditionsTool", **kwargs):
    # Load conditions configuration service and load folders and algorithm for it
    # Load folders that have to exist for both MC and Data
    folder_path = "/SCT/DAQ/Config/"
    #if its COMP200, use old folders...
    if flags.IOVDb.DatabaseInstance == "COMP200":
        folder_path = "/SCT/DAQ/Configuration/"
    #...but now check if we want to override that decision with explicit flag (if there is one)
    if flags.InDet.ForceCoraCool:
        folder_path = "/SCT/DAQ/Configuration/"
    if flags.InDet.ForceCoolVectorPayload:
        folder_path = "/SCT/DAQ/Config/"
    
    if flags.InDet.ForceCoolVectorPayload and flags.InDet.ForceCoraCool:
        print ("*** SCT DB CONFIGURATION FLAG CONFLICT: Both CVP and CoraCool selected****")
        folder_path = ""

    cond_kwargs = {}
    cond_kwargs["ChannelFolder"] = folder_path + ("Chip" if flags.IOVDb.DatabaseInstance == "COMP200" else "ChipSlim")
    cond_kwargs["ModuleFolder"] = f"{folder_path}Module"
    cond_kwargs["MurFolder"] = f"{folder_path}MUR"
    cond_kwargs["dbInstance"] = "SCT"
    cond_kwargs["SCT_ConfigurationCondAlgName"] = "SCT_ConfigurationCondAlg"

    # For SCT_ID and SCT_DetectorElementCollection used in SCT_ConfigurationCondAlg and SCT_ConfigurationConditionsTool
    acc = SCT_ReadoutGeometryCfg(flags)

    if "ChannelFolderDB" not in cond_kwargs:
        acc.merge(addFoldersSplitOnline(flags,
                                        detDb=cond_kwargs["dbInstance"],
                                        online_folders=cond_kwargs["ChannelFolder"],
                                        offline_folders=cond_kwargs["ChannelFolder"],
                                        className="CondAttrListVec",
                                        splitMC=True))
    else:
        acc.merge(addFolders(flags, [cond_kwargs["ChannelFolderDB"]], detDb = cond_kwargs["dbInstance"], className="CondAttrListVec"))
    if "ModuleFolderDB" not in cond_kwargs:
        acc.merge(addFoldersSplitOnline(flags,
                                        detDb=cond_kwargs["dbInstance"],
                                        online_folders=cond_kwargs["ModuleFolder"],
                                        offline_folders=cond_kwargs["ModuleFolder"],
                                        className="CondAttrListVec",
                                        splitMC=True))
    else:
        acc.merge(addFolders(flags, [cond_kwargs["ModuleFolderDB"]], detDb = cond_kwargs["dbInstance"], className="CondAttrListVec"))
    if "MurFolderDB" not in cond_kwargs:
        acc.merge(addFoldersSplitOnline(flags,
                                        detDb=cond_kwargs["dbInstance"],
                                        online_folders=cond_kwargs["MurFolder"],
                                        offline_folders=cond_kwargs["MurFolder"],
                                        className="CondAttrListVec",
                                        splitMC=True))
    else:
        acc.merge(addFolders(flags, [cond_kwargs["MurFolderDB"]], detDb = cond_kwargs["dbInstance"],  className="CondAttrListVec"))

    ConfigCondAlg_kwargs={}
    ConfigCondAlg_kwargs["ReadKeyChannel"] = cond_kwargs["ChannelFolder"]
    ConfigCondAlg_kwargs["ReadKeyModule"]  = cond_kwargs["ModuleFolder"]
    ConfigCondAlg_kwargs["ReadKeyMur"]     = cond_kwargs["MurFolder"]
    from SCT_ConditionsAlgorithms.SCT_ConditionsAlgorithmsConfig import SCT_ConfigurationCondAlgCfg
    acc.merge(SCT_ConfigurationCondAlgCfg(flags, name="SCT_ConfigurationCondAlg", **ConfigCondAlg_kwargs))

    acc.setPrivateTools(CompFactory.SCT_ConfigurationConditionsTool(name, **kwargs))
    return acc


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
    ReadAllDBFolders = kwargs.get("ReadAllDBFolders", not flags.Common.isOnline)
    ReturnHVTemp = kwargs.get("ReturnHVTemp", True)
    
    # Condition algorithms
    if ReadAllDBFolders == ReturnHVTemp and not flags.Common.isOnline:
        acc.merge(addFolders(flags, stateFolder, dbInstance, className="CondAttrListCollection"))
        # algo
        statArgs = {
            "name": f"{name}StatCondAlg",
            "ReturnHVTemp": ReturnHVTemp,
            "ReadKeyHV": hvFolder,
            "ReadKeyState": stateFolder,
        }
        acc.addCondAlgo(CompFactory.SCT_DCSConditionsStatCondAlg(**statArgs))
    if ReturnHVTemp:
        acc.merge(addFolders(flags, [hvFolder, tempFolder], dbInstance, className="CondAttrListCollection"))
        acc.addCondAlgo(CompFactory.SCT_DCSConditionsHVCondAlg(name=f"{name}HVCondAlg", ReadKey=hvFolder))
        acc.addCondAlgo(CompFactory.SCT_DCSConditionsTempCondAlg(name=f"{name}TempCondAlg", ReadKey=tempFolder))

    # Condition tool
    acc.merge(GeoModelCfg(flags)) # For SCT_ID used in SCT_DCSConditionsTool
    toolkwargs = {}
    toolkwargs["ReadAllDBFolders"] = ReadAllDBFolders
    toolkwargs["ReturnHVTemp"] = ReturnHVTemp
    acc.setPrivateTools(CompFactory.SCT_DCSConditionsTool(name=f"{name}Tool", **toolkwargs))

    return acc


def SCT_FlaggedConditionToolCfg(flags, name="InDetSCT_FlaggedConditionTool", **kwargs):
    # For SCT_ID and SCT_DetectorElementCollection used in SCT_FlaggedConditionTool
    acc = SCT_ReadoutGeometryCfg(flags)
    acc.setPrivateTools(CompFactory.SCT_FlaggedConditionTool(name, **kwargs))
    return acc


def SCT_LinkMaskingCfg(flags, name="SCT_LinkMasking", **kwargs):
    """Configure necessary condition folder, condition algorithm
       for SCT_LinkMaskingTool and return it.
    """
    # For SCT_ID used in SCT_LinkMaskingTool
    acc = GeoModelCfg(flags)

    # Condition folders
    folder = "/purple/pants"
    # mycool.db is hard coded in Database/IOVDbSvc/python/IOVDbSvcConfig.py
    dbConnection = "<dbConnection>sqlite://;schema=mycool.db;dbname=CONDBR2</dbConnection>"
    acc.merge(addFolders(configFlags=flags, folderstrings=folder, className="CondAttrListCollection", db=dbConnection))

    # Condition algorithm
    kwargs.setdefault("ReadKey", folder)
    acc.addCondAlgo(CompFactory.SCT_LinkMaskingCondAlg(name=f"{name}CondAlg", **kwargs))

    # Condition tool
    acc.setPrivateTools(CompFactory.SCT_LinkMaskingTool(name=f"{name}Tool"))

    return acc


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
    acc.setPrivateTools(CompFactory.SCT_MajorityConditionsTool(name=f"{name}Tool"))

    return acc


def SCT_ModuleVetoCfg(flags, name="InDetSCT_ModuleVeto", **kwargs):
    """Return a ComponentAccumulator configured for SCT_ModuleVetoTool
    """
    # For SCT_ID used in SCT_ModuleVetoTool
    acc = GeoModelCfg(flags)

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
    acc.setPrivateTools(CompFactory.SCT_ModuleVetoTool(name=f"{name}Tool", **toolArgs))

    return acc


def SCT_MonitorConditionsToolCfg(flags, name="InDetSCT_MonitorConditionsTool", cond_kwargs={}, **kwargs):
    cond_kwargs.setdefault("Folder", "/SCT/Derived/Monitoring")
    cond_kwargs.setdefault("dbInstance", "SCT_OFL")
    cond_kwargs.setdefault("MonitorCondAlgName", "SCT_MonitorCondAlg")

    # For SCT_ID used in SCT_MonitorConditionsTool
    from AtlasGeoModel.GeoModelConfig import GeoModelCfg
    acc = GeoModelCfg(flags)

    if "FolderDb" not in cond_kwargs:
        cond_kwargs["FolderDb"] = cond_kwargs["Folder"]
    acc.merge(addFolders(flags, cond_kwargs["FolderDb"], cond_kwargs["dbInstance"], className="CondAttrListCollection"))

    acc.addCondAlgo(CompFactory.SCT_MonitorCondAlg(name=cond_kwargs["MonitorCondAlgName"],
                                                   ReadKey=cond_kwargs["Folder"]))

    acc.setPrivateTools(CompFactory.SCT_MonitorConditionsTool(name, **kwargs))

    return acc


def SCT_ReadCalibChipDataCfg(flags, name="SCT_ReadCalibChip", **kwargs):
    """Return configured ComponentAccumulator with SCT_ReadCalibChipDataCfg tool

    Accepts optional noiseFolder and gainFolder keyword arguments
    """
    acc = GeoModelCfg(flags) # For SCT_ID used in SCT_ReadCalibChipNoiseCondAlg, SCT_ReadCalibChipGainCondAlg, SCT_ReadCalibChipDataTool

    # Folders
    noiseFolder = kwargs.get("noiseFolder", "/SCT/DAQ/Calibration/ChipNoise")
    gainFolder = kwargs.get("gainFolder", "/SCT/DAQ/Calibration/ChipGain")
    if flags.Overlay.DataOverlay:
        forceDb="OFLP200"
        noiseTag="SctDaqCalibrationChipNoise-Apr10-01"
        gainTag="SctDaqCalibrationChipGain-Apr10-01"
    else:
        forceDb=None
        noiseTag=None
        gainTag=None

    acc.merge(addFoldersSplitOnline(flags, "SCT", noiseFolder, noiseFolder, "CondAttrListCollection",
                                    forceDb=forceDb, tag=noiseTag))
    acc.merge(addFoldersSplitOnline(flags, "SCT", gainFolder, gainFolder, "CondAttrListCollection",
                                    forceDb=forceDb, tag=gainTag))

    # Algorithms
    acc.addCondAlgo(CompFactory.SCT_ReadCalibChipNoiseCondAlg(name=f"{name}NoiseCondAlg", ReadKey=noiseFolder))
    acc.addCondAlgo(CompFactory.SCT_ReadCalibChipGainCondAlg(name=f"{name}GainCondAlg", ReadKey=gainFolder))

    # Tool
    acc.setPrivateTools(CompFactory.SCT_ReadCalibChipDataTool(name="InDetSCT_ReadCalibChipDataTool"))

    return acc


def SCT_ReadCalibDataToolCfg(flags, name="InDetSCT_ReadCalibDataTool", cond_kwargs={}, **kwargs):
    # For SCT_ID and SCT_DetectorElementCollection used in SCT_ReadCalibDataCondAlg and SCT_ReadCalibDataTool
    acc = SCT_ReadoutGeometryCfg(flags)

    cond_kwargs.setdefault("NoiseFolder", "/SCT/DAQ/Calibration/NoiseOccupancyDefects")
    cond_kwargs.setdefault("GainFolder", "/SCT/DAQ/Calibration/NPtGainDefects")
    cond_kwargs.setdefault("ReadCalibDataCondAlgName", "SCT_ReadCalibDataCondAlg")

    acc.merge(addFoldersSplitOnline(flags,
                                    detDb="SCT",
                                    online_folders=cond_kwargs["NoiseFolder"],
                                    offline_folders=cond_kwargs["NoiseFolder"],
                                    className="CondAttrListCollection",
                                    splitMC=True))
    acc.merge(addFoldersSplitOnline(flags,
                                    detDb="SCT",
                                    online_folders=cond_kwargs["GainFolder"],
                                    offline_folders=cond_kwargs["GainFolder"],
                                    className="CondAttrListCollection",
                                    splitMC=True))

    acc.addCondAlgo(CompFactory.SCT_ReadCalibDataCondAlg(name=cond_kwargs["ReadCalibDataCondAlgName"],
                                                         ReadKeyGain=cond_kwargs["GainFolder"],
                                                         ReadKeyNoise=cond_kwargs["NoiseFolder"]))
    from SCT_Cabling.SCT_CablingConfig import SCT_CablingToolCfg
    kwargs.setdefault("SCT_CablingTool", acc.popToolsAndMerge(SCT_CablingToolCfg(flags)))

    acc.setPrivateTools(CompFactory.SCT_ReadCalibDataTool(name, **kwargs))
    return acc


def SCT_ReadoutToolCfg(flags, name="SCT_ReadoutTool", **kwargs):
    from SCT_Cabling.SCT_CablingConfig import SCT_CablingToolCfg
    acc = SCT_CablingToolCfg(flags)
    kwargs.setdefault("SCT_CablingTool", acc.popPrivateTools())
    acc.setPrivateTools(CompFactory.SCT_ReadoutTool(name, **kwargs))
    return acc


def SCT_RODVetoCfg(flags, name="SCT_RODVeto", **kwargs):
    """Configure necessary condition folders, condition algorithms 
       for SCT_DCSConditionsTool and return it.
    """
    # For SCT_ID used in SCT_RODVetoCondAlg and SCT_RODVetoTool
    acc = GeoModelCfg(flags)

    # For SCT_CablingTool used in SCT_RODVetoCondAlg
    from SCT_Cabling.SCT_CablingConfig import SCT_CablingToolCfg
    kwargs.setdefault("SCT_CablingTool", acc.popToolsAndMerge(SCT_CablingToolCfg(flags)))

    # Condition algorithm (actually event algorithm)
    acc.addEventAlgo(CompFactory.SCT_RODVetoCondAlg(name=f"{name}CondAlg", **kwargs))

    # Condition tool
    acc.setPrivateTools(CompFactory.SCT_RODVetoTool(name=f"{name}Tool"))

    return acc


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
    acc.setPrivateTools(CompFactory.SCT_SensorsTool(name=f"{name}Tool"))

    return acc


def SCT_SiliconConditionsCfg(flags, name="SCT_Silicon", **kwargs):
    """Return a ComponentAccumulator configured for SiliconConditions DB

    DCSConditionsTool may be provided in kwargs
    """
    # For SCT_ID used in SCT_SiliconHVCondAlg, SCT_SiliconTempCondAlg and SCT_SiliconConditionsTool
    acc = GeoModelCfg(flags)

    # Condition algorithms
    algkwargs = {}
    DCSConditionsTool = kwargs.get("DCSConditionsTool")
    if DCSConditionsTool:
        algkwargs["UseState"] = DCSConditionsTool.ReadAllDBFolders
        algkwargs["DCSConditionsTool"] = DCSConditionsTool
    else:
        algkwargs["UseState"] = not flags.Common.isOnline
        algkwargs["DCSConditionsTool"] = acc.popToolsAndMerge(SCT_DCSConditionsCfg(flags))
    acc.addCondAlgo(CompFactory.SCT_SiliconHVCondAlg(name=f"{name}HVCondAlg", **algkwargs))
    acc.addCondAlgo(CompFactory.SCT_SiliconTempCondAlg(name=f"{name}TempCondAlg", **algkwargs))

    # Condition tool
    toolkwargs = {}
    toolkwargs["UseDB"] = kwargs.get("UseDB", True)
    toolkwargs["ForceUseGeoModel"] = kwargs.get("ForceUseGeoModel", False)
    acc.setPrivateTools(CompFactory.SCT_SiliconConditionsTool(name=f"{name}ConditionsTool", **toolkwargs))

    return acc


def SCT_StripVetoCfg(flags, name="SCT_StripVeto", **kwargs):
    """Return a ComponentAccumulator configured for SCT_StripVetoTool
    """
    # For SCT_ID used in SCT_StripVetoTool
    acc = GeoModelCfg(flags)

    # Condition tool
    # Identifiers should be given as strings and decimal.
    # Identifier::compact() method returns unsigned long long.
    kwargs.setdefault("BadStripIdentifiers", ["576522359582752768",
                                              "576522475009998848",
                                              "576522475278434304",
                                              # "576522475546869760",
                                              # "576522475815305216",
                                              # "576522476083740672",
                                             ])
    acc.setPrivateTools(CompFactory.SCT_StripVetoTool(name=f"{name}Tool", **kwargs))

    return acc


def SCT_TdaqEnabledToolCfg(flags, name="InDetSCT_TdaqEnabledTool", **kwargs):
    # For SCT_ID used in SCT_TdaqEnabledTool
    acc = GeoModelCfg(flags)

    # Folder
    # FIXME - is there a better way to do this? What about run3?
    folder = "/TDAQ/Resources/ATLAS/SCT/Robins" if (flags.IOVDb.DatabaseInstance == "CONDBR2") else "/TDAQ/EnabledResources/ATLAS/SCT/Robins"
    acc.merge( addFolders(flags, [folder], detDb="TDAQ", className="CondAttrListCollection") )

    # Algorithm
    from SCT_Cabling.SCT_CablingConfig import SCT_CablingToolCfg
    kwargs.setdefault("SCT_CablingTool", acc.popToolsAndMerge(SCT_CablingToolCfg(flags)))
    acc.addCondAlgo(CompFactory.SCT_TdaqEnabledCondAlg(**kwargs))

    # Tool
    acc.setPrivateTools(CompFactory.SCT_TdaqEnabledTool(name))

    return acc
