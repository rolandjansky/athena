/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef G4ATLASINTERFACES__G4UA_IPRETRACKINGACTION_H
#define G4ATLASINTERFACES__G4UA_IPRETRACKINGACTION_H

// Forward declarations
class G4Track;

namespace G4UA
{

  /// Simple interface for an ATLAS pre-tracking action
  class IPreTrackingAction
  {
    public:
      /// Virtual destructor
      virtual ~IPreTrackingAction(){};
      /// Method called before tracking a particle
      virtual void preTracking(const G4Track*) = 0;
  };

} // namespace G4UA

#endif
