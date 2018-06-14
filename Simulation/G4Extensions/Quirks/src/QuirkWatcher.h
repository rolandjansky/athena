/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef QUIRKWATCHER_H
#define QUIRKWATCHER_H

#include "G4VProcess.hh"
#include "G4VParticleChange.hh"

class QuirkWatcher : public G4VProcess {
public:
    QuirkWatcher();
    virtual ~QuirkWatcher();

    virtual G4double PostStepGetPhysicalInteractionLength(
        const G4Track& track,
        G4double   previousStepSize,
        G4ForceCondition* condition
    );
    virtual G4VParticleChange* PostStepDoIt(
        const G4Track& track,
        const G4Step& //stepData
    );

    virtual G4double AlongStepGetPhysicalInteractionLength(
        const G4Track&, //track
        G4double, //previousStepSize
        G4double, //currentMinimumStep
        G4double&, //proposedSafety
        G4GPILSelection* //selection
    ) {return -1.0;}
    virtual G4double AtRestGetPhysicalInteractionLength(
        const G4Track&, //track
        G4ForceCondition* //condition
    ) {return -1.0;}
    virtual G4VParticleChange* AlongStepDoIt(
        const G4Track&, //track
        const G4Step& //stepData
    ) {return 0;}
    virtual G4VParticleChange* AtRestDoIt(
        const G4Track&, //track
        const G4Step& //stepData
    ) {return 0;}

private:
    G4VParticleChange m_particleChange;
};

#endif
