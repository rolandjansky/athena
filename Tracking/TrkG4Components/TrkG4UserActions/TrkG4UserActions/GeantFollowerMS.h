/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// GeantFollowerMS.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef GeantFollowerMS_H
#define GeantFollowerMS_H

#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ServiceHandle.h"
#include "TrkDetDescrInterfaces/ITrackingGeometrySvc.h"

#include "G4UserEventAction.hh"
#include "G4UserRunAction.hh"
#include "G4UserSteppingAction.hh"

/** @class GeantFollowerMS

    @author Andreas.Salzburger@cern.ch
*/

namespace Trk {
  class IGeantFollowerMSHelper;
  class TrackingGeometry;
}

namespace G4UA
{

  class GeantFollowerMS: public G4UserEventAction, public G4UserRunAction, public G4UserSteppingAction
  {

  public:

    struct Config
    {
      // FIXME: public tool handle in thread-local action!
      // See ATLASSIM-3561.
      ToolHandle<Trk::IGeantFollowerMSHelper> helper =
        ToolHandle<Trk::IGeantFollowerMSHelper>("Trk::GeantFollowerMSHelper/GeantFollowerMSHelper");
      ServiceHandle<Trk::ITrackingGeometrySvc> trackingGeometrySvc =
        ServiceHandle<Trk::ITrackingGeometrySvc>("AtlasTrackingGeometrySvc","GeantFollowerMS");
    };

    GeantFollowerMS(const Config& config);
    virtual void BeginOfEventAction(const G4Event*) override;
    virtual void EndOfEventAction(const G4Event*) override;
    virtual void BeginOfRunAction(const G4Run*) override;
    virtual void UserSteppingAction(const G4Step*) override;

  private:

    Config m_config;

    /** tracking geometry
        FIXME: mutables in thread-local action! ATLASSIM-3561.
    */
    mutable const Trk::IGeantFollowerMSHelper*   m_helperPointer;

  }; // class GeantFollowerMS

} // namespace G4UA

#endif
