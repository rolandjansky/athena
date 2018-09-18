/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#include "ActsGeometry/ActsTrackingGeometryTool.h"

// ATHENA
#include "GaudiKernel/IInterface.h"
#include "GeoPrimitives/GeoPrimitives.h"
#include "GeoModelUtilities/GeoAlignmentStore.h"

// PACKAGE
//#include "ActsGeometry/IActsTrackingGeometrySvc.h"

// STL
#include <iostream>
#include <memory>

ActsTrackingGeometryTool::ActsTrackingGeometryTool(const std::string& type, const std::string& name,
    const IInterface* parent) 
  : AthAlgTool(type, name, parent),
    m_trackingGeometrySvc("ActsTrackingGeometrySvc", name)
{
}
  
StatusCode 
ActsTrackingGeometryTool::initialize()
{
  ATH_MSG_INFO(name() << " initializing");

  ATH_CHECK( m_trackingGeometrySvc.retrieve() );

  ATH_CHECK( m_rchk.initialize() );

  return StatusCode::SUCCESS;
}

std::shared_ptr<const Acts::TrackingGeometry>
ActsTrackingGeometryTool::trackingGeometry() const
{
  return m_trackingGeometrySvc->trackingGeometry();
}

StatusCode
ActsTrackingGeometryTool::prepareAlignment() const
{
  ATH_MSG_DEBUG("Setting up alignment for this event");
  SG::ReadCondHandle<GeoAlignmentStore> rch(m_rchk);

  if (!rch.isValid()) {
    ATH_MSG_ERROR("Preparing alignment not possible, ReadCondHandle invalid");
    return StatusCode::FAILURE;
  }
  
  const GeoAlignmentStore* gas = *rch;

  m_trackingGeometrySvc->setGeoAlignmentStore(gas, Gaudi::Hive::currentContext());
  ATH_MSG_DEBUG("GeoAlignmentStore registered successfully");

  return StatusCode::SUCCESS;
}
