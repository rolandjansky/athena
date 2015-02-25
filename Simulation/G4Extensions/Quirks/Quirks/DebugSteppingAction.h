/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef DEBUGSTEPPINGACTION_H
#define DEBUGSTEPPINGACTION_H

#include "G4LorentzVector.hh"

#ifdef QUIRKS_STANDALONE
#include "G4UserSteppingAction.hh"
#else
#include "FadsActions/UserAction.h"
#endif

class DebugSteppingAction :
#ifdef QUIRKS_STANDALONE
    public G4UserSteppingAction
#else
    public FADS::UserAction
#endif
{
public:
    DebugSteppingAction(G4double step, G4int numSteps);
    virtual ~DebugSteppingAction();
    virtual void SteppingAction(const G4Step* step);
    virtual void UserSteppingAction(const G4Step* step) {SteppingAction(step);}
private:
    G4int m_iStep[2];
    G4double m_step;
    G4int m_numSteps;
    G4LorentzVector m_x[2];
    G4LorentzVector m_p[2];
    G4LorentzVector m_xprev[2];
};

#endif
