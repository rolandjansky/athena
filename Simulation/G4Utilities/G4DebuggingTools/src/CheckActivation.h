/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef G4DEBUGGINGTOOLS_G4UA__CHECKACTIVATION_H 
#define G4DEBUGGINGTOOLS_G4UA__CHECKACTIVATION_H 
#include "G4AtlasInterfaces/IBeginEventAction.h"
namespace G4UA{ 

class CheckActivation: 
  public IBeginEventAction
  {
    
  public:
    CheckActivation();
    virtual void beginOfEvent(const G4Event*) override;
  private:
  }; // class CheckActivation


} // namespace G4UA 
#endif
