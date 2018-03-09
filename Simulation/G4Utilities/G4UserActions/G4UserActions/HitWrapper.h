/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef G4UserActions_HitWrapper_H
#define G4UserActions_HitWrapper_H


#include "G4UserEventAction.hh"
#include "StoreGate/StoreGateSvc.h"
#include "GaudiKernel/ServiceHandle.h"
#include "AthenaBaseComps/AthMessaging.h"

namespace G4UA
{

  /// @brief NEEDS DOCUMENTATION
  class HitWrapper : public AthMessaging, public G4UserEventAction
  {
    public:

      struct Config
      {
        float time=25.;
      };

      HitWrapper(const Config& config);
      virtual void EndOfEventAction(const G4Event*) override;

    private:

      typedef ServiceHandle<StoreGateSvc> StoreGateSvc_t;
      /// Pointer to StoreGate (event store by default)
      mutable StoreGateSvc_t m_evtStore;
      /// Pointer to StoreGate (detector store by default)
      mutable StoreGateSvc_t m_detStore;

      Config m_config;

  }; // class HitWrapper

} // namespace G4UA

#endif
