/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef DEBUGSTEPPINGACTION_H
#define DEBUGSTEPPINGACTION_H

#include "G4LorentzVector.hh"

#ifdef QUIRKS_STANDALONE
#include "G4UserSteppingAction.hh"
#else
//#include "FadsActions/UserAction.h"
#include "G4AtlasTools/UserActionBase.h"
#endif

class DebugSteppingAction :
#ifdef QUIRKS_STANDALONE
    public G4UserSteppingAction
#else
    public UserActionBase
#endif
{
public:
#ifdef QUIRKS_STANDALONE
    DebugSteppingAction(G4double step, G4int numSteps);
#else
    DebugSteppingAction(const std::string& type, const std::string& name, const IInterface* parent);
    virtual StatusCode queryInterface(const InterfaceID&, void**) override;
#endif

    virtual ~DebugSteppingAction();

    virtual void Step(const G4Step* step) override;
    virtual void UserSteppingAction(const G4Step* step) {Step(step);}

private:
    G4int m_iStep[2];
    G4double m_step;
    G4int m_numSteps;
    G4LorentzVector m_x[2];
    G4LorentzVector m_p[2];
    G4LorentzVector m_xprev[2];
};

#endif
