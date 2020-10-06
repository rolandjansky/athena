/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/**
@page SCT_ConditionsData_page SCT_ConditionsData package
@author Shaun.Roe@cern.ch, Jorgen.Dalmau@cern.ch, Susumu.Oda@cern.ch


@section SCT_ConditionsData_SCT_ConditionsDataIntro Introduction
This package provides data classes used to access data in the conditions
 database or derived from the datastream in some way. Access to these underlying data classes is
 provided by the AlgTools in SCT_ConditionsTools, and more documentation can be found there.
  - SCT DAQ calibration data
  - SCT DAQ configuration
  - SCT DCS conditions
  - SCT byte stream errors
  - SCT noisy strips
  - SCT wafers with too many hits
  - SCT sensors
  - Veto some SCT elements
  - etc.

@section SCT_ConditionsData_SCT_ConditionsDataOverview Class overview

@subsection File for SCT_ByteStreamErrorsTool
  - SCT_ByteStreamErrors.h 

@subsection File for SCT_ConditionsParameterTool
  - SCT_ConditionsParameters.h: The several enums, which can be commonly used, are also defined.

@subsection Files for ROOT dictionary 
  - SCT_ConditionsDataDict.h
  - selection.xml

@subsection Class for SCT_ChargeTrappingTool
  - SCT_ChargeTrappingCondData

@subsection Class for SCT_ConditionsParameterTool, SCT_ConditionsParameterCondAlg
  - SCT_CondParameterData

@subsection Class for SCT_ConfigurationConditionsTool, SCT_ConfigurationCondAlg
  - SCT_ConfigurationCondData

@subsection Classes for SCT_DCSConditionsTool, SCT_DCSConditionsHVCondAlg, SCT_DCSConditionsStatCondAlg, SCT_DCSConditionsTempCondAlg
  - SCT_DCSFloatCondData
  - SCT_DCSStatCondData

@subsection Class for SCT_FlaggedConditionTool
  - SCT_FlaggedCondData: Obsolete. SCT_FlaggedConditionTool uses IDCInDetBSErrContainer now.

@subsection Class for SCT_MajorityConditionsTool, SCT_MajorityCondAlg
  - SCT_MajorityCondData

@subsection Class for SCT_ModuleVetoTool, SCT_LinkMaskingTool, SCT_ModuleVetoCondAlg, SCT_LinkMaskingCondAlg
  - SCT_ModuleVetoCondData

@subsection Class for SCT_MonitorConditionsTool, SCT_MonitorCondAlg
  - SCT_MonitorCondData

@subsection Classes for SCT_ReadCalibChipDataTool, SCT_ReadCalibChipGainCondAlg, SCT_ReadCalibChipNoiseCondAlg
  - SCT_GainCalibData
  - SCT_ModuleCalibParameter
  - SCT_ModuleGainCalibData
  - SCT_ModuleNoiseCalibData
  - SCT_NoiseCalibData

@subsection Classes for SCT_ReadCalibDataTool, SCT_ReadCalibDataCondAlg
  - SCT_AllGoodStripInfo: std::array of SCT_WaferGoodStripInfo to be used in SCT_ReadCalibDataCondAlg and SCT_ReadCalibDataTool.
  - SCT_CalibDefectData: Container with a list of defects derived from calibration data
  - SCT_WaferGoodStripInfo

@subsection Classes for SCT_ReadoutTool, SCT_ConditionsParameterCondAlg, SCT_ConfigurationCondAlg
  - SCT_Chip
  - SCT_PortMap
  - SCT_ReadoutData

@subsection Class for SCT_RODVetoTool, SCT_RODVetoCondAlg
  - IdentifierSet: std::set of Identifier to be used in SCT_RODVetoCondAlg and SCT_RODVetoTool. 

@subsection Classes for SCT_SensorsTool, SCT_SensorsCondAlg
  - SCT_SensorCondData
  - SCT_SensorsCondData

@subsection Class for SCT_TdaqEnabledTool, SCT_TdaqEnabledCondAlg
  - SCT_TdaqEnabledCondData
  
@section SCT_ConditionsData_SCT_ConditionsDataRefs Reference pages
*/
