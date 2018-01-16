/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef G4DEBUGGINGTOOLS_G4UA__CHECKACTIVATION_H 
#define G4DEBUGGINGTOOLS_G4UA__CHECKACTIVATION_H 
#include "G4UserEventAction.hh"
namespace G4UA{ 

class CheckActivation: 
  public G4UserEventAction
  {
    
  public:
    CheckActivation();
    virtual void BeginOfEventAction(const G4Event*) override;
  private:
  }; // class CheckActivation


} // namespace G4UA 
#endif
