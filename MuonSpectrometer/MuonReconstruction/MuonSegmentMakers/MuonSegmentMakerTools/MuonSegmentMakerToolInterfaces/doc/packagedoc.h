/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**

@page MuonSegmentMakerToolInterfaces_page MuonRecToolInterfaces Package

@author Niels.van.Eldik@cern.ch 

@section MuonSegmentMakerToolInterfaces_DescriptionMuonRecToolInterfaces General description

The MuonRecToolInterfaces is a packages providing pure virtual interfaces needed in the muon reconstruction.
The following interface are available:
- Muon::IMdtDriftCircleOnTrackCreator, interface for tools creating Muon::MdtDriftCircleOnTrack objects
- Muon::IMuonClusterOnTrackCreator, interface for tools creating Muon::MuonClusterOnTrack objects
- Muon::IMuonCompetingClustersOnTrackCreator, interface for tools creating Muon::CompetingMuonClustersOnTrack objects
- Muon::IMuonCombinePatternTool, interface for tools that combine Muon::MuonPattern objects 
- Muon::IMuonHitSelector, interface for tools selecting a subset of hits from a list of hits
- Muon::IMuonPatternFinder, interface for tools creating Muon::MuonPattern objects from a set of Trk::PrepRawData objects
- Muon::IMuonSegmentCleaner, interface for tools resolving hit ambigueties on segments 
- Muon::IMuonSegmentMaker, interface for tools finding segments from a set of Trk::RIO_OnTrack objects
- Muon::IMuonSegmentMomentumEstimator, interface for tools estimating the mometum of a muon from a list of Muon::MuonSegment objects
- Muon::IPRDHitsInConeTools, interface for tools to find hits in the region of a muon track

@section MuonSegmentMakerToolInterfaces_ExtrasMuonRecToolInterfaces Extra Pages

*/
