/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
@page MCTruth_page MCTruth
@author Andrea Dell'Acqua (dellacqu@mail.cern.ch)

@section MCTruth_MCTruth Introduction

This package provides many helper classes for dealing with Monte Carlo Truth.  Most of it is "core" simulation code and can be safely ignored by users.  This is where the truth really comes from.

@section MCTruth_MCTruth Class Overview

The MCTruth package includes the following classes:

 - AtlasTrajectory : An ATLAS-specific Geant4 trajectory, used as a helper (i.e. including some additional information for us)
 - CosmicTRSD : The track recording sensitive detector used by the cosmic simulation.  This sensitive detector stores cosmic rays at the entrance to the cavern so that they can be resimulated beginning from that point later on.
 - AtlasG4EventUserInfo : The ATLAS implementation of G4UserEventInformation.  Includes some helpful information about the event.
 - MCTruthSteppingAction : A piece of code that tests, at every step, whether a particle (or vertex, more accurately) is to be saved in the truth tree.  It decides based on the Truth Strategies enabled in the TruthStrategyManager
 - PrimaryParticleInformation : The ATLAS implementation of user information for a particle.  This stores additional information for those particles coming from the generator for use later in the simulation.
 - TrackHelper : Provides several useful functions for dealing with G4Tracks
 - TrackInformation : The ATLAS implementation of user track information.  Provides some ATLAS-specific information for each track.
 - TrackRecorderSD : A sensitive detector which stores tracks as they pass through a volume in the form of Track Records.  This is how, for example, all tracks are stored as they exit the inner detector.
 - TruthController : The master truth controler for the simulation.  Particularly useful for visualization.
 - TruthEvent : An interface to the GenEvent (truth event)
 - TruthStrategy : The base class for truth strategies implemented in G4TruthStrategies
 - TruthStrategyManager : The manager that keeps track of the truth strategies and where they are to be applied.
 - TruthStrategyManagerMessenger : An obsolete messenger for the TruthStrategyManager



*/
