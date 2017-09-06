/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
@page TrkCompetingRIOsOnTrack_page The TrkCompetingRIOsOnTrack package
The base class package for CompetingRIOsOnTrack.

@author Sebastian.Fleischmann@cern.ch

@section TrkCompetingRIOsOnTrack_introductionTrkCompetingRIOsOnTrack Introduction

This package contains the base class for all CompetingRIOsOnTrack that extends 
the Trk::MeasurementBase class.

This class is used e.g. by the Deterministic Annealing Filter to
handle several RIO_OnTracks in one "effective detector layer"
(might be more than one DetectorElement), which compete against
each other in being assigned to a track.
localParameters() and localErrorMatrix() should return the mean values
according to the weights (assignment probabilities).

CompetingRIOsOnTrack are characterized by a vector of Trk::RIO_OnTrack objects they hold, 
by a Surface the CompetingRIOsOnTrack is expressed to and a vector of assignment probabilities.

@section TrkCompetingRIOsOnTrack_TrkCompetingRIOsOnTrackOverview Class Overview
  The TrkCompetingRIOsOnTrack package contains the following classes:

  - Trk::CompetingRIOsOnTrack : base class for all CompetingRIOsOnTrack implementations

@section TrkCompetingRIOsOnTrack_ExtrasTrkCompetingRIOsOnTrack Extra Pages

*/

/**
*/

/**
*/
