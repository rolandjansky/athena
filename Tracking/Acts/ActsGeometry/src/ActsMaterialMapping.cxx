/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "ActsGeometry/ActsMaterialMapping.h"

// ATHENA
#include "Acts/Surfaces/PerigeeSurface.hpp"
#include "Acts/Utilities/Logger.hpp"
#include "ActsGeometry/IActsPropStepRootWriterSvc.h"
#include "GaudiKernel/EventContext.h"
#include "GaudiKernel/ISvcLocator.h"

// ACTS
#include "Acts/Propagator/detail/SteppingLogger.hpp"
#include "Acts/Utilities/Helpers.hpp"
#include "Acts/Utilities/Units.hpp"

// PACKAGE
#include "ActsInterop/Logger.h"
#include "ActsGeometry/ActsGeometryContext.h"
#include "ActsGeometryInterfaces/IActsMaterialJsonWriterTool.h"
#include "ActsGeometryInterfaces/IActsMaterialStepConverterTool.h"
#include "ActsGeometryInterfaces/IActsMaterialTrackWriterSvc.h"
#include "ActsGeometryInterfaces/IActsTrackingGeometryTool.h"
#include "ActsGeometryInterfaces/IActsSurfaceMappingTool.h"

// STL
#include <fstream>
#include <string>

//TEST
#include "Acts/EventData/NeutralTrackParameters.hpp"
#include "Acts/Propagator/ActionList.hpp"
#include "Acts/Propagator/DebugOutputActor.hpp"
#include "Acts/Propagator/Navigator.hpp"
#include "Acts/Propagator/Propagator.hpp"
#include "Acts/Propagator/StandardAborters.hpp"
#include "Acts/Propagator/StraightLineStepper.hpp"

ActsMaterialMapping::ActsMaterialMapping(const std::string &name,
                                           ISvcLocator *pSvcLocator)
    : AthReentrantAlgorithm(name, pSvcLocator),
      m_materialTrackWriterSvc("ActsMaterialTrackWriterSvc", name),
      m_inputMaterialStepCollection("MaterialStepRecords"),
      m_mappingState(m_gctx,m_mctx)
{}

StatusCode ActsMaterialMapping::initialize() {

  ATH_MSG_DEBUG(name() << "::" << __FUNCTION__);

  ATH_CHECK(m_materialStepConverterTool.retrieve() );
  ATH_CHECK(m_materialTrackWriterSvc.retrieve() );
  ATH_CHECK(m_surfaceMappingTool.retrieve() );
  ATH_CHECK(m_materialJsonWriterTool.retrieve() );

  ATH_CHECK( m_inputMaterialStepCollection.initialize() );

  m_mappingState = m_surfaceMappingTool->mappingState();

  return StatusCode::SUCCESS;
}

StatusCode ActsMaterialMapping::execute(const EventContext &ctx) const {

  ATH_MSG_VERBOSE(name() << "::" << __FUNCTION__);

  m_surfaceMappingTool->trackingGeometryTool()->getGeometryContext(ctx);

  Acts::RecordedMaterialTrack mTrack;

  SG::ReadHandle<Trk::MaterialStepCollection> materialStepCollection(m_inputMaterialStepCollection, ctx);

  mTrack = m_materialStepConverterTool->convertToMaterialTrack(*materialStepCollection);

  m_materialTrackWriterSvc->write(mTrack);

  auto mappingState
        = const_cast<Acts::SurfaceMaterialMapper::State*>(&m_mappingState);


  m_surfaceMappingTool->mapper()->mapMaterialTrack(*mappingState, mTrack);

  ATH_MSG_VERBOSE(name() << " execute done");

  return StatusCode::SUCCESS;
}

StatusCode ActsMaterialMapping::finalize() {

  // Finalize all the maps using the cached state
  m_surfaceMappingTool->mapper()->finalizeMaps(m_mappingState);

  Acts::DetectorMaterialMaps detectorMaterial;

  // Loop over the state, and collect the maps for surfaces
  for (auto& [key, value] : m_mappingState.surfaceMaterial) {
    detectorMaterial.first.insert({key, std::move(value)});
  }

  m_materialJsonWriterTool->write(detectorMaterial);

  return StatusCode::SUCCESS;

}
