/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
#include "ActsGeometry/ActsWriteTrackingGeometry.h"

// ATHENA
#include "AthenaKernel/RNGWrapper.h"
#include "Acts/Utilities/Logger.hpp"
#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ISvcLocator.h"
#include "ActsInterop/Logger.h"

// PACKAGE
#include "ActsGeometryInterfaces/IActsTrackingGeometrySvc.h"
#include "ActsGeometry/ActsGeometryContext.h"

// STL
#include <string>

ActsWriteTrackingGeometry::ActsWriteTrackingGeometry(const std::string& name,
                                 ISvcLocator* pSvcLocator)
    : AthAlgorithm(name, pSvcLocator)
{
}

StatusCode ActsWriteTrackingGeometry::initialize() {
  ATH_MSG_INFO("initializing");

  ATH_CHECK(m_objWriterTool.retrieve());
  ATH_CHECK(m_trackingGeometryTool.retrieve());



  return StatusCode::SUCCESS;
}

StatusCode ActsWriteTrackingGeometry::execute() {
  auto trackingGeometry = m_trackingGeometryTool->trackingGeometry();

  // Use nominal context
  ActsGeometryContext defGctx;

  m_objWriterTool->write(defGctx, *trackingGeometry);
  return StatusCode::SUCCESS;
}

StatusCode ActsWriteTrackingGeometry::finalize() {
  return StatusCode::SUCCESS;
}
