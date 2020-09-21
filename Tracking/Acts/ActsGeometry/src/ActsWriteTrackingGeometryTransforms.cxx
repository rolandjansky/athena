/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/
#include "ActsGeometry/ActsWriteTrackingGeometryTransforms.h"

// ATHENA
#include "AthenaKernel/RNGWrapper.h"
#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ISvcLocator.h"
#include "ActsInterop/Logger.h"

// PACKAGE
#include "ActsGeometryInterfaces/IActsTrackingGeometrySvc.h"
#include "ActsGeometry/ActsGeometryContext.h"
#include "ActsGeometry/ActsDetectorElement.h"

// ACTS
#include "Acts/Utilities/Logger.hpp"
#include "Acts/Geometry/TrackingGeometry.hpp"
#include "Acts/Surfaces/Surface.hpp"

// STL
#include <string>

using gid = Acts::GeometryIdentifier;

ActsWriteTrackingGeometryTransforms::ActsWriteTrackingGeometryTransforms(const std::string& name,
                                 ISvcLocator* pSvcLocator)
    : AthAlgorithm(name, pSvcLocator)
{
}

StatusCode ActsWriteTrackingGeometryTransforms::initialize() {
  ATH_MSG_INFO("initializing");

  ATH_CHECK(m_trackingGeometryTool.retrieve());

  std::ofstream os(m_outputName); // truncate

  return StatusCode::SUCCESS;
}

StatusCode ActsWriteTrackingGeometryTransforms::execute() {
  const EventContext& ctx = Gaudi::Hive::currentContext();
  auto trackingGeometry = m_trackingGeometryTool->trackingGeometry();
  const ActsGeometryContext& gctx = m_trackingGeometryTool->getGeometryContext(ctx);

  std::stringstream ss;


  std::ofstream os(m_outputName, std::ios_base::app);

  trackingGeometry->visitSurfaces([&] (const Acts::Surface* srf) {
    const Acts::DetectorElementBase *detElem = srf->associatedDetectorElement();
    const auto *gmde = static_cast<const ActsDetectorElement *>(detElem);

    if(gmde->getSubdetector() == ActsDetectorElement::Subdetector::TRT) {
      return;
    }

    gid geoID = srf->geometryId();
    os << geoID.volume() << ";";
    os << geoID.boundary() << ";";
    os << geoID.layer() << ";";
    os << geoID.approach() << ";";
    os << geoID.sensitive() << ";";

    os << ctx.eventID().event_number() << ";";

    if(gmde->getSubdetector() == ActsDetectorElement::Subdetector::Pixel) {
      os << 0;
    }
    else if(gmde->getSubdetector() == ActsDetectorElement::Subdetector::SCT) {
      os << 1;
    }

    os << ";";

    // iterate over components of transform
    const auto* p = srf->transform(gctx.any()).data();
    for(size_t i=0;i<16;i++) {
      if(i>0) {
        os << ",";
      }
      os << *(p+i);
    }
    os << "\n";
  });


  return StatusCode::SUCCESS;
}

StatusCode ActsWriteTrackingGeometryTransforms::finalize() {
  return StatusCode::SUCCESS;
}
