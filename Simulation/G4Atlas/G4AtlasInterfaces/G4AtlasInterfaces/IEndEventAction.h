/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef G4ATLASINTERFACES__G4UA_IENDEVENTACTION_H
#define G4ATLASINTERFACES__G4UA_IENDEVENTACTION_H

// Forward declarations
class G4Event;

namespace G4UA
{

  /// Simple interface for an ATLAS end-of-event action
  class IEndEventAction
  {
    public:
      /// Virtual destructor
      virtual ~IEndEventAction(){};
      /// Method called after simulating a G4 event
      virtual void endOfEvent(const G4Event*) = 0;
  };

} // namespace G4UA

#endif
