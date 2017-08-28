/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
@page TrkSegment_page The Segment class

@section TrkSegment_introductionTrkSegment Introduction

The base class package for Track Segments.
This package contains the base class for all track segments (Trk::Segment) that extens 
the Trk::MeasurementBase class.

Track segments are characterized by a vector of Trk::RIO_OnTrack objects they hold, 
by a Surface the segment is expressed to (typically for the 2nd or 3rd coordinate constraint)
and a FitQuality.

In addition, this package holds the simplest possible, generic track segment that contains \
polymorphic Trk::RIO_OnTrack objects stored through base class pointers.


Please let me know of any errors, or if anything is unclear.
@author Andreas.Salzburger@cern.ch


@namespace Trk
A namespace to enclose the Tracking classes.
*/
