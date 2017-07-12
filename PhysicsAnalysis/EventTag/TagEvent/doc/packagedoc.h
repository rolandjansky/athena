/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**

@page TagEvent_page TagEvent Package

The package which defines the TAG data classes. It the EDM package for the TAG.

@authors Ketevi Assamagan

This is the EDM package for the TAG. It consists of classes that represent the data objects of the various tag 
fragments. These tag fragments are to be built by the various tag building tools from the AOD. Then the 
fragments are aggregated by the tag builder algorithms, which then produce the event TAG. This package 
consists of the following classes - just 2 EDM classes for everythings:

 - TagFragmentCollection (a Tag Fragment is a TAG attribute and its value) for:

 - Electron tag fragments and Electron tag fragment Collection. 
 - Photon tag fragments and Photon tag Collection.
 - Muon tag fragments and Muon tag Collection.
 - TauJet tag fragments and TauJet tag Collection.
 - ParticleJet tag fragments and ParticleJet tag Collection.
 - MissingET tag fragments and Collection. 
 - Various detector status words and the Good Run bit.
 - Global event related information, including the time stamp and the streaming criteria and results.
 - Global trigger information: CTP decisions, level 1 passed trigger masks, level 2 passed trigger 
masks, event filter passed trigger masks, and high level trigger stuff.
 - Analysis specific information: here n (n=1 for now)  words are reserved in the TAG for each combined 
performance and physics group to encode whatever information they deem necessary. For example, the 
b-physics group may encode the results of various hypotheses about exclusive processes into their tag word.

 - For questions or comments: Ketevi A. Assamagan
   ketevi@bnl.gov

*/
