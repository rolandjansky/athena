/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef DEBUGSTEPPINGACTION_H
#define DEBUGSTEPPINGACTION_H

#include "G4LorentzVector.hh"

#ifdef QUIRKS_STANDALONE
#include "G4UserSteppingAction.hh"
#else
#include "G4AtlasInterfaces/ISteppingAction.h"
#endif

#ifndef QUIRKS_STANDALONE
namespace G4UA{
#endif

class DebugSteppingAction :
#ifdef QUIRKS_STANDALONE
    public G4UserSteppingAction
#else
    public ISteppingAction
#endif
{
public:
#ifdef QUIRKS_STANDALONE
    DebugSteppingAction(G4double step, G4int numSteps);
#else
    struct Config
    {
      G4double step=0;
      G4int numSteps=0;
    };
    
    DebugSteppingAction(const Config& config);    
#endif

    virtual ~DebugSteppingAction();

    virtual void processStep(const G4Step* step) override;
    virtual void UserSteppingAction(const G4Step* step) {processStep(step);}

private:
#ifndef QUIRKS_STANDALONE
    Config m_config;
#else
    G4double m_step;
    G4int m_numSteps;
#endif
    
    G4int m_iStep[2];
 
    G4LorentzVector m_x[2];
    G4LorentzVector m_p[2];
    G4LorentzVector m_xprev[2];
};



#ifndef QUIRKS_STANDALONE
} // namespace G4UA
#endif

#endif
