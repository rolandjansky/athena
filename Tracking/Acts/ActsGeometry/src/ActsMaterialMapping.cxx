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
#include "Acts/Definitions/Units.hpp"

// PACKAGE
#include "ActsInterop/Logger.h"
#include "ActsGeometry/ActsGeometryContext.h"
#include "ActsGeometryInterfaces/IActsMaterialJsonWriterTool.h"
#include "ActsGeometryInterfaces/IActsMaterialStepConverterTool.h"
#include "ActsGeometryInterfaces/IActsMaterialTrackWriterSvc.h"
#include "ActsGeometryInterfaces/IActsTrackingGeometryTool.h"
#include "ActsGeometryInterfaces/IActsSurfaceMappingTool.h"
#include "ActsGeometryInterfaces/IActsVolumeMappingTool.h"

// STL
#include <fstream>
#include <string>

//TEST
#include "Acts/EventData/NeutralTrackParameters.hpp"
#include "Acts/Propagator/ActionList.hpp"
#include "Acts/Propagator/Navigator.hpp"
#include "Acts/Propagator/Propagator.hpp"
#include "Acts/Propagator/StandardAborters.hpp"
#include "Acts/Propagator/StraightLineStepper.hpp"

ActsMaterialMapping::ActsMaterialMapping(const std::string &name,
                                           ISvcLocator *pSvcLocator)
    : AthReentrantAlgorithm(name, pSvcLocator),
      m_materialTrackWriterSvc("ActsMaterialTrackWriterSvc", name),
      m_inputMaterialStepCollection("MaterialStepRecords"),
      m_mappingState(m_gctx,m_mctx),
      m_mappingStateVol(m_gctx,m_mctx)
{}

StatusCode ActsMaterialMapping::initialize() {
  ATH_MSG_DEBUG(name() << "::" << __FUNCTION__);

  if(!m_mapSurfaces && !m_mapVolumes){
    ATH_MSG_ERROR("No element to map onto defined.");
    return StatusCode::FAILURE;
  }

  ATH_CHECK(m_materialStepConverterTool.retrieve() );
  ATH_CHECK(m_materialTrackWriterSvc.retrieve() );
  if(m_mapSurfaces){
    ATH_CHECK(m_surfaceMappingTool.retrieve() );
    m_mappingState = m_surfaceMappingTool->mappingState();
  }
  if(m_mapVolumes){
    ATH_CHECK(m_volumeMappingTool.retrieve() );
    m_mappingStateVol = m_volumeMappingTool->mappingState();
  }
  ATH_CHECK(m_materialJsonWriterTool.retrieve() );
  ATH_CHECK( m_inputMaterialStepCollection.initialize() );
  return StatusCode::SUCCESS;
}

StatusCode ActsMaterialMapping::execute(const EventContext &ctx) const {
  ATH_MSG_VERBOSE(name() << "::" << __FUNCTION__);

  Acts::RecordedMaterialTrack mTrack;
  SG::ReadHandle<Trk::MaterialStepCollection> materialStepCollection(m_inputMaterialStepCollection, ctx);
  mTrack = m_materialStepConverterTool->convertToMaterialTrack(*materialStepCollection);

  if(m_mapSurfaces){
    auto mappingState
         = const_cast<Acts::SurfaceMaterialMapper::State *>(&m_mappingState);
    auto context = m_surfaceMappingTool->trackingGeometryTool()->getNominalGeometryContext().context();
    std::reference_wrapper<const Acts::GeometryContext> geoContext(context);
    mappingState->geoContext = geoContext;

    m_surfaceMappingTool->mapper()->mapMaterialTrack(*mappingState, mTrack);
  }
  if(m_mapVolumes){
    auto mappingStateVol
         = const_cast<Acts::VolumeMaterialMapper::State *>(&m_mappingStateVol);
    auto context = m_volumeMappingTool->trackingGeometryTool()->getNominalGeometryContext().context();
    std::reference_wrapper<const Acts::GeometryContext> geoContext(context);
    mappingStateVol->geoContext = geoContext;       
    m_volumeMappingTool->mapper()->mapMaterialTrack(*mappingStateVol, mTrack);
  }
  m_materialTrackWriterSvc->write(mTrack);
  ATH_MSG_VERBOSE(name() << " execute done");
  return StatusCode::SUCCESS;
}

StatusCode ActsMaterialMapping::finalize() {
  Acts::DetectorMaterialMaps detectorMaterial;

  // Finalize all the maps using the cached state
  if(m_mapSurfaces && m_mapVolumes){
    m_surfaceMappingTool->mapper()->finalizeMaps(m_mappingState);
    m_volumeMappingTool->mapper()->finalizeMaps(m_mappingStateVol);
    // Loop over the state, and collect the maps for surfaces
    for (auto& [key, value] : m_mappingState.surfaceMaterial) {
      detectorMaterial.first.insert({key, std::move(value)});
    }
    // Loop over the state, and collect the maps for volumes
    for (auto& [key, value] : m_mappingStateVol.volumeMaterial) {
      detectorMaterial.second.insert({key, std::move(value)});
    }
  }
  else{
    if(m_mapSurfaces){
      m_surfaceMappingTool->mapper()->finalizeMaps(m_mappingState);
      // Loop over the state, and collect the maps for surfaces
      for (auto& [key, value] : m_mappingState.surfaceMaterial) {
        detectorMaterial.first.insert({key, std::move(value)});
      }
      // Loop over the state, and collect the maps for volumes
      for (auto& [key, value] : m_mappingState.volumeMaterial) {
        detectorMaterial.second.insert({key, std::move(value)});
      }
    }
    if(m_mapVolumes){
      m_volumeMappingTool->mapper()->finalizeMaps(m_mappingStateVol);
      // Loop over the state, and collect the maps for surfaces
      for (auto& [key, value] : m_mappingStateVol.surfaceMaterial) {
        detectorMaterial.first.insert({key, std::move(value)});
      }
      // Loop over the state, and collect the maps for volumes
      for (auto& [key, value] : m_mappingStateVol.volumeMaterial) {
        detectorMaterial.second.insert({key, std::move(value)});
      }
    }
  }
  
  auto context = m_surfaceMappingTool->trackingGeometryTool()->getNominalGeometryContext();
  m_materialJsonWriterTool->write(context, detectorMaterial);

  return StatusCode::SUCCESS;

}
