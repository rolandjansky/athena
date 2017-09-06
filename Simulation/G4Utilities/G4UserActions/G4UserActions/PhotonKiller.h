/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef G4UserActions_PhotonKiller_H
#define G4UserActions_PhotonKiller_H

// Geant4 includes
#include "G4UserSteppingAction.hh"
#include "G4UserTrackingAction.hh"

namespace G4UA
{

  /// @brief NEEDS DOCUMENTATION
  class PhotonKiller : public G4UserSteppingAction, public G4UserTrackingAction
  {
    public:
      PhotonKiller();
      virtual void UserSteppingAction(const G4Step*) override final;
      virtual void PreUserTrackingAction(const G4Track*) override final;
    private:
      G4Track* m_lastTrack;
      int m_count;
      float m_energy;
  }; // class PhotonKiller

} // namespace G4UA

#endif
