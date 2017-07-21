/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
@page MuonCalibIdentifier_page MuonCalibIdentifier Package
@author Domizia.Orestano@cern.ch 

Original authors are Martin Woudstra, Zdenko van Kesteren and Peter Kluit 

@section MuonCalibIdentifier_MuonCalibIdentifierIntro Introduction
MuonCalibIdentifier is a utility package which collects classes and macros 
defining identifiers and hashes internally used by the Muon Calibration 

@section MuonCalibIdentifier_MuonCalibIdentifierOverview Class Overview
The MuonCalibIdentifier package contains the following classes used by other 
MuonCalib packages

- MdtHashTable: provides fast conversion between identifiers and hashes for 
MDT chambers, MDT multilayers, MDT tubes (=global) and MDT tube-in-chamber (=local).
All data members and member functions are static, so no instantiation of an MdtHashTable is needed.

- MdtRegion: class defining a calibration region through the list of the identifiers associated to it 

- MdtRegionSet: A full set of regions, should normally cover the whole detector 

- MdtRegionTool: manages the access to the MdtRegion in the MdtRegionSet through hashes

- MuonCalib::MuonFixedId: Implements fixed identifiers not dependent upon Athena Identifier for internal use in the calibration framework. 

and many others used internally to provide the above functionalities (see Class list).



*/
