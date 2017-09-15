/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**

@page MdtDriftCircleOnTrackCreator_page 

@section MdtDriftCircleOnTrackCreator_MdtDriftCircleOnTrackCreatorDocIntro Overview

This package provides a tool, Muon::MdtDriftCircleOnTrackCreator, which turns
Muon::MdtPrepData into Muon::MdtDriftCircleOnTrack objects
using a given trajectory prediction. 

The tool calls the MDT calibration service (MdtCalibrationSvc). This 
service provides the same functionality which would usually be foreseen
for this tool, i.e. it computes and corrects the drift radius using the
track prediction and calibrates its error estimate.

More general information on RIO_OnTrackCreation and instructions
how to use these tools can be found under Trk::RIO_OnTrackCreator.


@section MdtDriftCircleOnTrackCreator_MdtDriftCircleOnTrackCreatorDocMgr Package Managers
The Muon::MdtDriftCircleOnTrackCreator package is currently maintained by Niels van Eldik

*/
