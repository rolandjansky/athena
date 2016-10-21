/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// GeantFollowerMS.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef GeantFollowerMS_H
#define GeantFollowerMS_H

#include "G4AtlasTools/UserActionBase.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ServiceHandle.h"
#include "TrkDetDescrInterfaces/ITrackingGeometrySvc.h"
#include <string>
#include <vector>

/** @class GeantFollowerMS

    @author Andreas.Salzburger@cern.ch
*/

namespace Trk {
  class IGeantFollowerMSHelper;
  class TrackingGeometry;
}

class StoreGateSvc;

class GeantFollowerMS final: public UserActionBase {

public:
  /** Standard UserAction Constructor*/
  GeantFollowerMS(const std::string& type, const std::string& name, const IInterface* parent);

  /** All G4 interface methods */
  virtual void BeginOfEvent(const G4Event*) override;
  virtual void EndOfEvent(const G4Event*) override;
  virtual void Step(const G4Step*) override;

  virtual StatusCode queryInterface(const InterfaceID&, void**) override;
  virtual StatusCode initialize() override;

private:
  ToolHandle<Trk::IGeantFollowerMSHelper>     m_helper;
  mutable const Trk::IGeantFollowerMSHelper*  m_helperPointer;


  /** tracking geometry */
  mutable const Trk::TrackingGeometry*                         m_trackingGeometry;
  ServiceHandle<Trk::ITrackingGeometrySvc>                     m_trackingGeometrySvc;
  std::string                                                  m_trackingGeometryName;


};


#include "G4AtlasInterfaces/IBeginEventAction.h"
#include "G4AtlasInterfaces/IEndEventAction.h"
#include "G4AtlasInterfaces/IBeginRunAction.h"
#include "G4AtlasInterfaces/ISteppingAction.h"
#include "AthenaBaseComps/AthMessaging.h"
namespace G4UA{


  class GeantFollowerMS:
  public AthMessaging, public IBeginEventAction,  public IEndEventAction,  public IBeginRunAction,  public ISteppingAction
  {
    
  public:
    
    struct Config
    {
      ToolHandle<Trk::IGeantFollowerMSHelper>  helper=ToolHandle<Trk::IGeantFollowerMSHelper>("Trk::GeantFollowerMSHelper/GeantFollowerMSHelper");
      ServiceHandle<Trk::ITrackingGeometrySvc> trackingGeometrySvc=ServiceHandle<Trk::ITrackingGeometrySvc>("AtlasTrackingGeometrySvc","GeantFollowerMS");
    };
    
    GeantFollowerMS(const Config& config);
    virtual void beginOfEvent(const G4Event*) override;
    virtual void endOfEvent(const G4Event*) override;
    virtual void beginOfRun(const G4Run*) override;
    virtual void processStep(const G4Step*) override;
  private:
    Config m_config;
    std::string                                  m_name;
    /** tracking geometry */
    mutable const Trk::TrackingGeometry*         m_trackingGeometry;
    mutable const Trk::IGeantFollowerMSHelper*   m_helperPointer;

  }; // class GeantFollowerMS
  
  
} // namespace G4UA 

#endif
