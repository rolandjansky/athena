/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**


@mainpage
@author Zdenko.Van.Kesteren@cern.ch

@section CalibNtupleReaderIntro Introduction
Package responsible for reading in the CalibrationNtuple, storing the variables into Calib-EDM classes (as described in MuonCalibEventBase).

@section CalibNtupleReaderOverview Class Overview
This package consists out of two classes:
- NTReaderBase : This is a helper class, designed to store the CalibrationNtuple variables
- MuonCalib::NTReader : class which fills the Calib-EDM classes with the CalibrationNtuple variables

@htmlinclude used_packages.html

@include requirements

*/


/**
@page used_CalibNtupleReader Used Packages
@htmlinclude used_packages.html
*/

/**
@page requirements_CalibNtupleReader Requirements
@include requirements
*/
