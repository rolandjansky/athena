/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef G4ATLASTOOLS__G4UA_G4ATLASTRACKINGACTION_H
#define G4ATLASTOOLS__G4UA_G4ATLASTRACKINGACTION_H

// STL includes
#include <vector>

// Geant4 includes
#include "G4UserTrackingAction.hh"

namespace G4UA
{

  /// @class G4AtlasTrackingAction
  /// @brief Atlas subclass of the G4 tracking action.
  ///
  /// Maintains a list of custom tracking actions and when invoked by
  /// Geant4 will forard the call to each of them in turn.
  ///
  /// @todo TODO lifetime management of wrapper actions.
  ///
  /// @author Steve Farrell <Steven.Farrell@cern.ch>
  ///
  class G4AtlasTrackingAction : public G4UserTrackingAction
  {

    public:

      /// Constructor
      G4AtlasTrackingAction();

      /// @brief Geant4 method for pre-tracking action.
      /// This method forwards the G4 call onto each of its
      /// pre-tracking ATLAS actions.
      void PreUserTrackingAction(const G4Track* trk) override final;

      /// @brief Geant4 method for post-tracking action.
      /// This method forwards the G4 call onto each of its
      /// post-tracking ATLAS actions.
      void PostUserTrackingAction(const G4Track* trk) override final;

      /// @brief Add one action to the list
      void addTrackAction(G4UserTrackingAction* action);

    private:

      /// List of ATLAS actions
      std::vector<G4UserTrackingAction*> m_trackActions;

  }; // class G4AtlasTrackingAction

} // namespace G4UA

#endif
