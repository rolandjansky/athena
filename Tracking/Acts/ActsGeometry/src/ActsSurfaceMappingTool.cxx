/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "ActsGeometry/ActsSurfaceMappingTool.h"

// ATHENA
#include "GaudiKernel/IInterface.h"
// PACKAGE
#include "ActsInterop/Logger.h"
#include "ActsGeometry/ActsGeometryContext.h"
#include "ActsGeometry/ActsTrackingGeometryTool.h"
#include "ActsGeometryInterfaces/IActsTrackingGeometryTool.h"

// ActsSurfaceMappingTool
#include "Acts/Geometry/GeometryContext.hpp"
#include "Acts/Propagator/Navigator.hpp"
#include "Acts/Propagator/Propagator.hpp"
#include "Acts/Propagator/StraightLineStepper.hpp"

// STL
#include <iostream>
#include <memory>


ActsSurfaceMappingTool::ActsSurfaceMappingTool(const std::string& type, const std::string& name,
    const IInterface* parent)
  : base_class(type, name, parent)
{
}

StatusCode
ActsSurfaceMappingTool::initialize()
{
  ATH_MSG_INFO("Initializing ACTS Surface Mapper");

  ATH_CHECK( m_trackingGeometryTool.retrieve() );

  m_trackingGeometry = m_trackingGeometryTool->trackingGeometry();

  Acts::Navigator navigator(m_trackingGeometry);
  // Make stepper and propagator
  SlStepper stepper;
  StraightLinePropagator propagator = StraightLinePropagator(std::move(stepper), std::move(navigator));

  /// The material mapper
  Acts::SurfaceMaterialMapper::Config smmConfig;
  smmConfig.mapperDebugOutput = true;
  m_mapper = std::make_shared<Acts::SurfaceMaterialMapper>(
      smmConfig,
      std::move(propagator),
      makeActsAthenaLogger(this, "SurfaceMaterialMapper"));

  m_geoContext = m_trackingGeometryTool->getNominalGeometryContext().any();

  ATH_MSG_INFO("ACTS Surface Mapper successfully initialized");
  return StatusCode::SUCCESS;
}

Acts::SurfaceMaterialMapper::State
ActsSurfaceMappingTool::mappingState() const
{
  auto mappingState = m_mapper->createState(
    m_geoContext, m_magFieldContext, *m_trackingGeometry);

  return mappingState;
}
