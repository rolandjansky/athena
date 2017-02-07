/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**

@page JetRec_page JetRec

@author D. Adams and P.A. Delsart

@section JetRec_Content 

Package JetRec (updated for run 2) contains the high-level tools used in jet reconstruction.
These are dual-use tools so that reconstruction can be run in either Athena or Root.
A top-level Athena algorithm is also provided.

@section JetRec_Main classes
  - JetAlgorithm : This Athena algorithm runs a series of JetRec tools.
  - JetRecTool : Each jet collection is produced by a dedicated JetRec tool (i.e. instance of JetRecTool).
  - PseudoJetGetter : Inputs for jet finding, in the form of vectors of fastjet pseudojets, are accessed
and constructed with tools of this type.
  - JetFromPseudojet: This tool constructs an ATLAS jet collection from a vector of fastjet pseudojets.
  - JetFinder : This tool uses fastjet to find jets using an input list of pseudojets.



*/
