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
			    )
  {
//    G4Track a = track;
//    G4Step b = stepData;
    
    return nullptr;
  }

  G4VParticleChange* AlongStepDoIt(
			     const G4Track& track,
			     const G4Step& stepData
			    )
  {
//    G4Track a = track;
//    G4Step b = stepData;

    return nullptr;
  }

  G4VParticleChange* AtRestDoIt(
			     const G4Track& track,
			     const G4Step& stepData
			    )
  {
//    G4Track a = track;
//    G4Step b = stepData;

    return nullptr;
  }

  G4double AlongStepGetPhysicalInteractionLength(
                             const G4Track& track,
			     G4double  previousStepSize,
			     G4double  currentMinimumStep,
			     G4double& proposedSafety,
                             G4GPILSelection* selection)
  {
//    G4Track a = track;
//    G4double c = previousStepSize;
//    G4double d = currentMinimumStep;
//    G4double e = proposedSafety;
//    G4GPILSelection* f = selection;
    return 0.0;
  }

  G4double AtRestGetPhysicalInteractionLength(
                             const G4Track& track,
			     G4ForceCondition* condition
			    )
  {
//    G4Track a = track;
//    G4ForceCondition* b = condition;
    return 0.0;
  }

  G4double PostStepGetPhysicalInteractionLength(
                             const G4Track& track,
			     G4double   previousStepSize,
			     G4ForceCondition* condition
			    )
  {
//    G4Track a = track;
//    G4double b = previousStepSize;
//    G4ForceCondition* c = condition;
    return 0.0;
  }
};

#endif

