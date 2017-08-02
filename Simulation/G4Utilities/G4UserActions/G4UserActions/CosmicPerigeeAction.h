/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef G4UserActions_CosmicPerigeeAction_H
#define G4UserActions_CosmicPerigeeAction_H

#include "StoreGate/WriteHandle.h"
#include "TrackRecord/TrackRecordCollection.h" // Can't be forward declared - it's a type def

#include "G4AtlasInterfaces/ISteppingAction.h"
#include "G4AtlasInterfaces/IBeginEventAction.h"
#include "G4AtlasInterfaces/IEndEventAction.h"
#include "G4AtlasInterfaces/IPreTrackingAction.h"

#include "CLHEP/Units/SystemOfUnits.h"

namespace G4UA
{

  /// @brief NEEDS DOCUMENTATION
  class CosmicPerigeeAction final : public ISteppingAction, public IEndEventAction,
                                    public IBeginEventAction, public IPreTrackingAction
  {

  public:

    struct Config
    {
      float pMinPrimary = 100*CLHEP::MeV;
    };

    CosmicPerigeeAction(const Config& config);

    virtual void processStep(const G4Step*) override;
    virtual void endOfEvent(const G4Event*) override;
    virtual void beginOfEvent(const G4Event*) override;
    virtual void preTracking(const G4Track*) override;

  private:

    Config m_config;
    SG::WriteHandle<TrackRecordCollection> m_trackRecordCollection;
    bool m_hasBeenSaved;
    double m_idZ, m_idR;

  }; // class CosmicPerigeeAction

} // namespace G4UA

#endif
