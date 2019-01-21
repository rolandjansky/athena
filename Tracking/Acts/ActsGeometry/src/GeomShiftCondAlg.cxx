/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "ActsGeometry/GeomShiftCondAlg.h"

// ATHENA
#include "StoreGate/WriteCondHandle.h"
#include "GaudiKernel/EventIDBase.h"
#include "GaudiKernel/EventIDRange.h"
#include "EventInfo/EventInfo.h"
#include "EventInfo/EventID.h"
#include "GeoModelKernel/GeoAlignableTransform.h"
#include "InDetReadoutGeometry/PixelDetectorManager.h"
#include "InDetReadoutGeometry/SCT_DetectorManager.h"
#include "InDetReadoutGeometry/TRT_DetectorManager.h"
#include "GeoPrimitives/CLHEPtoEigenConverter.h"
#include "InDetReadoutGeometry/ExtendedAlignableTransform.h"
#include "GaudiKernel/ICondSvc.h"
#include "StoreGate/StoreGateSvc.h"

// PACKAGE
#include "ActsGeometry/ActsAlignmentStore.h"
#include "ActsGeometry/ActsDetectorElement.h"
#include "ActsGeometry/IActsTrackingGeometrySvc.h"

// ACTS
#include "Acts/Utilities/Definitions.hpp"
#include "Acts/Detector/TrackingGeometry.hpp"
#include "Acts/Detector/DetectorElementBase.hpp"
#include "Acts/Surfaces/Surface.hpp"

// STL
#include <memory>

GeomShiftCondAlg::GeomShiftCondAlg( const std::string& name, 
            ISvcLocator* pSvcLocator ) : 
  ::AthAlgorithm( name, pSvcLocator ),
  m_cs("CondSvc",name),
  m_trackingGeometrySvc("ActsTrackingGeometrySvc", name),
  m_detStore("StoreGateSvc/DetectorStore", name)
{
}

GeomShiftCondAlg::~GeomShiftCondAlg() {}

StatusCode GeomShiftCondAlg::initialize() {
  ATH_MSG_DEBUG("initialize " << name());

  ATH_CHECK( m_evt.initialize() );

  if (m_cs.retrieve().isFailure()) {
    ATH_MSG_ERROR("unable to retrieve CondSvc");
  }

  ATH_CHECK ( m_detStore->retrieve(p_pixelManager, "Pixel") );
  ATH_CHECK ( m_detStore->retrieve(p_SCTManager, "SCT") );
  ATH_CHECK ( m_detStore->retrieve(p_TRTManager, "TRT") );


  if (m_wchk.initialize().isFailure()) {
    ATH_MSG_ERROR("unable to initialize WriteCondHandle with key" << m_wchk.key() );
    return StatusCode::FAILURE;
  }

  if (m_cs->regHandle(this, m_wchk).isFailure()) {
    ATH_MSG_ERROR("unable to register WriteCondHandle " << m_wchk.fullKey() 
                  << " with CondSvc");
    return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;
}

StatusCode GeomShiftCondAlg::finalize() {
  ATH_MSG_DEBUG("finalize " << name());
  return StatusCode::SUCCESS;
}

StatusCode GeomShiftCondAlg::execute() {
  ATH_MSG_DEBUG("execute " << name());

  SG::ReadHandle<EventInfo> evt( m_evt );
  if (!evt.isValid()) {
    ATH_MSG_ERROR ("Could not retrieve EventInfo");
    return StatusCode::FAILURE;
  }

  ATH_MSG_DEBUG("   EventInfo:  r: " << evt->event_ID()->run_number()
                << " e: " << evt->event_ID()->event_number() );


  SG::WriteCondHandle<ActsAlignmentStore> wch(m_wchk);

  EventIDBase now(getContext().eventID());

  if (evt->event_ID()->event_number() == 10) {
      std::this_thread::sleep_for(std::chrono::milliseconds( 500 ));
  }

  // do we have a valid m_wch for current time?
  if ( wch.isValid(now) ) {
    ATH_MSG_DEBUG("CondHandle is already valid for " << now
		  << ". In theory this should not be called, but may happen"
		  << " if multiple concurrent events are being processed out of order.");

  } else {

    ATH_MSG_DEBUG("  CondHandle " << wch.key() 
                  << " not valid now (" << now << "). Getting new info for dbKey \"" 
                  << wch.dbKey() << "\" from CondDb");

    ATH_MSG_ALWAYS("SG evt: " << *(evt->event_ID()));

    ATH_MSG_DEBUG("LB EventInfo = " << evt->event_ID()->lumi_block());
    ATH_MSG_DEBUG("LB context = " << getContext().eventID().lumi_block());

    unsigned int intvl_num = evt->event_ID()->lumi_block();

    double val = intvl_num * double(m_zShiftPerLB);

    EventIDBase start(1, EventIDBase::UNDEFEVT);
    EventIDBase end(1, EventIDBase::UNDEFEVT);

    start.set_lumi_block(intvl_num);
    end.set_lumi_block(intvl_num+1);

    EventIDRange r(start, end);

    ActsAlignmentStore* alignStore = new ActsAlignmentStore();
    
    InDetDD::PixelDetectorManager::AlignableTransformMap& atMatL1 
      = const_cast<InDetDD::PixelDetectorManager::AlignableTransformMap&>(
          p_pixelManager->m_higherAlignableTransforms.at(1));


    PixelID idHelper;

    for(const auto& eat_item : atMatL1) {
      const Identifier &id = eat_item.first;

      //std::cout << "Identifier: (bec, d, phim, etam, phiidx, etaidx): ";
      //std::cout << idHelper.barrel_ec(id) << " ";
      //std::cout << idHelper.layer_disk(id) << " ";
      //std::cout << idHelper.phi_module(id) << " ";
      //std::cout << idHelper.eta_module(id) << " ";
      //std::cout << idHelper.phi_index(id) << " ";
      //std::cout << idHelper.eta_index(id) << " ";
      //std::cout << std::endl;
    

      InDetDD::ExtendedAlignableTransform* eat = eat_item.second;
      GeoAlignableTransform* alTrf = eat->alignableTransform();
      Acts::Transform3D delta;
      delta = Acts::Translation3D(Acts::Vector3D::UnitZ()*val);
      ATH_MSG_DEBUG("add delta: " << alTrf << " -> (z=" << val << ")");
      alignStore->setDelta(alTrf, Amg::EigenTransformToCLHEP(delta));
    }
    
    auto trkGeom = m_trackingGeometrySvc->trackingGeometry();

    // deltas are set, now populate sensitive element transforms
    ATH_MSG_DEBUG("Populating ActsAlignmentStore for IOV");
    size_t nElems = 0;
    trkGeom->visitSurfaces(
      [alignStore, &nElems](const Acts::Surface* srf) {
      const Acts::DetectorElementBase* detElem = srf->associatedDetectorElement();
      const auto* gmde = dynamic_cast<const ActsDetectorElement*>(detElem);
      gmde->storeTransform(alignStore);
      nElems++;
    });
    ATH_MSG_DEBUG("ActsAlignmentStore populated for " << nElems << " detector elements");


    if (wch.record(r, alignStore).isFailure()) {
      ATH_MSG_ERROR("could not record shift " << wch.key() 
		    << " = " << alignStore
                    << " with EventRange " << r);
      return StatusCode::FAILURE;
    }

    // is it valid now?
    ATH_MSG_ALWAYS("isValid now? " << wch.isValid(now));


    ATH_MSG_INFO("recorded new shift " << wch.key() << " "
		 << " with range " << r);
  }



  return StatusCode::SUCCESS;

}

