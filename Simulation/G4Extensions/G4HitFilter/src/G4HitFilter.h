/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef G4HITFILTER_H
#define G4HITFILTER_H


#include "G4AtlasTools/UserActionBase.h"

#include <string>
#include <vector>
#include <map>

class G4HitFilter final: public UserActionBase {

 public:
  G4HitFilter(const std::string& type, const std::string& name, const IInterface* parent);

  virtual void EndOfEvent(const G4Event*) override;
  virtual StatusCode initialize() override;
  virtual StatusCode finalize() override;

  virtual StatusCode queryInterface(const InterfaceID&, void**) override;

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

  // property on python side
  std::vector<std::string> m_volumenames;

  // internals
  int m_ntot,m_npass;
  std::vector<std::pair<int,std::string> > m_hitContainers;
};


#include "G4AtlasInterfaces/IEndEventAction.h"
#include "G4AtlasInterfaces/IBeginRunAction.h"
#include "AthenaBaseComps/AthMessaging.h"

#include "StoreGate/StoreGateSvc.h"
#include "GaudiKernel/ServiceHandle.h"

namespace G4UA{

  class G4HitFilter:
    public AthMessaging, public IEndEventAction,public IBeginRunAction
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

    virtual void endOfEvent(const G4Event*) override;
    virtual void beginOfRun(const G4Run*) override;
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
