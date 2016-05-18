/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef G4ATLASINTERFACES__G4UA_ISTEPPINGACTION_H
#define G4ATLASINTERFACES__G4UA_ISTEPPINGACTION_H

// Forward declarations
class G4Step;

namespace G4UA
{

  /// Simple interface class for an ATLAS stepping action
  class ISteppingAction
  {
    public:
      /// Virtual destructor
      virtual ~ISteppingAction(){};
      /// Method called at every step for processing
      virtual void processStep(const G4Step*) = 0;
  };

} // namespace G4UA

#endif
