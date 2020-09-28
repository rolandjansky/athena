/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/**
@page SCT_ConditionsTools_page SCT_ConditionsTools Package
@author shaun.roe@cern.ch, Susumu.Oda@cern.ch


@section SCT_ConditionsTools_SCT_ConditionsTools Introduction

This package contains tools which access and provide conditions data for SCT detector elements: modules, wafers, chips and strips. The various tools each have a 
specific purpose, but all inherit from ISCT_ConditionsTool, which provides an interface (specifically, the isGood(...) methods) which can be used by the 
SCT_ConditionsSummaryTool to summarise the total status and report this to clients. SCT_ConditionsSummaryTool in turn inherits from IInDetConditionsTool, the
common interface for Pixels and TRT as well as SCT.

   - Conditions data classes are defined in SCT_ConditionsData package. Those classes represent conditions data and are used in SCT_ConditionsAlgorithms and SCT_ConditionsTools.

   - Conditions algorithm classes are defined in SCT_ConditionsAlgorithms package. Those classes retrieve data from COOL database and write out conditions data in StoreGate.

   - Conditions tool classes are defined in this SCT_ConditionsTools package. Those classes are used to retrieved conditions data in StoreGate by clients.

   - Test algorithm classes are defined in SCT_ConditionsAlgorithms package. Those classes are used to test conditions data, conditions algorithms and conditions tools.

@section SCT_ConditionsTools_SCT_ConditionsOverview Class Overview
SCT_ConditionsTools contains the following classes:

   - ISCT_ConditionsTool: Base class for the various conditions tools, the interface provides the isGood() methods to be used by clients.
   
   - SCT_ByteStreamErrorsTool: Used to convey errors in the bytestream. In addition to the common interface provided by ISCT_ConditionsTool, it has methods for 
   retrieving error conditions for modules which display communication/datastream errors. The tool provides the 'isGood' methods to clients based on this information.
     - Interface: ISCT_ByteStreamErrorsTool
     - Conditions data: IDCInDetBSErrContainer ("SCT_ByteStreamErrs")
     - Conditions folder: None
     - Conditions algorithm: None (filled by SCT_RodDecoder and SCT_ByteStreamErrorsTool)
     - Test algorithm: SCT_ByteStreamErrorsTestAlg
     - Test script: testBSError.py
     - Configuration script: SCT_ByteStreamErrorsToolSetup.py
   
   - SCT_ChargeTrappingTool: Used for special digitisation with charge trapping.
     - Interface: ISCT_ChargeTrappingTool
     - Conditions data: SCT_ChargeTrappingCondData
     - Conditions folder: None
     - Conditions algorithm: None (filled by SCT_ChargeTrappingTool)
     - Test algorithm: None
     - Test script: None
     - Configuration script: None

   - SCT_ConditionsParameterTool: Used to give numerical parameters in standalone tests.
     - Interface: ISCT_ConditionsParameterTool
     - Conditions data: SCT_CondParameterData ("SCT_CondParameterData")
     - Conditions folder: /SCT/DAQ/Configuration/Chip
     - Conditions algorithm: SCT_ConditionsParameterCondAlg
     - Test algorithm: SCT_ConditionsParameterTestAlg
     - Test script: testParameters.py
     - Configuration script: SCT_ConditionsParameterToolSetup.py

   - SCT_ConditionsSummaryTool: This class can be configured to loop over the other tools in the package, providing an eventual status which is a summary
   of the component tools. The tools to be used are decided in job options.
     - Interface: IInDetConditionsTool
     - Conditions data: None
     - Conditions folder: None
     - Conditions algorithm: None
     - Test algorithm: SCT_ConditionsSummaryTestAlg
     - Test script: testSummary.py
     - Configuration script: SCT_ConditionsSummaryToolSetup.py
   
   - SCT_ConfigurationConditionsTool: Reads the configuration data for SCT modules which are stored in the COOL database in CoraCool format, and provides module
   or channel mask information to clients.
     - Interface: ISCT_ConfigurationConditionsTool
     - Conditions data: SCT_ConfigurationCondData ("SCT_ConfigurationCondData")
     - Conditions folder: /SCT/DAQ/Config(uration)/{Chip(Slim),Module,MUR,RODMUR,ROD,Geog} 
     - Conditions algorithm: SCT_ConfigurationCondAlg
     - Test algorithm: SCT_ConfigurationConditionsTestAlg
     - Test script: testConfig.py
     - Configuration script: SCT_ConfigurationConditionsToolSetup.py

   - SCT_DCSConditionsTool: Reads DCS data such as power supply status and temperature, and decides on this basis (and some internal criteria) whether a module
   is good or bad at a particular instant.
     - Interface: ISCT_DCSConditionsTool
     - Conditions data: SCT_DCSStatCondData ("SCT_DCSStatCondData"), SCT_DCSFloatCondData ("SCT_DCSHVCondData", "SCT_DCSTemp0CondData")
     - Conditions folder: /SCT/DCS/{CHANSTAT,HV,MODTEMP}
     - Conditions algorithm: SCT_DCSConditionsStatCondAlg, SCT_DCSConditionsHVCondAlg, SCT_DCSConditionsTempCondAlg
     - Test algorithm: SCT_DCSConditionsTestAlg
     - Test script: testDCSConditions.py
     - Configuration script: SCT_DCSConditionsToolSetup.py
   
   - SCT_ElectricFieldTool: Used for special digitisation with charge trapping.
     - Interface: ISCT_ElectricFieldTool
     - Conditions data: None
     - Conditions folder: None
     - Conditions algorithm: None
     - Test algorithm: None
     - Test script: None
     - Configuration script: None

   - SCT_FlaggedConditionTool: Used for retrieving bad wafers with more than 384 fired strips. Those bad wafers are declared by clustering algorithms/tools.
     - Interface: ISCT_FlaggedConditionTool
     - Conditions data: IDCInDetBSErrContainer ("SCT_FlaggedCondData")
     - Conditions folder: None
     - Conditions algorithm: None (filled by SCT_Clusterization)
     - Test algorithm: SCT_FlaggedConditionTestAlg
     - Test script: testFlagged.py
     - Configuration script: SCT_FlaggedConditionToolSetup.py
   
   - SCT_LinkMaskingTool: Analogous to the SCT_ModuleVetoTool, this provides a way to turn off individual Front-End links.
     - Interface: ISCT_ConditionsTool
     - Conditions data: SCT_ModuleVetoCondData ("SCT_LinkMaskingCondData")
     - Conditions folder: /purple/pants (created by createLinkMaskingSQLiteFile.py)
     - Conditions algorithm: SCT_LinkMaskingCondAlg
     - Test algorithm: SCT_LinkMaskingTestAlg
     - Test script: TestLinkMasking.sh
     - Configuration script: SCT_LinkMaskingToolSetup.py

   - SCT_MajorityConditionsTool: Used for retrieving majority of DCS state in Barrel, Endcap A, Endcap C.
     - Interface: ISCT_DetectorLevelConditionsTool
     - Conditions data: SCT_MajorityCondData ("SCT_MajorityCondData")
     - Conditions folder: "/SCT/DCS/MAJ"
     - Conditions algorithm: SCT_MajorityCondAlg
     - Test algorithm: SCT_MajorityConditionsTestAlg
     - Test script: testMajority.py
     - Configuration script: SCT_MajorityConditionsToolSetup.py

   - SCT_ModuleVetoTool: A class which enables one to manually veto modules, either by providing the identifiers in job options or by specifying the module identifiers
   in the /SCT/Manual/BadModules folder of the Cool database.
     - Interface: ISCT_ConditionsTool
     - Conditions data: SCT_ModuleVetoCondData ("SCT_ModuleVetoCondData") or none
     - Conditions folder: /SCT/Manual/BadModules or none (spcified by "BadModuleIdentifiers" in job option)
     - Conditions algorithm: SCT_ModuleVetoCondAlg
     - Test algorithm: SCT_ModuleVetoTestAlg
     - Test script: testModuleVeto.py
     - Configuration script: SCT_ModuleVetoToolSetup.py
   
   - SCT_MonitorConditionsTool: Retrieves data from the database folder /SCT/Derived/Monitoring, which is created by monitoring jobs on the processing farm and contains
   data concerning occupancy/noise during data taking.
     - Interface: ISCT_MonitorConditionsTool
     - Conditions data: SCT_MonitorCondData ("SCT_MonitorCondData")
     - Conditions folder: /SCT/Derived/Monitoring
     - Conditions algorithm: SCT_MonitorCondAlg
     - Test algorithm: SCT_MonitorConditionsTestAlg
     - Test script: testMonRead.py
     - Configuration script: SCT_MonitorConditionsToolSetup.py
   
   - SCT_RadDamageSummaryTool: Used for special digitisation with radiation damage.
     - Interface: ISCT_RadDamageSummaryTool
     - Conditions data: None
     - Conditions folder: None
     - Conditions algorithm: None
     - Test algorithm: None
     - Test script: None
     - Configuration script: None

   - SCT_ReadCalibChipDataTool: Used for retrieving chip calibration parameters.
     - Interface: ISCT_ReadCalibChipDataTool
     - Conditions data: SCT_GainCalibData ("SCT_GainCalibData"), SCT_NoiseCalibData ("SCT_NoiseCalibData")
     - Conditions folder: /SCT/DAQ/Calibration/Chip{Noise,Gain}
     - Conditions algorithm: SCT_ReadCalibChipNoiseCondAlg, SCT_ReadCalibChipGainCondAlg
     - Test algorithm: SCT_ReadCalibChipDataTestAlg
     - Test script: testCalibChipRead.py
     - Configuration script: SCT_ReadCalibChipDataToolSetup.py

   - SCT_ReadCalibDataTool: Accesses the calibration folders of the cool database, which are updated as a result of SCT calibration runs. Provides data on defects and noise
   determined by calibration.
     - Interface: ISCT_ReadCalibDataTool
     - Conditions data: SCT_CalibDefectData ("SCT_CalibDefectNPtGain", "SCT_CalibDefectNPtNoise"), SCT_AllGoodStripInfo ("SCT_AllGoodStripInfo")
     - Conditions folder: /SCT/DAQ/Calibration/{NoiseOccupancyDefects,NPtGainDefects}
     - Conditions algorithm: SCT_ReadCalibDataCondAlg
     - Test algorithm: SCT_ReadCalibDataTestAlg
     - Test script: testCalibRead.py
     - Configuration script: SCT_ReadCalibDataToolSetup.py
   
   - SCT_ReadoutTool: Interface for the SCT_ReadoutTool, used to determine which chips are read out from a module, taking bypass/redundancy configuration
   into account
     - Interface: ISCT_ReadoutTool
     - Conditions data: SCT_ReadoutData
     - Conditions folder: None
     - Conditions algorithm: None
     - Test algorithm: SCT_ReadoutTestAlg
     - Test script: testReadout.py
     - Configuration script: None
  
   - SCT_RODVetoTool: Analogous to the SCT_ModuleVetoTool, this provides a way to turn off individual ReadOut Drivers.
     - Interface: ISCT_ConditionsTool
     - Conditions data: IdentifierSet ("BadSCTModuleIds_RODVeto")
     - Conditions folder: None (specified by "BadRODIds" in job option)
     - Conditions algorithm: SCT_RODVetoCondAlg
     - Test algorithm: SCT_RODVetoTestAlg
     - Test script: testRodVeto.py
     - Configuration script: SCT_RODVetoToolSetup.py

   - SCT_SensorsTool: Provides depletion voltage, crystal orientation and manufacturer
     - Interface: ISCT_SensorsTool
     - Conditions data: SCT_SensorsCondData ("SCT_SensorsCondData")
     - Conditions folder: /SCT/Sensors
     - Conditions algorithm: SCT_SensorsCondAlg
     - Test algorithm: SCT_SensorsTestAlg
     - Test script: testSensors.py
     - Configuration script: SCT_SensorsToolSetup.py

   - SCT_SiliconConditionsTool: Inheriting from ISiliconConditionsTool (a common interface for Pixels and SCT), this gives information needed to calculate the Lorentz angle, 
   such as bias voltage, depletion voltage and silicon temperature. For the moment, the silicon depletion voltage is hard coded.
     - Interface: ISiliconConditionsTool
     - Conditions data: SCT_DCSFloatCondData ("SCT_SiliconBiasVoltCondData", "SCT_SiliconTempCondData")
     - Conditions folder: None
     - Conditions algorithm: SCT_SiliconHVCondAlg, SCT_SiliconTempCondAlg
     - Test algorithm: SCT_SiliconConditionsTestAlg
     - Test script: testSilicon.py
     - Configuration script: SCT_SiliconConditionsToolSetup.py
   
   - SCT_StripVetoTool: Analogous to the SCT_ModuleVetoTool, this provides a way to turn off individual strips manually via job options. Intended for testing and Monte Carlo 
   studies in particular.
     - Interface: ISCT_ConditionsTool
     - Conditions data: None
     - Conditions folder: None (spcified by "BadStripIdentifiers" in job option)
     - Conditions algorithm: None
     - Test algorithm: SCT_StripVetoTestAlg
     - Test script: testStripVeto.py
     - Configuration script: SCT_StripVetoToolSetup.py

   - SCT_TdaqEnabledTool: Provides enabled RODs (only for data)
     - Interface: ISCT_ConditionsTool
     - Conditions data: SCT_TdaqEnabledCondData ("SCT_TdaqEnabledCondData")
     - Conditions folder: /TDAQ/Resources/ATLAS/SCT/Robins (>=Run 2) or /TDAQ/EnabledResources/ATLAS/SCT/Robins (Run 1)
     - Conditions algorithm: SCT_TdaqEnabledCondAlg
     - Test algorithm: SCT_TdaqEnabledTestAlg
     - Test script: testTdaqEnabled.py
     - Configuration script: SCT_TdaqEnabledToolSetup.py
**/
