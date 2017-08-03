/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef G4UserActions_CosmicPerigeeAction_H
#define G4UserActions_CosmicPerigeeAction_H

#include "StoreGate/WriteHandle.h"
#include "TrackRecord/TrackRecordCollection.h" // Can't be forward declared - it's a type def

#include "G4UserSteppingAction.hh"
#include "G4UserEventAction.hh"
#include "G4UserTrackingAction.hh"

#include "CLHEP/Units/SystemOfUnits.h"

namespace G4UA
{

  /// @brief NEEDS DOCUMENTATION
  class CosmicPerigeeAction final : public G4UserSteppingAction,
                                    public G4UserEventAction,
                                    public G4UserTrackingAction
  {

  public:

    struct Config
    {
      float pMinPrimary = 100*CLHEP::MeV;
    };

    CosmicPerigeeAction(const Config& config);

    virtual void UserSteppingAction(const G4Step*) override;
    virtual void EndOfEventAction(const G4Event*) override;
    virtual void BeginOfEventAction(const G4Event*) override;
    virtual void PreUserTrackingAction(const G4Track*) override;

  private:

    /// Configuration options
    Config m_config;

    /// Output track track record
    SG::WriteHandle<TrackRecordCollection> m_trackRecordCollection;

    /// State members
    bool m_hasBeenSaved;
    double m_idZ;
    double m_idR;

  }; // class CosmicPerigeeAction

} // namespace G4UA

#endif
