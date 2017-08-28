/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
@page MdtCalibFitters_page MdtCalibFitters Package
@author Niels.Van.Eldik@cern.ch, Fabrizio.Petrucci@cern.ch
                                                                                
@section MdtCalibFitters_MdtCalibFittersIntro Introduction
Package containing local pattern recognition algorithms and fitters that 
work on segment level.
                                                                                
@section MdtCalibFitters_MdtCalibFittersOverview Class Overview

- MuonCalib::DCSLFitter: straight line fitter for drift circles
- MuonCalib::LocalSegmentResolver: calculates the local position and direction of the segment using the hits on track
- MuonCalib::LocalToPrecision: provides transformation between local and precision reference frames
- MuonCalib::MuCCaFitter: interface to the straight line fitter for drift circles used by Calib
- MuonCalib::MuCCaFitterImplementation: the straight line fitter for drift circles used by Calib
                                                                                
                                                                                
*/
