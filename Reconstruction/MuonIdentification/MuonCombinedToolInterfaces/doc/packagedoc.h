/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
@page MuonCombinedToolInterfaces_page MuonCombinedToolInterfaces Package

This package contains abstract interfaces for two types of common tool
in combined muon reconstruction: interfaces for modules of the
(non-existent) common baseline reco chain and interfaces for tools to
evaluate a combined muon.
Concrete implementations can be found in each leaf package.

@author ATLAS Muon Combined Performance Group

@section MuonCombinedToolInterfaces_MuonCombinedToolInterfacesList List of Defined Interfaces

Abstract interfaces for the following combined reconstruction tasks are defined:

@htmlinclude annotated.html

   - MuonCombined::ICombinedTrackTagTool : what does it do?
   - MounCombined::IMuonTrackTagTool     : what does it do?

Abstract interfaces for the following muon evaluation tasks are defined:

   - Rec::IMuonPropertiesTool : Interface providing uniform access methods to internal properties of analysis muons 
   - Rec::IMuonOverlapTool    : Interface for calculating if two independently reconstructed muons overlap, that is share hits, and thus possibly stem from the same muon
   - Rec::IMuonCombinedSelectorTool : Interface providing a yes/no decision if an analysis muon is selected 
   - Rec::IMuonScatteringAngleSignificance : Interface providing discriminators against decays in flight
   - Rec::IMuonMomentumBalanceSignificance : Interface providing discriminators against decays in flight and other BG
   - Rec::IMuonCombinedTimingTool : Interface for extracting and calculating the time measurements along a Muon trajectory
   - Rec::IMuonOverlapResolverTool : Interface for fusioning overlapping muons, as used by the muon builder's algorithms 

This package is not built as any library, it is a simple include package.   

@section MuonCombinedToolInterfaces_ExtrasMuCombToolInterfaces Extra Pages

*/
