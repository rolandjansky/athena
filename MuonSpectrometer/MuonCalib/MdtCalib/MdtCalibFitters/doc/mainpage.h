/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
@mainpage MdtCalibFitters Package
@author Niels.Van.Eldik@cern.ch, Fabrizio.Petrucci@cern.ch
                                                                                
@section MdtCalibFittersIntro Introduction
Package containing local pattern recognition algorithms and fitters that 
work on segment level.
                                                                                
@section MdtCalibFittersOverview Class Overview

- MuonCalib::DCSLFitter: straight line fitter for drift circles
- MuonCalib::LocalSegmentResolver: calculates the local position and direction of the segment using the hits on track
- MuonCalib::LocalToPrecision: provides transformation between local and precision reference frames
- MuonCalib::MuCCaFitter: interface to the straight line fitter for drift circles used by Calib
- MuonCalib::MuCCaFitterImplementation: the straight line fitter for drift circles used by Calib
                                                                                
@ref used_MdtCalibFitters
                                                                                
@ref requirements_MdtCalibFitters
*/
                                                                                
/**
@page used_MdtCalibFitters Used Packages
@htmlinclude used_packages.html
*/
                                                                                
/**
@page requirements_MdtCalibFitters Requirements
@include requirements
*/

