/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef G4COSMICFILTER_G4CosmicFilter_H
#define G4COSMICFILTER_G4CosmicFilter_H

#include "G4UserEventAction.hh"
#include "AthenaBaseComps/AthMessaging.h"

namespace G4UA
{

  /// @class G4CosmicFilter
  /// an action to query the SD responsible for the storing of the
  /// TrackRecords *at the entrance of the ID* if no track it aborts
  /// the event - jamie boyd 15 nov 06
  class G4CosmicFilter: public AthMessaging, public G4UserEventAction
  {

  public:

    struct Config
    {
      std::string collectionName = "CaloEntryLayer";
      int PDGId = 0;
      double ptMin = -1;
      double ptMax = -1;
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

    G4CosmicFilter(const Config& config);

    const Report& getReport() const
    { return m_report; }

    virtual void EndOfEventAction(const G4Event*) override;

  private:

    Config m_config;
    Report m_report;

  }; // class G4CosmicFilter

} // namespace G4UA

#endif
