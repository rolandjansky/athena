/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**

@mainpage JetInterface

@author D. Adams and P-A. Delsart

@section Content

Package JetInterface (updated for run 2) contains the interfaces for the tools used in
jet reconstruction.
These are dual-use tools so that reconstruction can be run in either Athena or Root.

@section Main classes
  - IJetExecuteTool : Interface for an algorithm-like tool.
  - IJetFinder : Interface for finding jets.
  - IJetGroomer : Interface for grooming a jet.
  - IJetFromPseudojet : Interface for constructing a jet from a pseudojet.
  - IJetModifier : Interface for modifying a jet collection.
  - ISingleJetModifier : Interface for modfying a single jet.
  - IPseudoJetGetter : Interface for retriving a vector of pseudojets.
  - IFastJetInterfaceTool: Interface to fastjet wrapper.

@htmlinclude used_packages.html

@include requirements

*/
