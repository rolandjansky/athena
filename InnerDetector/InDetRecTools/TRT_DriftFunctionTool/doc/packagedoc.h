/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
@page TRT_DriftFunctionTool_page TRT_DriftFunctionTool package

@author phansen@nbi.dk

@section TRT_DriftFunctionTool_TRT_DriftFunctionToolIntro Introduction

This package provides DriftCircle data from RDO.
It transforms a raw drift time digit into a calibrated
drift radius using various conditions data.

@section TRT_DriftFunctionTool_TRT_DriftFunctionToolOverview Class Overview

 - TRT_DriftFunctionTool : called from TRT_DriftCircleTool

@section TRT_DriftFunctionTool_TRT_DriftFunctionOptions jobOptions

 - AllowDigiVersionOverride : Permit using the Rt relation of another digi version 
 - ForcedDigiVersion : the other digi version (an integer)
 - AllowDataMCOverride : Permit using data tools for retrieving MC constants or vice versa
 - ForceData : use only data tools to access constants
 - OverrideCalibrationSimulation : use constants supplied by text file
 - DriftFunctionFile : the text file (a string, see code for format of file)
 - ForceUniversalErrors : a time-independent resolution is used
 - UniversalError : the time-independent resolution
 - DummyMode : the drift time info is not used
 - ErrorFugdeFactor : the resolution is multiplied by this
 - UseCalDb : TRTCalDbTool is used for Db access in case of data (default is TRT_CondDbTool)
 - TRTCalDbTool : the tool handle is a property
 - TRT_CondDbTool : the tool handle is a property
*/
