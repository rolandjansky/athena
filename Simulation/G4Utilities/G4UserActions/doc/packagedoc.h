/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
@page G4UserActions_page G4UserActions
@author Andrea.Di.Simone@cern.ch, Andrea.Dellacqua@cern.ch

@section G4UserActions_G4UserActionsIntro Introduction

This package includes various utility classes for Geant4 simulation.  Some are always activated by 
default, and some are for very specific cases.

@section G4UserActions_G4UserActionsOverview Class Overview
  The G4UserActions package contains of following classes:

  - FastIDKiller : Removes some particles prior to their entry into the calorimeter

  - G4SimMem : Prints memory information for the running job

  - G4SimTimer : Prints time spent information for the running job

  - G4TrackCounter : Counts the tracks for each event, produces average output

  - Interaction/RadLengthIntegrator : Integrates interaction lengths or radiation lengths
	at the particles eta.  Use with geantinos (charged or neutral) to map the detectors

  - StepNtuple : Records the steps of each event into an ntuple.  HEAVY - do not use for normal simulation

  - TestAction : Sample user test actions

  - TimeCutAction : Example test action for killing particles after a certain physics time has elapsed

  - VerboseSelector : Sets the verbosity level for several Geant actions



*/
