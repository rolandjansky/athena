/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**

@page MuonPattern_page MuonPattern package
	@author Niels Van Eldik <niels.van.eldik@cern.ch>
	
@section MuonPattern_MuonPatternIntro Introduction

Package containing classes to store sets of Trk::PrepRawData objects that are the output of the
initial road finding in the muon spectrometer.

The package contains the following objects:
- Muon::MuonPattern: base class for simple lists of Trk::PrepRawData objects. 
- Muon::MuonPrdPattern: concrete implementation of a Muon::MuonPattern storing a list of Trk::PrepRawData objects
- Muon::MuonPatternCombination: The output of the global pattern recognition in the muon spectrometer consists of 
                          global roads in the detector. The Muon::MuonPatternCombination provides the means to 
			  store the Trk::PrepRawData objects sorted by chamber in form of a Muon::MuonPatternChamberIntersect. 
- Muon::MuonPatternChamberIntersect: Object to store a list of PrepRawData objects in a given chamber that 
                               were associated with a pattern. In addition to a list of Trk::PrepRawData, the 
			       Muon::MuonPatternChamberIntersect provides a placeholder for a Trk::TrackParameters 
			       allowing the user to store the intersect of a given pattern with the station.
			       
The objects can be stored in StoreGate using the following collections:
- MuonPatternCollection: a vector of Muon::MuonPattern objects
- MuonPrdPatternCollection: a vector of Muon::MuonPrdPattern objects
- MuonPatternCombinationCollection: a vector of Muon::MuonPatternCombination objects
	
@section MuonPattern_ExtrasMuonPattern Extra Pages

*/
