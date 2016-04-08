/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
@mainpage  G4AtlasControl
@author Andrea Dell'Acqua (dellacqu@mail.cern.ch)

@section G4AtlasControl Introduction

This package includes all the control interfaces from the python layer into the C++ layer of the simulation.  All the menus available to the user trhough the AtlasG4Engine are defined here (and are applied in G4AtlasApps).  

@section G4AtlasControl Class Overview

The classes in this package are:

 - ActionMenu : Controls the addition of user actions (user code that is applied before a run or event or after a run, event, or step).
 - ColorFacade : An interface for retrieving r/g/b colors
 - DataCardSvc : Best not to ask what this does
 - ElementFacade : An interface for dealing with G4Elements
 - FieldMenu : An interface to the fields applied during simulation
 - G4CommandInterface : An interface to the G4 command line (available from Python)
 - GenericDetectorFacility : An interface to the FadsDetectorFacility available from python
 - GeometryMenu : An interface to basic geometry functions in Geant4
 - MaterialFacade : An interface for dealing with G4Materials
 - MCTruthMenu : An interface for adding or applying truth strategies during the simulation
 - ParticleDataModifier : An interface for modifying particle properties during the simulation (charge, mass, etc)
 - PhysicsMenu : An interface for modifying the physics list to be used during G4 simulation
 - PhysicsRegionProxy : An interface for dealing with physics regions in G4
 - RandomNrMenu : An interface for dealing with random numbers during the simulation
 - SimControl : One interface to rule them all

@ref used_G4AtlasControl

@ref requirements_G4AtlasControl

*/

/**
@page used_G4AtlasControl Used Packages
@htmlinclude used_packages.html
*/

/**
@page requirements_G4AtlasControl Requirements
@include requirements
*/

