/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef G4UserActions_HitWrapper_H
#define G4UserActions_HitWrapper_H

#include "G4AtlasTools/UserActionBase.h"


#include <string>

class HitWrapper final: public UserActionBase {

  public:
  HitWrapper(const std::string& type, const std::string& name, const IInterface* parent);

  virtual void EndOfEvent(const G4Event*) override;

  virtual StatusCode queryInterface(const InterfaceID&, void**) override;

  private:

  double m_time; //!< Time over which to wrap the hits



};

#include "G4AtlasInterfaces/IEndEventAction.h"
#include "StoreGate/StoreGateSvc.h"
#include "GaudiKernel/ServiceHandle.h"
#include  "AthenaBaseComps/AthMessaging.h"

namespace G4UA
{

  /// @brief NEEDS DOCUMENTATION
  class HitWrapper : public AthMessaging, public IEndEventAction
  {
    public:

      struct Config
      {
        float time=25.;
      };

      HitWrapper(const Config& config);
      virtual void endOfEvent(const G4Event*) override;

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
