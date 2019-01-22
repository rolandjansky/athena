/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
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

namespace InDetDD {
  class InDetDetectorManager;
  class PixelDetectorManager;
  class SCT_DetectorManager;
  class TRT_DetectorManager;
}

class EventInfo;
class ICondSvc;
class StoreGateSvc;
class IActsTrackingGeometrySvc;
class ActsAlignmentStore;
class GeoAlignableTransform;


class GeomShiftCondAlg  :  public AthAlgorithm {
  
public:
    
  GeomShiftCondAlg (const std::string& name, ISvcLocator* pSvcLocator);
  virtual ~GeomShiftCondAlg();
  
  virtual bool isClonable() const override { return true; }

  virtual StatusCode initialize() override;
  virtual StatusCode execute() override;
  virtual StatusCode finalize() override;

private:
  
  SG::ReadHandleKey<EventInfo> m_evt {this,"EvtInfo", "McEventInfo", "EventInfo name"};

  SG::WriteCondHandleKey<ActsAlignmentStore> m_wchk {this, "PixelAlignmentKey", "PixelAlignment", "cond handle key"};

  Gaudi::Property<double> m_zShiftPerLB {this, "ZShiftPerLB", 10.5, ""};

  ServiceHandle<ICondSvc> m_cs;
  ServiceHandle<IActsTrackingGeometrySvc> m_trackingGeometrySvc;

  ServiceHandle<StoreGateSvc> m_detStore;
  const InDetDD::PixelDetectorManager* p_pixelManager;
  const InDetDD::SCT_DetectorManager* p_SCTManager;
  const InDetDD::TRT_DetectorManager* p_TRTManager;

  std::vector<const GeoAlignableTransform*> m_topAligns;

};

