/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
@page InDetCompetingRIOsOnTrack_page The InDetCompetingRIOsOnTrack package
The InDet package for CompetingRIOsOnTrack.

@author Sebastian.Fleischmann@cern.ch

@section InDetCompetingRIOsOnTrack_introductionInDetCompetingRIOsOnTrack Introduction

This package contains the InDet classes which inherit from Trk::CompetingRIOsOnTrack that extends 
the Trk::MeasurementBase class.

These classes are used e.g. by the Deterministic Annealing Filter to
handle several RIO_OnTracks in one "effective detector layer"
(might be more than one DetectorElement), which compete against
each other in being assigned to a track.
localParameters() and localErrorMatrix() return the mean values
according to the weights (assignment probabilities).

@section InDetCompetingRIOsOnTrack_InDetCompetingRIOsOnTrackOverview Class Overview
  The InDetCompetingRIOsOnTrack package contains the following classes:
  - InDet::CompetingPixelClustersOnTrack : Pixel specific implementation of CompetingRIOsOnTrack with InDet::PixelClusterOnTrack
  - InDet::CompetingSCT_ClustersOnTrack : SCT specific implementation of CompetingRIOsOnTrack with InDet::SCT_ClusterOnTrack
  - InDet::CompetingTRT_DriftCirclesOnTrack : TRT specific implementation of CompetingRIOsOnTrack with InDet::TRT_DriftCircleOnTrack

@section InDetCompetingRIOsOnTrack_ExtrasInDetCompetingRIOsOnTrack Extra Pages

*/
