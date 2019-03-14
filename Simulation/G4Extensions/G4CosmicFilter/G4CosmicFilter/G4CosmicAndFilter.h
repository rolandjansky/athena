/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef G4COSMICFILTER_G4CosmicAndFilter_H
#define G4COSMICFILTER_G4CosmicAndFilter_H

#include "G4UserEventAction.hh"
#include "AthenaBaseComps/AthMessaging.h"

namespace G4UA
{

  /// @class G4CosmicAndFilter
  /// an action to query the SD responsible for the storing of the
  /// TrackRecords *at the entrance of the ID* if no track it aborts
  /// the event - jamie boyd 15 nov 06
  ///
  class G4CosmicAndFilter: public AthMessaging, public G4UserEventAction
  {

    public:

      struct Config
      {
        std::string collectionName = "TRTBarrelEntryLayer";
        std::string collectionName2 = "CaloEntryLayer";
      };

      struct Report
      {
        int ntot = 0;
        int npass = 0;
        void merge(const Report& rep){
          ntot += rep.ntot;
          npass += rep.npass;
        }
      };

      G4CosmicAndFilter(const Config& config);

      const Report& getReport() const
      { return m_report; }

      virtual void EndOfEventAction(const G4Event*) override;

    private:

      Config m_config;
      Report m_report;

  }; // class G4CosmicFilter

} // namespace G4UA

#endif
