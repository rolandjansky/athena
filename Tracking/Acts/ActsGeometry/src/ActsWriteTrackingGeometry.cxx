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
#include "ActsGeometry/IActsTrackingGeometrySvc.h"

// STL
#include <string>

ActsWriteTrackingGeometry::ActsWriteTrackingGeometry(const std::string& name,
                                 ISvcLocator* pSvcLocator)
    : AthAlgorithm(name, pSvcLocator),
      m_trackingGeometrySvc("ActsTrackingGeometrySvc", name)
{
}

StatusCode ActsWriteTrackingGeometry::initialize() {
  ATH_MSG_INFO("initializing");

  ATH_CHECK(m_trackingGeometrySvc.retrieve());
  ATH_CHECK(m_objWriterTool.retrieve());

  m_trackingGeometry = m_trackingGeometrySvc->trackingGeometry();

  m_objWriterTool->write(*m_trackingGeometry);

  return StatusCode::SUCCESS;
}

StatusCode ActsWriteTrackingGeometry::execute() {
  return StatusCode::SUCCESS;
}

StatusCode ActsWriteTrackingGeometry::finalize() {
  return StatusCode::SUCCESS;
}

