#include "ActsGeometry/TrackingGeometryTool.h"

#include "GaudiKernel/IInterface.h"
#include "GeoPrimitives/GeoPrimitives.h" // Get athena matrix plugin before ACTS'

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

  return StatusCode::SUCCESS;
}

std::shared_ptr<const Acts::TrackingGeometry>
Acts::TrackingGeometryTool::trackingGeometry() const
{
  return m_trackingGeometrySvc->trackingGeometry();
}
