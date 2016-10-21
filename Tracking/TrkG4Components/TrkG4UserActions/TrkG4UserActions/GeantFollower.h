/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// GeantFollower.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef GeantFollower_H
#define GeantFollower_H

#include "G4AtlasTools/UserActionBase.h"
#include "GaudiKernel/ToolHandle.h"
#include "TrkG4UserActions/IGeantFollowerHelper.h"
#include <string>
#include <vector>

/** @class GeantFollower

    @author Andreas.Salzburger@cern.ch
*/

namespace Trk {
    class IGeantFollowerHelper;
}

class StoreGateSvc;

class GeantFollower final: public UserActionBase {

  public:
    /** Standard UsesAction */
    GeantFollower(const std::string& type, const std::string& name, const IInterface* parent);

    /** All G4 interface methods */
    virtual void BeginOfEvent(const G4Event*) override;
    virtual void EndOfEvent(const G4Event*) override;
    virtual void Step(const G4Step*) override;

    virtual StatusCode queryInterface(const InterfaceID&, void**) override;
    virtual StatusCode initialize() override;

  private:
    std::string                                  m_name;
    ToolHandle<Trk::IGeantFollowerHelper>     m_helper;
    mutable const Trk::IGeantFollowerHelper*  m_helperPointer;
    

};


#include "G4AtlasInterfaces/IBeginEventAction.h"
#include "G4AtlasInterfaces/IEndEventAction.h"
#include "G4AtlasInterfaces/IBeginRunAction.h"
#include "G4AtlasInterfaces/ISteppingAction.h"
#include "AthenaBaseComps/AthMessaging.h"
namespace G4UA{


  class GeantFollower:
  public AthMessaging, public IBeginEventAction,  public IEndEventAction,  public IBeginRunAction,  public ISteppingAction
  {
    
  public:
    
    struct Config
    {
      ToolHandle<Trk::IGeantFollowerHelper>  helper=ToolHandle<Trk::IGeantFollowerHelper>("Trk::GeantFollowerHelper/GeantFollowerHelper");
    };
    
    GeantFollower(const Config& config);
    virtual void beginOfEvent(const G4Event*) override;
    virtual void endOfEvent(const G4Event*) override;
    virtual void beginOfRun(const G4Run*) override;
    virtual void processStep(const G4Step*) override;
  private:
    Config m_config;
    std::string                                  m_name;
    mutable const Trk::IGeantFollowerHelper*  m_helperPointer;

  }; // class GeantFollower
  
  
} // namespace G4UA 

#endif

