/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "ActsGeometry/ActsAlignmentCondAlg.h"

// ATHENA
#include "EventInfo/EventID.h"
#include "EventInfo/EventInfo.h"
#include "GaudiKernel/EventIDBase.h"
#include "GaudiKernel/EventIDRange.h"
#include "GaudiKernel/ICondSvc.h"
#include "GeoModelKernel/GeoAlignableTransform.h"
#include "StoreGate/StoreGateSvc.h"
#include "StoreGate/WriteCondHandle.h"

// PACKAGE
#include "ActsGeometry/ActsAlignmentStore.h"
#include "ActsGeometry/ActsDetectorElement.h"
#include "ActsGeometry/ActsGeometryContext.h"
#include "ActsGeometryInterfaces/IActsTrackingGeometrySvc.h"

// ACTS
#include "Acts/Geometry/DetectorElementBase.hpp"
#include "Acts/Geometry/TrackingGeometry.hpp"
#include "Acts/Surfaces/Surface.hpp"
#include "Acts/Utilities/Definitions.hpp"

// STL
#include <memory>

ActsAlignmentCondAlg::ActsAlignmentCondAlg(const std::string &name,
                                           ISvcLocator *pSvcLocator)
    : ::AthAlgorithm(name, pSvcLocator), m_cs("CondSvc", name),
      m_trackingGeometrySvc("ActsTrackingGeometrySvc", name) {}

ActsAlignmentCondAlg::~ActsAlignmentCondAlg() {}

StatusCode ActsAlignmentCondAlg::initialize() {
  ATH_MSG_DEBUG("initialize " << name());

  ATH_CHECK(m_pixelAlignStoreReadKey.initialize());
  ATH_CHECK(m_sctAlignStoreReadKey.initialize());

  if (m_cs.retrieve().isFailure()) {
    ATH_MSG_ERROR("unable to retrieve CondSvc");
    return StatusCode::FAILURE;
  }

  if (m_wchk.initialize().isFailure()) {
    ATH_MSG_ERROR("unable to initialize WriteCondHandle with key"
                  << m_wchk.key());
    return StatusCode::FAILURE;
  }

  if (m_cs->regHandle(this, m_wchk).isFailure()) {
    ATH_MSG_ERROR("unable to register WriteCondHandle " << m_wchk.fullKey()
                                                        << " with CondSvc");
    return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;
}

StatusCode ActsAlignmentCondAlg::finalize() {
  ATH_MSG_DEBUG("finalize " << name());
  return StatusCode::SUCCESS;
}

StatusCode ActsAlignmentCondAlg::execute() {
  ATH_MSG_DEBUG("execute " << name());

  auto trkGeom = m_trackingGeometrySvc->trackingGeometry();

  EventIDBase now(getContext().eventID());
  SG::WriteCondHandle<ActsGeometryContext> wch(m_wchk);

  if (wch.isValid(now)) {
    ATH_MSG_DEBUG(
        "CondHandle is already valid for "
        << now << ". In theory this should not be called, but may happen"
        << " if multiple concurrent events are being processed out of order.");

  } else {

    ATH_MSG_DEBUG("  CondHandle " << wch.key() << " not valid now (" << now
                                  << "). Getting new info for dbKey \""
                                  << wch.dbKey() << "\" from CondDb");

    // Pixel
    SG::ReadCondHandle<GeoAlignmentStore> pixelAlignStore(m_pixelAlignStoreReadKey);
    SG::ReadCondHandle<GeoAlignmentStore> sctAlignStore(m_sctAlignStoreReadKey);


    EventIDRange pixelRange;
    if(!pixelAlignStore.range(pixelRange)) {
      ATH_MSG_FATAL("Failed to retrieve validity range for " << pixelAlignStore.key());
      return StatusCode::FAILURE;
    }

    EventIDRange sctRange;
    if(!sctAlignStore.range(sctRange)) {
      ATH_MSG_FATAL("Failed to retrieve validity range for " << sctAlignStore.key());
      return StatusCode::FAILURE;
    }

    // create an Acts aware geo alignment store from the one given
    // (this makes a copy for now, which is not ideal)
    auto actsAlignStore =
        std::make_unique<ActsAlignmentStore>(**pixelAlignStore);

    actsAlignStore->append(**sctAlignStore);

    //std::unique_ptr<ActsAlignmentStore> sctActsAlignStore =
        //std::make_unique<ActsAlignmentStore>(**sctAlignStore);

    // deltas are set, now populate sensitive element transforms
    ATH_MSG_DEBUG("Populating ActsAlignmentStore for IOV");
    size_t nElems = 0;
    trkGeom->visitSurfaces([&actsAlignStore, &nElems](const Acts::Surface *srf) {
      const Acts::DetectorElementBase *detElem =
          srf->associatedDetectorElement();
      const auto *gmde = static_cast<const ActsDetectorElement *>(detElem);
      gmde->storeTransform(actsAlignStore.get());
      nElems++;
    });
    ATH_MSG_DEBUG("ActsAlignmentStore populated for " << nElems
                                                      << " detector elements");

    EventIDRange r = EventIDRange::intersect(pixelRange, sctRange);

    // get a nominal alignment store from the tracking geometry service
    // and plug it into a geometry context
    auto gctx = std::make_unique<ActsGeometryContext>();
    gctx->ownedAlignmentStore =
        std::move(actsAlignStore); // GCTX owns the alignment store
    gctx->alignmentStore = gctx->ownedAlignmentStore.get();

    if (wch.record(r, gctx.release()).isFailure()) {
      ATH_MSG_ERROR("could not record alignment store "
                    << wch.key() << " = " << gctx.get() << " with EventRange "
                    << r);
      return StatusCode::FAILURE;
    }

    ATH_MSG_INFO("Recorded new " << wch.key() << " "
                                       << " with range " << r);
  }



  return StatusCode::SUCCESS;
}
