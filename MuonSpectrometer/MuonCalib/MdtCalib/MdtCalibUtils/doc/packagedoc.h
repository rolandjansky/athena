/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
@page MdtCalibUtils_page MdtCalibUtils Package
@author Niels.Van.Eldik@cern.ch
                                                                                
@section MdtCalibUtils_MdtCalibUtilsIntro Introduction
This package contains the utilities used for Mdt calibration.
                                                                                
@section MdtCalibUtils_MdtCalibUtilsOverview Class Overview
The MdtCalibUtils package contains the following classes and structures:

- LookUpTable: Generic lookup table (template)
- MuonCalib::MultilayerId: Holds the station, eta, phi and multilayer Ids for a given multilayer
- MuonCalib::RegionDataFromFile: Manages the I/O of the list of multilayers belonging to a calibration region
- MuonCalib::RtData_t_r_reso: Represents an Rt relation in the form of a set of (time,radius,resolution) points
- MuonCalib::RtDataFromFile: Manages the I/O of the Rt realtions from/to file
- MuonCalib::SimpleMdtSegmentSelector: Basic Mdt Segment selector
- MuonCalib::SimplePatternSelector: Basic Pattern selector
- MuonCalib::SingleTubeConstants: Holds the tube constants: t0, inverse propagation speed, an adc correction factor and a status flag
- MuonCalib::TubeDataFromFile: Manage the I/O of MuonCalib::MdtTubeConstants objects
- MuonCalib::UpdateCalibHit: Provides the operator to update drift radius and error of a given MuonCalib::MdtCalibHit using the pointer to the MuonCalib::IRtRelation passed to the constructor
                                                                                
                                                                                
*/
