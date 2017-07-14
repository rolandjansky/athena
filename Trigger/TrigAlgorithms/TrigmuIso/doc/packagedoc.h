/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**

@page TrigmuIso_page 
@author Stefano Giagu

@section TrigmuIso_TrigmuIsoOverview Overview
This algorithm provide isolation informations for muons at LVL2.
Both calorimetric and inner detector informations can be used. 
The muIso algorithm decode the calorimetric informations and ID 
informations in DeltaR cones around the muon directions and save 
the isolation quantities in a IsoMuonFeature object atatched to the 
event record for further analysis.



*/
