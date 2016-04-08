/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
@mainpage MdtCalibEventLoop Package
@author Niels.Van.Eldik@cern.ch
                                                                                
@section MdtCalibEventLoopIntro Introduction
The Mdt calibration, and in particular the autocalibration, is an iterative
procedure. In the Muon Calibration Framework track segments are stored
in memory organized per calibration region (on chamber or one multilayer) 
and are used one or more times by the calibration. This package
provides the loop over these segments.
                                                                                
@section MdtCalibEventLoopOverview Class Overview
The MdtCalibEventLoop package contains a single class,
MuonCalib::MdtCalibEventLoop, responsible for steering the Loop over the
segments and perform calibration (derived from MuonCalib::IMdtCalibration).
In ATHENA framework it is used by MdtCalibTool but it could
also be used in a standalone environment.
                                                                                
@ref used_MdtCalibEventLoop
                                                                                
@ref requirements_MdtCalibEventLoop
*/
                                                                                
/**
@page used_MdtCalibEventLoop Used Packages
@htmlinclude used_packages.html
*/
                                                                                
/**
@page requirements_MdtCalibEventLoop Requirements
@include requirements
*/

