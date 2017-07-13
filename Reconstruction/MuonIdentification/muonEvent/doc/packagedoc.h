/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**

@page muonEvent_page Package providing the Muon object for combined reconstruction and physics analysis

 - This package contains the ATLAS muon EDM class, Analysis::Muon.
   Using a flexible author/content scheme it describes the identification
   mechanism together with muon parameters and the underlying Inner Detector,
   Calorimeter and Muon Spectrometer information used for the identification.
   The class is used for ALL muon (combined) reconstruction algorithms in
   ATLAS, therefore describes standalone, combined, segment- and calo-tagged muons.
 - The Analysis::MuonContainer is the StoreGate container holding a vector of
   Analysis::Muon. 
 - It also carries the energy loss by the muon in the Calorimeter, CaloEnergy.
   The CaloEnergy carries a vector of DepositInCalo, energy deposition in the
   various calorimeter layers. 
 - This class also carries the calorimeter isolation ET and TrackIsolation
   (number of tracks) calculated in various cones of 0.1, 0.2, 0.3, 0.4, 0.45 (default),
   0.5, 0.6 and 0.7. See the muon isolation tools, 
   MuonIsolationTool and TrackIsolationTool, for further details.
 - It is the same class that is used for high Pt or low Pt reconstructions, although
   each algorithm creates its own collection of Muon in the ESD.
 - Currently the Muons in the names author collections or special ESD collections
   are made without overlap removal, therefore contain only one author per muon.
   In the persistified muon collections on ESD and AOD, overlaps are removed and
   a physical muon identified by several algorithms has a choice of primary author
   as well as the history which other authors have found this muon.
 - There is also the Muon Spectrometer Shower object, Rec::MuonSpShowerContainer,
   in the same package. The Rec::MuonSpShower object consists of the numbers of
   Muon Spectrometer Hits and numbers of  Muon::Segment in the Inner, Middle and
   Outer multi-layer, numbers calculated in some cone (default 0.7) around the
   Jet axes. 
 - The Rec::SegmentParticleContainer is a collection of Rec::SegmentParticle,
   track segments to be used in the AOD. The Rec::SegmentParticle has a link,
   ElementLink, to the original Muon::Segment, or Trk::TrackSegment from which
   the Rec::SegmentParticle was created. Rec::SegmentParticle also has pointers 
   to the Trk::GlobalDirection, the global Trk::ErrorMatrix, the Trk::FitQuality,
   the Trk::GlobalPosition, the number of Trk::RIO_OnTrack, and the IdentifierHash
   for the detector element associated to the Trk::Surface of the orginal Trk::Segment. 

 - Responsible: ATLAS Muon Combined Performance Working Group <br>
   atlas-perf-muons-conveners (at) cern.ch

@section muonEvent_ExtrasMuonEvent CMT and Dependency Description

*/
