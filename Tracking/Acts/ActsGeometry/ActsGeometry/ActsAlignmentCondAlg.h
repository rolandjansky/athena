/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ACTSGEOMETRY_ACTSALIGNMENTCONDALG_H
#define ACTSGEOMETRY_ACTSALIGNMENTCONDALG_H

// ATHENA
#include "AthenaBaseComps/AthAlgorithm.h"
#include "EventInfo/EventInfo.h" // ReadHandleKey wants complete type
#include "GaudiKernel/ServiceHandle.h"
#include "StoreGate/WriteCondHandleKey.h"

// PACKAGE

// STL
#include <string>

class EventInfo;
class ICondSvc;
class StoreGateSvc;
class IActsTrackingGeometrySvc;
class ActsAlignmentStore;
class GeoAlignableTransform;
class ActsGeometryContext;
class GeoAlignmentStore;

class ActsAlignmentCondAlg : public AthAlgorithm {

public:
  ActsAlignmentCondAlg(const std::string &name, ISvcLocator *pSvcLocator);
  virtual ~ActsAlignmentCondAlg();

  virtual StatusCode initialize() override;
  virtual StatusCode execute() override;

private:

  SG::ReadCondHandleKey<GeoAlignmentStore> m_sctAlignStoreReadKey{
      this, "SCTAlignStoreReadKey", "SCTAlignmentStore", ""};

  SG::ReadCondHandleKey<GeoAlignmentStore> m_pixelAlignStoreReadKey{
      this, "PixelAlignStoreReadKey", "PixelAlignmentStore", ""};

  SG::WriteCondHandleKey<ActsGeometryContext> m_wchk{
      this, "ActsAlignmentKey", "ActsAlignment", "cond handle key"};

  ServiceHandle<IActsTrackingGeometrySvc> m_trackingGeometrySvc;
};

#endif
