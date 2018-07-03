/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "ActsGeometry/GeomShiftCondAlg.h"
#include "ActsGeometry/ShiftCondObj.h"

#include "StoreGate/WriteCondHandle.h"

#include "GaudiKernel/ServiceHandle.h"

#include "GaudiKernel/EventIDBase.h"
#include "GaudiKernel/EventIDRange.h"

#include "EventInfo/EventInfo.h"
#include "EventInfo/EventID.h"
#include "GeoModelKernel/GeoVPhysVol.h"
#include "GeoModelKernel/GeoNodeAction.h"
#include "GeoModelKernel/GeoAlignableTransform.h"

#include "InDetReadoutGeometry/SiDetectorManager.h"
#include "InDetReadoutGeometry/PixelDetectorManager.h"
#include "InDetReadoutGeometry/TRT_DetectorManager.h"

#include "GeoPrimitives/CLHEPtoEigenConverter.h"
#include "GeoModelUtilities/GeoAlignmentStore.h"
#include "InDetReadoutGeometry/ExtendedAlignableTransform.h"

#include "Acts/Utilities/Definitions.hpp"

#include <thread>
#include <chrono>
#include <memory>
#include <iostream>

GeomShiftCondAlg::GeomShiftCondAlg( const std::string& name, 
            ISvcLocator* pSvcLocator ) : 
  ::AthAlgorithm( name, pSvcLocator ),
  m_cs("CondSvc",name),
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

  struct AlignableTransformFindAction : public GeoNodeAction {

    const GeoAlignableTransform* highestTransform = nullptr;

    //void 
    //handlePhysVol (const GeoPhysVol *vol) {
      //std::cout << "VOLUME(" << vol->getLogVol()->getName() <<")" << std::endl;
    //}
    
    //void 
    //handleFullPhysVol (const GeoPhysVol *vol) {
      //std::cout << "VOLUME(" << vol->getLogVol()->getName() <<")" << std::endl;
    //}
    //void
    //handleNameTag (const GeoNameTag *nameTag)
    //{
      //std::cout << "NAMETAG: " << nameTag->getName() << "+";
    //}

    //void 
    //handleIdentifierTag (const GeoIdentifierTag *idTag) {
      //std::cout << "NAME: " << idTag->getIdentifier() << "+";
    //}

    void
    handleTransform(const GeoTransform* xf) {
      if (highestTransform != nullptr) return;
      auto alignableTransform = dynamic_cast<const GeoAlignableTransform*>(xf);
      if (alignableTransform != nullptr) {
        //std::cout << __FUNCTION__ << ": " << xf << std::endl;
        highestTransform = alignableTransform;
      }
    }

  };


  // let's only do pixel for now
  //size_t numTreeTops = p_pixelManager->getNumTreeTops();

  //std::vector<const GeoAlignableTransform*> topAligns(numTreeTops);

  //for(size_t i=0;i<numTreeTops;i++) {
    //AlignableTransformFindAction alignableTransformFindAction;
    //PVConstLink pixelTreeTop = p_pixelManager->getTreeTop(i);
    //pixelTreeTop->exec(&alignableTransformFindAction);
    //m_topAligns.push_back(alignableTransformFindAction.highestTransform);
  //}


  //if (m_cds.retrieve().isFailure()) {
    //ATH_MSG_ERROR("unable to retrieve ASCIICondDbSvc");
  //}

  m_wchk.setDbKey(m_dbKey);

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


  SG::WriteCondHandle<GeoAlignmentStore> wch(m_wchk);

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

    //Transform3D shift;
    //shift

    //ShiftCondObj* cdo = new ShiftCondObj( val );
    
    GeoAlignmentStore* alignStore = new GeoAlignmentStore();
    
    const InDetDD::PixelDetectorManager* pixMgr 
      = dynamic_cast<const InDetDD::PixelDetectorManager*>(p_pixelManager);

    InDetDD::PixelDetectorManager::AlignableTransformMap& atMatL1 
      = const_cast<InDetDD::PixelDetectorManager::AlignableTransformMap&>(
          pixMgr->m_higherAlignableTransforms.at(1));


    PixelID idHelper;

    for(const auto& eat_item : atMatL1) {
      const Identifier &id = eat_item.first;

      std::cout << "Identifier: (bec, d, phim, etam, phiidx, etaidx): ";
      std::cout << idHelper.barrel_ec(id) << " ";
      std::cout << idHelper.layer_disk(id) << " ";
      std::cout << idHelper.phi_module(id) << " ";
      std::cout << idHelper.eta_module(id) << " ";
      std::cout << idHelper.phi_index(id) << " ";
      std::cout << idHelper.eta_index(id) << " ";
      std::cout << std::endl;
    

      InDetDD::ExtendedAlignableTransform* eat = eat_item.second;
      GeoAlignableTransform* alTrf = eat->alignableTransform();
      Acts::Transform3D delta;
      delta = Acts::Translation3D(Acts::Vector3D::UnitZ()*val);
      ATH_MSG_DEBUG("add delta: " << alTrf << " -> (z=" << val << ")");
      alignStore->setDelta(alTrf, Amg::EigenTransformToCLHEP(delta));
    }

    //for (const GeoAlignableTransform* alignableTrf : m_topAligns) {
      //Acts::Transform3D delta;
      //delta = Acts::Translation3D(Acts::Vector3D::UnitZ()*val);
      //alignStore->setDelta(alignableTrf, Amg::EigenTransformToCLHEP(delta));
    //}


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

