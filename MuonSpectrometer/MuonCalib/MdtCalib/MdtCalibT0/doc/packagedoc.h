/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
@page MdtCalibT0_page MdtCalibT0 Package
@author Domizia.Orestano@cern.ch 
                                                                                
@section MdtCalibT0_MdtCalibT0Intro Introduction
This package is foreseen to collect the various implementations of Mdt T0 calibration. 
                                                                                
@section MdtCalibT0_MdtCalibT0Overview Class Overview
The MdtCalibT0 package currently contains the following classes common to all the implementations:

- MuonCalib::T0CalibrationOutput: Class for communication between event loop and T0 calibration  

The following classes related to "Classic" implementation:
- MuonCalib::T0ClassicSettings: Parameters needed by the T0 calibrations which should be provided to the T0CalibrationClassic constructor
- MuonCalib::T0CalibrationClassic: Implementation of a T0 calibration using the classical approach
- MuonCalib::T0ClassicHistos: Tube histograms used in T0 calibration 

The following classes related to "MT" implementation:
- MuonCalib::T0MTSettings: Parameters needed by the T0 calibrations which should be provided to the T0CalibrationMT constructor. Uses T0MTSettingsT0 and T0MTSettingsTMax.
- MuonCalib::T0MTSettingsT0: Settings for the T0 fit to the drift time spectrum
- MuonCalib::T0MTSettingsTMax: Settings for the TMax fit to the drift time spectrum
- MuonCalib::T0CalibrationMT: Implementation of a T0 calibration using the MT approach
- MuonCalib::T0MTHistos:Histogram and fitter class for drift time and pulsehight spectra. 
- MuonCalib::MTT0PattternRecognition: Performs the pattern recognition for the MT t0-fit routine. Fir ranges and start values are estimated.
- MuonCalib::MTTmaxPattternRecognition: Performs the pattern recognition for the MT tmax-fit routine. Fir ranges and start values are estimated.
- MuonCalib::HistogramId:Identifier class for drift time histograms. The class can be switched to the identification of single tubes, and different types of groupes of tubes.
- MuonCalib::VariableBinwidthHistogram: a histogram where every bin has the same number of entries. The density is represented by the bin width.
- MuonCalib::VariableBinwidthHistogramBin: a bin of a VariableBinwidthHistogram.

                                                                                
*/
