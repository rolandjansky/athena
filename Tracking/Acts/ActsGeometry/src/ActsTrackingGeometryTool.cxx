/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#include "ActsGeometry/ActsTrackingGeometryTool.h"

// ATHENA
#include "GaudiKernel/EventContext.h"

// PACKAGE
#include "ActsGeometry/ActsAlignmentStore.h"
#include "ActsGeometryInterfaces/IActsTrackingGeometrySvc.h"

// STL
#include <iostream>
#include <memory>

ActsTrackingGeometryTool::ActsTrackingGeometryTool(const std::string& type, const std::string& name,
    const IInterface* parent)
  : base_class(type, name, parent),
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

const ActsGeometryContext&
ActsTrackingGeometryTool::getGeometryContext(const EventContext& ctx) const
{
  ATH_MSG_DEBUG("Creating alignment context for event");
  SG::ReadCondHandle<ActsGeometryContext> rch(m_rchk, ctx);

  if(!rch.isValid()) {
    ATH_MSG_ERROR("Creating alignment context failed: read cond handle invalid!");
  }

  return **rch;
}

ActsGeometryContext
ActsTrackingGeometryTool::getNominalGeometryContext() const
{

  ActsGeometryContext gctx;
  gctx.alignmentStore = m_trackingGeometrySvc->getNominalAlignmentStore();

  return gctx;
}
