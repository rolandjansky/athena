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

#endif
