/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef G4COSMICFILTER_G4CosmicOrFilter_H
#define G4COSMICFILTER_G4CosmicOrFilter_H

#include "G4UserEventAction.hh"
#include "AthenaBaseComps/AthMessaging.h"

#include "StoreGate/StoreGateSvc.h"
#include "GaudiKernel/ServiceHandle.h"

namespace G4UA
{

  /// @class G4CosmicOrFilter
  /// an action to query the SD responsible for the storing of the
  /// TrackRecords *at the entrance of the ID* if no track it aborts
  /// the event - jamie boyd 15 nov 06
  class G4CosmicOrFilter : public AthMessaging, public G4UserEventAction
  {

    public:

      struct Config
      {
        std::string collectionName = "TRTBarrelEntryLayer";
        std::string collectionName2 = "CaloEntryLayer";
        std::string collectionName3 = "TRTBarrelEntryLayer";
      };

      G4CosmicOrFilter(const Config& config);

      struct Report
      {
        int ntot = 0;
        int npass = 0;
        void merge(const Report& rep){
          ntot += rep.ntot;
          npass += rep.npass;
        }
      };

      const Report& getReport() const
      { return m_report; }

      virtual void EndOfEventAction(const G4Event*) override;

    private:

      Config m_config;
      Report m_report;

      typedef ServiceHandle<StoreGateSvc> StoreGateSvc_t;
      /// Pointer to StoreGate (event store by default)
      mutable StoreGateSvc_t m_evtStore;
      /// Pointer to StoreGate (detector store by default)
      mutable StoreGateSvc_t m_detStore;

  }; // class G4CosmicOrFilter

} // namespace G4UA

#endif
