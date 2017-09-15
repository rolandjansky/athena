/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
@page TrkMeasurementBase_page The MeasurementBase class

@section TrkMeasurementBase_introductionTrkMeasurementBase Introduction

This package includes the pure absract base class Trk::MeasurementBase 
for all fittable measurements in the Tracking realm. 
It holds the minimal common interface which is characterized by

   - Trk::LocalParameters*
   - Trk::ErrorMatrix*
   - Trk::Surface*
   - Trk::GlobalPosition*

The following child branches are (planned) to inherit from this one (only base classes are given here):

   - Trk::RIO_OnTrack, calibrated measurement on track
   - Trk::Segment, track segment class with calibrated measurements on track
   - Trk::CompetingRIO_OnTrack, data structure for DAF with multiple competing RIO_OnTracks
   - Trk::SpacePoint, 3D space points to allow fitting on HLT level (planned)
   
Please let me know of any errors, or if anything is unclear.
@author Andreas.Salzburger@cern.ch


@namespace Trk
A namespace to enclose the Tracking classes.
*/
