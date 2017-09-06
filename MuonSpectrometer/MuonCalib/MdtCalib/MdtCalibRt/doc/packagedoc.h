/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
@page MdtCalibRt_page MdtCalibRt Package
@author Fabrizio.Petrucci@cern.ch 
                                                                                
@section MdtCalibRt_MdtCalibRtIntro Introduction
This package is foreseen to collect the various implementations of Mdt Rt calibration. 
                                                                                
@section MdtCalibRt_MdtCalibRtOverview Class Overview
The MdtCalibRt package contains at the moment the following classes:

- MuonCalib::RtCalibrationAnalytic: This class performs the analytic autocalibration whose basic ideas were developed by Mario Deile (see ATL-MUON-2004-021)

- MuonCalib::RtCalibrationIntegration: This class allows the user to obtain an r-t relationship from the drift time spectrum in a given calibration region. 

- MuonCalib::RtCalibrationClassic: Implementation of a rt calibration using the classical approach (iterative residuals minimization)
- MuonCalib::ClassicSettings: Parameters needed by the Rt classic calibrations which should be provided to the RtCalibrationClassic constructor

- MuonCalib::RtCalibrationOutput: Class for communication between event loop and rt calibration algorithm contains only a rt relation for now

- MuonCalib::RtHistos: Set of histograms used in an iteration of RtCalibrationClassic
                                                                                
                                                                                
*/
