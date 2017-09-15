/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
@page LArRawConditions_page LArRawConditions
@author Marco.Delmastro@cern.ch

@section LArRawConditions_LArRawConditionsIntro Introduction

This package containes the concrete implementation classes of the electronics
calibration constants for the LAr calorimeters. The classes are normally recorded
in the ConditionStore, and to be used by Algorithms. The interfaces to retrieve
the data classes contained in this this package are found in <a
href="http://isscvs.cern.ch/cgi-bin/viewcvs-all.cgi/offline/LArCalorimeter/LArElecCalib/?cvsroot=atlas">LArCalorimeter/LArElecCalib</a>.

@section LArRawConditions_LArRawConditionsOverview Class Overview
  
  The LArRawConditions package contains of following classes:

  - IFillNovaIOVTool
  - LArAutoCorrComplete
  - LArAutoCorrDB
  - LArAutoCorrMC
  - LArAutoCorrP
  - LArCableAttenuationComplete
  - LArCableAttenuationDB
  - LArCableAttenuationP
  - LArCableLengthComplete
  - LArCableLengthDB
  - LArCableLengthP
  - LArCalibParamsDB
  - LArCalibParams
  - LArCaliPulseParamsComplete
  - LArCaliPulseParamsDB
  - LArCaliPulseParamsP
  - LArCaliPulseParamsVsCalib
  - LArCaliWaveContainer
  - LArCaliWaveDB
  - LArCaliWave
  - LArCellTimeOffset
  - LArCondFEBIdChanMap
  - LArConditionsChannelSet
  - LArConditionsContainerBase
  - LArConditionsContainerDB
  - LArConditionsContainer
  - LArConditionsSubset
  - LArConditionsVsCalibBase
  - LArConditionsVsCalibDB
  - LArConditionsVsCalib
  - LArDAC2uAComplete
  - LArDAC2uADB
  - LArDAC2uAMC
  - LArDAC2uAP
  - LArDetCellParamsComplete
  - LArDetCellParamsDB
  - LArDetCellParamsP
  - LArEMEC_CphiComplete
  - LArEMEC_CphiDB
  - LArEMEC_CphiP
  - LArEMEC_HValphaComplete
  - LArEMEC_HValphaDB
  - LArEMEC_HValphaP
  - LArEMEC_HVbetaComplete
  - LArEMEC_HVbetaDB
  - LArEMEC_HVbetaP
  - LArFEBTimeOffsetDB
  - LArFEBTimeOffset
  - LArFecLvTempDcs
  - LArfSamplComplete
  - LArfSamplDB
  - LArfSamplMC
  - LArfSamplP
  - LArGlobalTimeOffsetDB
  - LArGlobalTimeOffsetDB
  - LArGlobalTimeOffset
  - LArH6OscillationComplete
  - LArH6OscillationDB
  - LArH6OscillationP
  - LArMCSym
  - LArMinBiasComplete
  - LArMinBiasDB
  - LArMinBiasMC
  - LArMinBiasP
  - LArMphysOverMcalComplete
  - LArMphysOverMcalDB
  - LArMphysOverMcalMC
  - LArMphysOverMcalP
  - LArNoiseComplete
  - LArNoiseDB
  - LArNoiseMC
  - LArNoiseP
  - LArOFCComplete
  - LArOFCDB
  - LArOFCDB_V0
  - LArOFCMC
  - LArOFCP
  - LArPedestalComplete
  - LArPedestalDB
  - LArPedestalMC
  - LArPedestalP
  - LArPhysCaliTdiffComplete
  - LArPhysCaliTdiffDB
  - LArPhysCaliTdiffP
  - LArPhysWaveContainer
  - LArPhysWaveDB
  - LArPhysWave
  - LArRamp4Complete
  - LArRamp4DB
  - LArRampComplete
  - LArRampDB
  - LArRampMC
  - LArRampP
  - LArRawConditionsDict
  - LArRawRampContainer
  - LArRawRamp
  - LArRinjComplete
  - LArRinjDB
  - LArRinjP
  - LArShape32DB
  - LArShape32MC
  - LArShapeComplete
  - LArShapeMC
  - LArShapeP
  - LArTdriftComplete
  - LArTdriftDB
  - LArTdriftP
  - LArTshaperComplete
  - LArTshaperDB
  - LArTshaperP
  - LAruA2MeVComplete
  - LAruA2MeVDB
  - LAruA2MeVMC
  - LAruA2MeVP
  - LArWaveCumul
  - LArWave
  - LArWaveHelper
  - LArWFParamsComplete
  - LArWFParamsContainer
  - LArWFParamsDB
  - LArWFParams



*/
