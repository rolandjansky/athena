/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
@page G4AtlasControl_page G4AtlasControl
@author Andrea Dell'Acqua (dellacqu@mail.cern.ch)

@section G4AtlasControl_G4AtlasControl Introduction

This package includes all the control interfaces from the python layer into the C++ layer of the simulation.  All the menus available to the user trhough the AtlasG4Engine are defined here (and are applied in G4AtlasApps).

@section G4AtlasControl_G4AtlasControl Class Overview

The classes in this package are:

 - G4CommandInterface : An interface to the G4 command line (available from Python)
 - MCTruthMenu : An interface for adding or applying truth strategies during the simulation
 - ParticleDataModifier : An interface for modifying particle properties during the simulation (charge, mass, etc)
 - SimControl : One interface to rule them all



*/
