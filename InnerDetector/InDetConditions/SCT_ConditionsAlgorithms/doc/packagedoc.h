/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/**
@page SCT_ConditionsAlgorithms_page SCT_ConditionsAlgorithms Package
@author Susumu.Oda@cern.ch


@section SCT_ConditionsAlgorithms_SCT_ConditionsAlgorithms Introduction

This package contains condition algorithm and test algorithms for SCT condition tools in SCT_ConditionsTools package

@section SCT_ConditionsAlgorithms_SCT_ConditionsAlgorithmsOverview Class Overview
SCT_ConditionsAlgorithms package contains the following classes.

@subsection SCT_ConditionsAlgorithms_SCT_ConditionsAlgorithmsCondAlg Condition algorithms

- SCT_AlignCondAlg
  - Purpose: For SCT alignment. Followed by SCT_DetectorElementCondAlg
  - Input condition folder: /Indet/Align or (Indet/AlignL1/ID, /Indet/AlignL2/SCT, /Indet/AlignL3)
  - Output condition data: GeoAlignmentStore ("SCTAlignmentStore")

- SCT_ConditionsParameterCondAlg
  - Purpose: For SCT_ConditionsParameterTool
  - Input condition folder: /SCT/DAQ/Configuration/Chip
  - Output condition data: SCT_CondParameterData ("SCT_CondParameterData")

- SCT_ConfigurationCondAlg
  - Purpose: For SCT_ConfigurationConditionsTool
  - Input condition folder: /SCT/DAQ/Config(uration)/{Chip(Slim),Module,MUR,RODMUR,ROD,Geog}
  - Output condition data: SCT_ConfigurationCondData ("SCT_ConfigurationCondData")

- SCT_DCSConditionsHVCondAlg
  - Purpose: For SCT_DCSConditionsTool
  - Input condition folder: /SCT/DCS/HV
  - Output condition data: SCT_DCSFloatCondData ("SCT_DCSHVCondData")

- SCT_DCSConditionsStatCondAlg
  - Purpose: For SCT_DCSConditionsTool
  - Input condition folder: /SCT/DCS/{HV,CHANSTAT}
  - Output condition data: SCT_DCSStatCondData ("SCT_DCSStatCondData")

- SCT_DCSConditionsTempCondAlg
  - Purpose: For SCT_DCSConditionsTool
  - Input condition folder: /SCT/DCS/MODTEMP
  - Output condition data: SCT_DCSFloatCondData ("SCT_DCSTemp0CondData")

- SCT_DetectorElementCondAlg
  - Purpose: For SCT alignment
  - Input condition data: GeoAlignmentStore ("SCTAlignmentStore")
  - Output condition data: InDetDD::SiDetectorElementCollection> ("SCT_DetectorElementCollection")

- SCT_LinkMaskingCondAlg
  - Purpose: For SCT_LinkMaskingTool
  - Input condition folder: /purple/pants (created by createLinkMaskingSQLiteFile.py)
  - Output condition data: SCT_ModuleVetoCondData ("SCT_LinkMaskingCondData")

- SCT_MajorityCondAlg
  - Purpose: For SCT_MajorityConditionsTool
  - Input condition folder: /SCT/DCS/MAJ
  - Output condition data: SCT_MajorityCondData ("SCT_MajorityCondData")

- SCT_ModuleVetoCondAlg
  - Purpose: For SCT_ModuleVetoTool
  - Input condition folder: /SCT/Manual/BadModules
  - Output condition data: SCT_ModuleVetoCondData ("SCT_ModuleVetoCondData")

- SCT_MonitorCondAlg
  - Purpose: For SCT_MonitorConditionsTool
  - Input condition folder: /SCT/Derived/Monitoring
  - Output condition data: SCT_MonitorCondData ("SCT_MonitorCondData")

- SCT_RODVetoCondAlg
  - Purpose: For SCT_RODVetoToolSCT_RODVetoTool
  - Input condition folder: None (specified by "BadRODIds" in job option)
  - Output condition data: IdentifierSet ("BadSCTModuleIds_RODVeto")

- SCT_ReadCalibChipGainCondAlg
  - Purpose: For SCT_ReadCalibChipDataTool
  - Input condition folder: /SCT/DAQ/Calibration/ChipGain
  - Output condition data: SCT_GainCalibData ("SCT_GainCalibData")

- SCT_ReadCalibChipNoiseCondAlg
  - Purpose: For SCT_ReadCalibChipDataTool
  - Input condition folder: /SCT/DAQ/Calibration/ChipNoise
  - Output condition data: SCT_NoiseCalibData ("SCT_NoiseCalibData")

- SCT_ReadCalibDataCondAlg
  - Purpose: For SCT_ReadCalibDataTool
  - Input condition folder: /SCT/DAQ/Calibration/{NoiseOccupancyDefects,NPtGainDefects}
  - Output condition data: SCT_CalibDefectData ("SCT_CalibDefectNPtGain", "SCT_CalibDefectNPtNoise"), SCT_AllGoodStripInfo ("SCT_AllGoodStripInfo")

- SCT_SensorsCondAlg
  - Purpose: For SCT_SensorsTool
  - Input condition folder: /SCT/Sensors
  - Output condition data: SCT_SensorsCondData ("SCT_SensorsCondData")

- SCT_SiliconHVCondAlg
  - Purpose: For SCT_SiliconConditionsTool
  - Input condition data: SCT_DCSStatCondData ("SCT_DCSStatCondData"), SCT_DCSFloatCondData ("SCT_DCSHVCondData")
  - Output condition data: SCT_DCSFloatCondData ("SCT_SiliconBiasVoltCondData")

- SCT_SiliconTempCondAlg
  - Purpose: For SCT_SiliconConditionsTool
  - Input condition data: SCT_DCSStatCondData ("SCT_DCSStatCondData"), SCT_DCSFloatCondData ("SCT_DCSTemp0CondData")
  - Output condition data: SCT_DCSFloatCondData ("SCT_SiliconTempCondData")

- SCT_TdaqEnabledCondAlg
  - Purpose: For SCT_TdaqEnabledTool
  - Input condition folder: /TDAQ/Resources/ATLAS/SCT/Robins (>=Run 2) or /TDAQ/EnabledResources/ATLAS/SCT/Robins (Run 1)
  - Output condition data: SCT_TdaqEnabledCondData ("SCT_TdaqEnabledCondData")

@subsection SCT_ConditionsAlgorithms_SCT_ConditionsAlgorithmsTestAlg Test algorithms

- SCT_ByteStreamErrorsTestAlg
  - Purpose: To test SCT_ByteStreamErrorsTool
  - Test script: testBSError.py

- SCT_ConditionsParameterTestAlg
  - Purpose: To test SCT_ConditionsParameterTool
  - Test script: testParameters.py

- SCT_ConditionsSummaryTestAlg
  - Purpose: To test SCT_ConditionsSummaryTool
  - Test script: testSummary.py

- SCT_ConfigurationConditionsTestAlg
  - Purpose: To test SCT_ConfigurationConditionsTool
  - Test script: testConfig.py

- SCT_DCSConditionsTestAlg
  - Purpose: To test SCT_DCSConditionsTool
  - Test script: testDCSConditions.py

- SCT_FlaggedConditionTestAlg
  - Purpose: To test SCT_FlaggedConditionTool
  - Test script: testFlagged.py

- SCT_LinkMaskingTestAlg
  - Purpse: To test SCT_LinkMaskingTool
  - Test script: TestLinkMasking.sh

- SCT_MajorityConditionsTestAlg
  - Purpose: To test SCT_MajorityConditionsTool
  - Test script: testMajority.py

- SCT_ModuleVetoTestAlg
  - Purpose: To test SCT_ModuleVetoTool
  - Test script: testModuleVeto.py

- SCT_MonitorConditionsTestAlg
  - Purpose: To test SCT_MonitorConditionsTool
  - Test script: testMonRead.py

- SCT_RODVetoTestAlg
  - Purpose: To test SCT_RODVetoTool
  - Test script: testRodVeto.py

- SCT_ReadCalibChipDataTestAlg
  - Purpose: To test SCT_ReadCalibChipDataTool
  - Test script: testCalibChipRead.py

- SCT_ReadCalibDataTestAlg
  - Purpose: To test SCT_ReadCalibDataTool
  - Test script: testCalibRead.py

- SCT_ReadoutTestAlg
  - Purpose: To test SCT_ReadoutTool
  - Test script: testReadout.py

- SCT_SensorsTestAlg
  - Purpose: To test SCT_SensorsTool
  - Test script: testSensors.py

- SCT_SiliconConditionsTestAlg
  - Purpose: To test SCT_SiliconConditionsTool
  - Test script: testSilicon.py

- SCT_StripVetoTestAlg
  - Purpose: To test SCT_StripVetoTool
  - Test script: testStripVeto.py

- SCT_TdaqEnabledTestAlg
  - Purpose: To test SCT_TdaqEnabledTool
  - Test script: testTdaqEnabled.py

@section SCT_ConditionsAlgorithms_SCT_ConditionsAlgorithmsUtility Files for utilities

- FillFromStringUtility.h
  - fillArrayFromString and fillEmptyVectorFromString functions
  - Used by SCT_ReadCalibChipGainCondAlg.cxx, SCT_ReadCalibChipNoiseCondAlg.cxx, SCT_ReadCalibDataCondAlg.cxx

- SCT_ConditionsUtilities.h and SCT_ConditionsUtilities.cxx
  - getenv function
  - Used by SCT_ConditionsParameterTestAlg.cxx

- SCT_SimpleHisto.h
  - init, fill, asXmlString, xmlHeader, stylesheet functions
  - Used by SCT_ConditionsParameterTestAlg.cxx

**/
