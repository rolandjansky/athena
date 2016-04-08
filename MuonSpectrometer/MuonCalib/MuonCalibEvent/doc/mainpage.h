/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**

@mainpage MuonCalibEvent Package
@author Martin.Woudstra@nikhef.nl, Niels.Van.Eldik@cern.ch

@section MuonCalibEventIntro Introduction
This package contains the event model classes internally used by the 
calibration.  

@section MuonCalibEventOverview Class Overview
The MuonCalibEvent package contains the following class:

- MdtCalibHit: an object containing all the intermediate information needed in the conversion of raw Mdt data to a drift radius with its error.

and a typedef:

- MuonCalibPatternCollection: a DataVector of MuonCalib::MuonCalibPattern objects

@ref used_MuonCalibEvent
                                                                                
@ref requirements_MuonCalibEvent
                                                                                
*/
                                                                                
/**
@page used_MuonCalibEvent Used Packages
@htmlinclude used_packages.html
*/
                                                                                
/**
@page requirements_MuonCalibEvent Requirements
@include requirements
*/
