/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef G4HITFILTER_H
#define G4HITFILTER_H



#include <string>
#include <vector>
#include <map>


#include "G4UserEventAction.hh"
#include "G4UserRunAction.hh"
#include "AthenaBaseComps/AthMessaging.h"

#include "StoreGate/StoreGateSvc.h"
#include "GaudiKernel/ServiceHandle.h"

namespace G4UA{

  class G4HitFilter:
    public AthMessaging, public G4UserEventAction, public G4UserRunAction
  {

  public:

    struct Config
    {
      std::vector<std::string> volumenames;
    };

    G4HitFilter(const Config& config);

    struct Report
    {
      int ntot=0;
      int npass=0;
      void merge(const Report& rep){

        ntot+=rep.ntot;
        npass+=rep.npass;

      }
    };

    const Report& getReport() const
    { return m_report; }

    virtual void EndOfEventAction(const G4Event*) override;
    virtual void BeginOfRunAction(const G4Run*) override;
  private:

    enum hitCntainerTypes {
      CALOCALIB,
      CSC,
      LAR,
      LUCID,
      MDT,
      RPC,
      SI,
      TGC,
      TILE,
      TRT };


    Config m_config;
    Report m_report;

    typedef ServiceHandle<StoreGateSvc> StoreGateSvc_t;
    /// Pointer to StoreGate (event store by default)
    mutable StoreGateSvc_t m_evtStore;
    /// Pointer to StoreGate (detector store by default)
    mutable StoreGateSvc_t m_detStore;
    std::vector<std::pair<int,std::string> > m_hitContainers;
  }; // class G4HitFilter


} // namespace G4UA




#endif
