/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
@page MuonCompetingRIOsOnTrack_page The MuonCompetingRIOsOnTrack package
The Muon package for CompetingRIOsOnTrack.

@author niels.van.Eldik@cern.ch

@section MuonCompetingRIOsOnTrack_introductionMuonCompetingRIOsOnTrack Introduction

This package contains the Muon classes which inherit from Trk::CompetingRIOsOnTrack that extends 
the Trk::MeasurementBase class.

These classes are used e.g. by the Deterministic Annealing Filter to
handle several RIO_OnTracks in one "effective detector layer"
(might be more than one DetectorElement), which compete against each other in being assigned to a track.
localParameters() and localErrorMatrix() return the mean values according to the weights (assignment probabilities).
  

@section MuonCompetingRIOsOnTrack_ExtrasMuonCompetingRIOsOnTrack Extra Pages

*/
