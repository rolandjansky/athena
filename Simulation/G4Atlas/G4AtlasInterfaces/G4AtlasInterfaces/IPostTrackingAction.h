/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef G4ATLASINTERFACES__G4UA_IPOSTTRACKINGACTION_H
#define G4ATLASINTERFACES__G4UA_IPOSTTRACKINGACTION_H

// Forward declarations
class G4Track;

namespace G4UA
{

  /// Simple interface for an ATLAS post-tracking action
  class IPostTrackingAction
  {
    public:
      /// Virtual destructor
      virtual ~IPostTrackingAction(){};
      /// Method called before tracking a particle
      virtual void postTracking(const G4Track*) = 0;
  };

} // namespace G4UA

#endif
