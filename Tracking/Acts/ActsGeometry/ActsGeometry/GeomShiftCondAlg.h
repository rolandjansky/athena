/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#pragma once

// ATHENA
#include "AthenaBaseComps/AthAlgorithm.h"
#include "StoreGate/WriteCondHandleKey.h"
#include "GaudiKernel/ServiceHandle.h"
#include "EventInfo/EventInfo.h" // ReadHandleKey wants complete type

// PACKAGE

// STL
#include <string>

class EventInfo;
class ICondSvc;
class IActsTrackingGeometrySvc;
class ActsAlignmentStore;
class GeoAlignableTransform;
class ActsGeometryContext;


class GeomShiftCondAlg  :  public AthAlgorithm {

public:

  GeomShiftCondAlg (const std::string& name, ISvcLocator* pSvcLocator);
  virtual ~GeomShiftCondAlg();

  virtual StatusCode initialize() override;
  virtual StatusCode execute() override;

private:

  SG::ReadHandleKey<EventInfo> m_evt {this,"EvtInfo", "McEventInfo", "EventInfo name"};

  SG::WriteCondHandleKey<ActsGeometryContext> m_wchk {this, "PixelAlignmentKey", "PixelAlignment", "cond handle key"};

  Gaudi::Property<double> m_zShiftPerLB {this, "ZShiftPerLB", 10.5, ""};

  ServiceHandle<IActsTrackingGeometrySvc> m_trackingGeometrySvc;

  std::vector<const GeoAlignableTransform*> m_topAligns;

};
