/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
@mainpage

@section introductionMuonROT Introduction

This package provides Muon specific implementations of the Trk::RIO_OnTrack class, namely:
    - Muon::MuonClusterOnTrack - base class for technology-specific cluster implementations
       - Muon::CscClusterOnTrack
       - Muon::RpcClusterOnTrack
       - Muon::TgcClusterOnTrack
    - Muon::MdtDriftCircleOnTrack - drift tube calibrated measurement
   

To produce these classes you should not attempt to create them directly, but should instead 
use the RIOOnTrackCreators(i.e. tools which implement the Trk::IRIO_OnTrackCreator interface). 

For the Muons, the specific implementations are:
   - Muon::MdtDriftCircleOnTrackCreator  	
   - Muon::MuonClusterOnTrackCreator  	

@see Trk::RIO_OnTrack
@see Trk::IRIO_OnTrackCreator
@see Muon::MdtDriftCircleOnTrackCreator
@see Muon::MuonClusterOnTrackCreator


@htmlinclude used_packages.html

@include requirements

*/
