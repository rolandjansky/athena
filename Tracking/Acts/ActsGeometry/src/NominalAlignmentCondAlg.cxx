/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "ActsGeometry/NominalAlignmentCondAlg.h"
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

NominalAlignmentCondAlg::NominalAlignmentCondAlg( const std::string& name, 
            ISvcLocator* pSvcLocator ) : 
  ::AthAlgorithm( name, pSvcLocator ),
  m_cs("CondSvc",name)
{
}

NominalAlignmentCondAlg::~NominalAlignmentCondAlg() {}

StatusCode NominalAlignmentCondAlg::initialize() {
  ATH_MSG_DEBUG("initialize " << name());

  ATH_CHECK( m_evt.initialize() );

  if (m_cs.retrieve().isFailure()) {
    ATH_MSG_ERROR("unable to retrieve CondSvc");
  }

  //m_wchk.setDbKey(m_dbKey);

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

StatusCode NominalAlignmentCondAlg::finalize() {
  ATH_MSG_DEBUG("finalize " << name());
  return StatusCode::SUCCESS;
}

StatusCode NominalAlignmentCondAlg::execute() {
  ATH_MSG_DEBUG("execute " << name());

  SG::ReadHandle<EventInfo> evt( m_evt );
  if (!evt.isValid()) {
    ATH_MSG_ERROR ("Could not retrieve EventInfo");
    return StatusCode::FAILURE;
  }

  SG::WriteCondHandle<GeoAlignmentStore> wch(m_wchk);

  EventIDBase now(getContext().eventID());

  // do we have a valid m_wch for current time?
  if ( wch.isValid(now) ) {
    ATH_MSG_DEBUG("CondHandle is already valid for " << now
		  << ". In theory this should not be called, but may happen"
		  << " if multiple concurrent events are being processed out of order.");

  } else {

    ATH_MSG_DEBUG("  CondHandle " << wch.key() 
                  << " not valid now (" << now << "). Setting nominal alignment cond");


    EventIDBase start(1, EventIDBase::UNDEFEVT);
    EventIDBase end(1, EventIDBase::UNDEFEVT);
    start.set_lumi_block(0);
    end.set_lumi_block(9999); // this is not actually forever

    EventIDRange r(start, end);

    ATH_MSG_DEBUG("Will register nominal alignment for range: " << r);

    // we record an empty alignment store
    GeoAlignmentStore* alignStore = new GeoAlignmentStore();

    if (wch.record(r, alignStore).isFailure()) {
      ATH_MSG_ERROR("could not record nominal GeoAlignmentStore " << wch.key() 
		    << " = " << alignStore
                    << " with EventRange " << r);
      return StatusCode::FAILURE;
    }

    ATH_MSG_DEBUG("WCH is valid now? " << wch.isValid(now));
  }
  return StatusCode::SUCCESS;
}

