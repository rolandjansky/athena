/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef G4ATLASINTERFACES__G4UA_IBEGINEVENTACTION_H
#define G4ATLASINTERFACES__G4UA_IBEGINEVENTACTION_H

// Forward declarations
class G4Event;

namespace G4UA
{

  /// Simple interface for an ATLAS begin-of-event action
  class IBeginEventAction
  {
    public:
      /// Virtual destructor
      virtual ~IBeginEventAction(){};
      /// Method called before simulating a G4 event
      virtual void beginOfEvent(const G4Event*) = 0;
  };

} // namespace G4UA

#endif
