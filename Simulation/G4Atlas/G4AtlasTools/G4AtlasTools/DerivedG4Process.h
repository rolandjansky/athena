/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef G4MyProcess_h
#define G4MyProcess_h

#include "G4VProcess.hh"

class G4MyProcess : public G4VProcess {

 public:
  
  G4MyProcess( G4String& aName, G4ProcessType aType ) : G4VProcess( aName, aType )
  { 
    
  }

  ~G4MyProcess() {}

  G4VParticleChange* PostStepDoIt(
			     const G4Track& track,
			     const G4Step&  stepData
			    ) final
  {
    (void)track; //just for silence UNUSED warning! The same below
    (void)stepData;
    return nullptr;
  }

  G4VParticleChange* AlongStepDoIt(
			     const G4Track& track,
			     const G4Step& stepData
			    ) final
  {
    (void)track;
    (void)stepData;
    return nullptr;
  }

  G4VParticleChange* AtRestDoIt(
			     const G4Track& track,
			     const G4Step& stepData
			    ) final
  {
    (void)track;
    (void)stepData;
    return nullptr;
  }

  G4double AlongStepGetPhysicalInteractionLength(
                             const G4Track& track,
			     G4double  previousStepSize,
			     G4double  currentMinimumStep,
			     G4double& proposedSafety,
                             G4GPILSelection* selection) final
  {
    (void)track;
    (void)previousStepSize;
    (void)currentMinimumStep;
    (void)proposedSafety;
    (void)selection;
    return 0.0;
  }

  G4double AtRestGetPhysicalInteractionLength(
                             const G4Track& track,
			     G4ForceCondition* condition
			    ) final
  {
    (void)track;
    (void)condition;
    return 0.0;
  }

  G4double PostStepGetPhysicalInteractionLength(
                             const G4Track& track,
			     G4double   previousStepSize,
			     G4ForceCondition* condition
			    ) final
  {
    (void)track;
    (void)previousStepSize;
    (void)condition;
    return 0.0;
  }
};

#endif

