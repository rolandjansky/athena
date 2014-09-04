/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**


@mainpage TrigInterfaces Package

@section TrigSteeringInterfacesIntro Introduction

This package defines the new base classes for trigger algorithms.
The base classes from which developers must inherit their algorithms are:
 - HLT::FexAlgo:   execute on single TEs, can produce new data and attach it to processed TE;
                   must contain the most computing-intensive processing needed for trigger
		   reconstruction and selection; examples: track reconstruction, calorimeter
		   clustering, muon candidate reconstruction.
 - HLT::HypoAlgo:  activate or deactivate TEs based on existing features, but cannot create
                   TEs nor add features; must be fast, keeping calculations to a minimum; can
		   be re-run offline; examples: validate isolation hypotesis for single tracks,
		   validate ID track to calorimeter cluster matching.
 - HLT::ComboAlgo: combine TEs of one or more types into TEs of one type; run once on each
	           possible combinatorial input TE combination and create one output TE
		   for each validated one; examples: topological invariant mass reconstruction.
 - HLT::AllTEAlgo: combine TEs of one or more types into TEs of one type; run on all input TE
                   at the same time; the creation of output TEs is handled by the developer;
		   examples: missing transverse energy algorithms.


@htmlinclude used_packages.html 


*/
