# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

def getSCT_ConfigurationConditionsTool() :
    # Load conditions configuration service and load folders and algorithm for it
    # Load folders that have to exist for both MC and Data
    from IOVDbSvc.CondDB import conddb
    from InDetRecExample.InDetJobProperties import InDetFlags

    SCTConfigurationFolderPath='/SCT/DAQ/Config/'
    #if its COMP200, use old folders...
    if (conddb.dbdata == "COMP200"):
        SCTConfigurationFolderPath='/SCT/DAQ/Configuration/'
    #...but now check if we want to override that decision with explicit flag (if there is one)
    if InDetFlags.ForceCoraCool():
        SCTConfigurationFolderPath='/SCT/DAQ/Configuration/'

    if InDetFlags.ForceCoolVectorPayload():
        SCTConfigurationFolderPath='/SCT/DAQ/Config/'

    if (InDetFlags.ForceCoolVectorPayload() and InDetFlags.ForceCoraCool()):
        raise Exception('*** SCT DB CONFIGURATION FLAG CONFLICT: Both CVP and CoraCool selected****')

    from SCT_ConditionsTools.SCT_ConfigurationConditionsToolSetup import SCT_ConfigurationConditionsToolSetup
    sct_ConfigurationConditionsToolSetup = SCT_ConfigurationConditionsToolSetup()
    if SCTConfigurationFolderPath=='/SCT/DAQ/Configuration/':
        sct_ConfigurationConditionsToolSetup.setChannelFolder(SCTConfigurationFolderPath+"Chip") # Run 1 data (COMP200)
    else:
        sct_ConfigurationConditionsToolSetup.setChannelFolder(SCTConfigurationFolderPath+"ChipSlim") # For MC (OFLP200) and Run 2, 3 data (CONDBR2)
    sct_ConfigurationConditionsToolSetup.setModuleFolder(SCTConfigurationFolderPath+"Module")
    sct_ConfigurationConditionsToolSetup.setMurFolder(SCTConfigurationFolderPath+"MUR")
    sct_ConfigurationConditionsToolSetup.setup()
    return sct_ConfigurationConditionsToolSetup.getTool()

def getSCT_ByteStreamErrorsTool() :
    from SCT_ConditionsTools.SCT_ByteStreamErrorsToolSetup import SCT_ByteStreamErrorsToolSetup
    sct_ByteStreamErrorsToolSetup = SCT_ByteStreamErrorsToolSetup()
    sct_ByteStreamErrorsToolSetup.setConfigTool( getSCT_ConfigurationConditionsTool() )
    sct_ByteStreamErrorsToolSetup.setup()
    return sct_ByteStreamErrorsToolSetup.getTool()
