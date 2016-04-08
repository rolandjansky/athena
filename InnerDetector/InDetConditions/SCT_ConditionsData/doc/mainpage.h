/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
@mainpage SCT_ConditionsData package
@author Shaun.Roe@cern.ch, Jorgen.Dalmau@cern.ch


@section SCT_ConditionsDataIntro Introduction
This package provides data classes used to access data in the conditions
 database or derived from the datastream in some way. Access to these underlying data classes is
 provided by the AlgTools in SCT_ConditionsAlgs, and more documentation can be found there.
  - SCT DAQ calibration data
  - SCT DCS conditions



@section SCT_ConditionsDataOverview Class overview 
 

  - SCT_DCSConditionsData: Container for DCS conditions, such as module temperature.
  
  - SCT_CalibData: Container for Calibration data
  
  - SCT_CalibDefectData: Container with a list of defects derived from calibration data
  
  - StringifyVector: Local utility function to tokenise and stream a string to a vector.

@section SCT_ConditionsDataRefs Reference pages
  - @ref used_SCT_ConditionsData
  - @ref requirements_SCT_ConditionsData
*/


/**
@page used_SCT_ConditionsData Used packages
@htmlinclude used_packages.html
*/


/**
@page requirements_SCT_ConditionsData Requirements
@include requirements
*/
