/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
@page MdtCalibData_page MdtCalibData Package
@author Niels.Van.Eldik@cern.ch, Oliver.Kortner@cern.ch, Martin.Woudstra@cern.ch
                                                                                
@section MdtCalibData_MdtCalibDataIntro Introduction
This package contains the Mdt Calibration Data Model.
                                                                                
@section MdtCalibData_MdtCalibDataOverview Class Overview
The MdtCalibData package contains the following classes:

- MuonCalib::CalibFunc: Generic interface for a calibration function
- MuonCalib::IMdtBackgroundCorFunc: Generic interface for cavern background correction functions
- MuonCalib::IMdtBFieldCorFunc: Generic interface for b-field correction functions
- MuonCalib::IMdtSlewCorFunc: Generic interface for slewing correction functions
- MuonCalib::IMdtTempCorFunc: Generic interface for temperature correction functions
- MuonCalib::IMdtWireSagCorFunc: Generic interface for a wire sag correction function
- MuonCalib::IRtResolution: Generic interface for a resolution function
- MuonCalib::MdtBackgroundCorFunc0: Implementation of a background correction function
- MuonCalib::MdtBFieldCorFunc0: Implementation of a b-field correction function
- MuonCalib::MdtCalibrationFactory: Factory to create instances of correction functions 
- MuonCalib::MdtCorFuncSet: Class which holds all correction functions for a given region
- MuonCalib::MdtFullCalibData: Class which holds the full set of calibration constants for a given tube
- MuonCalib::MdtRtRelation: Class which holds calibration constants per rt-region
- MuonCalib::MdtSlewCorFunc0: Implementation of a slewing correction function
- MuonCalib::MdtTempCorFunc0: Implementation of a temperature correction function
- MuonCalib::MdtTubeCalibContainer: Holds single-tube calibration constants of one chamber
- MuonCalib::MdtTubeCalibContainer::SingleTubeCalib: Calibration constants for a single tube 
- MuonCalib::MdtWireSagCorFunc0: Implemenatation of a wire sag correction function
- MuonCalib::RtChebyshev: This class contains the implementation of an r(t) relationship parametrized by a linear combination of chebyshev polyonomials
- MuonCalib::RtRelationLookUp: Equidistant look up table for rt-relations with the time as key
- MuonCalib::RtResolution0: Implementatin of a resolution function
- MuonCalib::RtResolutionLookUp: Equidistant look up table for resolution tables with the time as key
                                                                                
                                                                                
                                                                                
*/
