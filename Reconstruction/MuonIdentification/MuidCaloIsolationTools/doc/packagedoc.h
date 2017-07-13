/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
@page MuidCaloIsolationTools_page MuidCaloIsolationTools Package

contains the @ref Rec::MuidTrackIsolation tool for estimating the number and summed momentum of inner detector tracks in a given cone around the muon track i.e. the track isolation of the muon. The isolation can be evaluated either at the production vertex or around the muon intersection with the calorimeter.

Track isolation is used by the @ref MuidCaloEnergyTool (MuidCaloEnergyTools package) to determine the reliability of the calorimeter measurement of the muon energy loss. Simulation results indicate that production vertex isolation is the better choice - possibly through better accounting of any neutral particle contribution.

@author Alan.Poppleton@cern.ch
@author Konstantinos.Nikolopoulos@cern.ch


 @namespace Rec namespace for combined reconstruction tools and interfaces
 @namespace Trk namespace for general tracking tools and interfaces

*/
