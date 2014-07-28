/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**

@mainpage
@author Stefano Giagu

@section TrigmuIsoOverview Overview 
This algorithm provide isolation informations for muons at LVL2.
Both calorimetric and inner detector informations can be used. 
The muIso algorithm decode the calorimetric informations and ID 
informations in DeltaR cones around the muon directions and save 
the isolation quantities in a IsoMuonFeature object atatched to the 
event record for further analysis.

@ref used_TrigmuIso

@ref requirements_TrigmuIso

*/

/**
@page used_TrigmuIso Used Packages
@htmlinclude used_packages.html
*/

/**
@page requirements_TrigmuIso Requirements
@include requirements
*/
