#include "ActsGeometry/TrackingGeometryTool.h"

#include "GaudiKernel/IInterface.h"
#include "GeoPrimitives/GeoPrimitives.h" // Get athena matrix plugin before ACTS'
#include "GeoModelUtilities/GeoAlignmentStore.h"

#include "ActsGeometry/TrackingGeometrySvc.h"

#include <iostream>
#include <memory>

Acts::TrackingGeometryTool::TrackingGeometryTool(const std::string& type, const std::string& name,
    const IInterface* parent) 
  : AthAlgTool(type, name, parent),
    m_trackingGeometrySvc("TrackingGeometrySvc", name)
{
}
  
StatusCode 
Acts::TrackingGeometryTool::initialize()
{
  ATH_MSG_INFO(name() << " initializing");
  ATH_CHECK( m_trackingGeometrySvc.retrieve() );

  ATH_CHECK( m_rchk.initialize() );

  return StatusCode::SUCCESS;
}

std::shared_ptr<const Acts::TrackingGeometry>
Acts::TrackingGeometryTool::trackingGeometry() const
{
  return m_trackingGeometrySvc->trackingGeometry();
}

StatusCode
Acts::TrackingGeometryTool::prepareAlignment() const
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
