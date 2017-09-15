/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
@page LArElecCalib_page LArElecCalib
@author Marco.Delmastro@cern.ch

@section LArElecCalib_LArElecCalibIntro Introduction

This package containes the interfaces to retrieve the electronics calibration
constants for the LAr calorimeters. The classes are normally recorded in the
ConditionStore, and to be used by Algorithms. These classes should not have
explicit dependency on the backend, which is normally a database for the
stored constants. The concrete implementation of the interfaces contained in
this package is found in <a
href="http://isscvs.cern.ch/cgi-bin/viewcvs-all.cgi/offline/LArCalorimeter/LArRawConditions/?cvsroot=atlas">LArCalorimeter/LArRawConditions</a>.

@section LArElecCalib_LArElecCalibOverview Class Overview
  
  The LArElecCalib package contains of following classes:

  - ILArAdc2GeV
  - ILArADC2MeVTool
  - ILArAutoCorr
  - ILArAutoCorrNoiseTool
  - ILArAutoCorrTotalTool
  - ILArCableAttenuation
  - ILArCableLength
  - ILArCaliPulseParams
  - ILArDAC2uA
  - ILArDetCellParams
  - ILArDigitOscillationCorrTool
  - ILArEMEC_Cphi
  - ILArEMEC_HValpha
  - ILArEMEC_HVbeta
  - ILArFEBTimeOffset
  - ILArfSampl
  - ILArGlobal
  - ILArGlobalTimeOffset
  - ILArH6Oscillation
  - ILArHVCorrTool
  - ILArMinBias
  - ILArMphysOverMcal
  - ILArNoise
  - ILArOFC
  - ILArOFC_Shape
  - ILArOFCTool
  - ILArPedestal
  - ILArPhaseTool
  - ILArPhysCaliTdiff
  - ILArRamp
  - ILArRinj
  - ILArRterm
  - ILArShape
  - ILArTdrift
  - ILArTiming
  - ILArTshaper
  - ILAruA2MeV
  - LArPedestalAll



*/
