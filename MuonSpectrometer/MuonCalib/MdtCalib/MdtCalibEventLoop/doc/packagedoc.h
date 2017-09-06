/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
@page MdtCalibEventLoop_page MdtCalibEventLoop Package
@author Niels.Van.Eldik@cern.ch
                                                                                
@section MdtCalibEventLoop_MdtCalibEventLoopIntro Introduction
The Mdt calibration, and in particular the autocalibration, is an iterative
procedure. In the Muon Calibration Framework track segments are stored
in memory organized per calibration region (on chamber or one multilayer) 
and are used one or more times by the calibration. This package
provides the loop over these segments.
                                                                                
@section MdtCalibEventLoop_MdtCalibEventLoopOverview Class Overview
The MdtCalibEventLoop package contains a single class,
MuonCalib::MdtCalibEventLoop, responsible for steering the Loop over the
segments and perform calibration (derived from MuonCalib::IMdtCalibration).
In ATHENA framework it is used by MdtCalibTool but it could
also be used in a standalone environment.
                                                                                
                                                                                
*/
