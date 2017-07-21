/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
@page FadsControl_page FadsControl
@author Andrea Dell'Acqua (dellacqu@mail.cern.ch)

@section FadsControl_FadsControl Introduction

This package includes several useful base classes that are used throughout the simulation.

@section FadsControl_FadsControl Class Overview

This package's classes are logically divided in the following way:

 - DetectorConstructionEntryBase , PhysicsListEntryBase , PrimaryGeneratorEntryBase , RunManagerEntryBase , UserActionEntryBase : Pure athena base classes for the corresponding G4 objects
 - DetectorConstructionEntry , PhysicsListEntry , PrimaryGeneratorEntry , RunManagerEntry , UserActionEntry : Inherit from the pure-athena base classes and add support for the Geant4 object (usually through members rather than through inheritance)
 - DetectorDef , PhysicsListDef , PrimaryGeneratorDef , RunManagerDef , UserActionDef : Definitions for the classes (only C++ definitions)
 - DetectorConstructionManager , PhysicsListManager , PrimaryGeneratorActionManager , RunManagerStore , UserActionManager : Logical managers for keeping track of the defined objects and which are activated

These individual classes are what one would expect.  DetectorConstruction* deal with engines for constructing the detector; PhysicsList* deal with G4PhysicsLists; PrimaryGenerator* deal with G4PrimaryGenerators, RunManager* deal with G4RunManagers (note that there should only be one active at a time, which is why the manager is called a "store"); UserAction* deal with G4UserActions.

Three other classes are included in this package:

 - RandomNrCenter : Defines the random number generator to be used, including seeds, for Geant4 (essentially an interface for HepRandomEngines)
 - FadsActionBase : A generic action that includes all the G4 action types (stepping actions, tracking actions, run actions, stacking actions).  All other action types can inherit from this one and select the methods they wish to implement.
 - SteppingVerboseDef : An interface for G4 stepping verbose within the athena framework



*/
