#include "GaudiKernel/IInterface.h"
#include "GeoPrimitives/GeoPrimitives.h" // Get athena matrix plugin before ACTS'

#include "ACTS/Extrapolation/ExtrapolationCell.hpp" // for excell and ecode
#include "ACTS/Extrapolation/IExtrapolationEngine.hpp" // for the parameters
#include "ACTS/Extrapolation/ExtrapolationEngine.hpp"
#include "ACTS/Extrapolation/RungeKuttaEngine.hpp"
#include "ACTS/Extrapolation/MaterialEffectsEngine.hpp"
#include "ACTS/Extrapolation/StaticNavigationEngine.hpp"
#include "ACTS/Extrapolation/StaticEngine.hpp"
#include "ACTS/Surfaces/Surface.hpp"
#include "ACTS/Surfaces/BoundaryCheck.hpp"

#include "GeomACTS/IExtrapolationTool.h"
#include "GeomACTS/ExtrapolationTool.h"
#include "GeomACTS/ITrackingGeometrySvc.h"
#include "GeomACTS/TrackingGeometrySvc.h"
#include "GeomACTS/ATLASMagneticFieldWrapper.hpp"
#include "GeomACTS/Logger.h"

#include <iostream>

Acts::ExtrapolationTool::ExtrapolationTool(const std::string& type, const std::string& name,
    const IInterface* parent) 
  : IExtrapolationTool(type, name, parent),
    m_fieldServiceHandle("AtlasFieldSvc", name),
    m_trackingGeometrySvc("TrackingGeometrySvc", name)
{

}
  
StatusCode 
Acts::ExtrapolationTool::initialize()
{

  ATH_MSG_INFO("Initializing ACTS extrapolation");

  // we need the field service
  ATH_CHECK( m_fieldServiceHandle.retrieve() );
  ATH_CHECK( m_trackingGeometrySvc.retrieve() );

  auto trackingGeometry = m_trackingGeometrySvc->trackingGeometry();

  using BField_t = ATLASMagneticFieldWrapper;
  auto bField = std::make_shared<ATLASMagneticFieldWrapper>(m_fieldServiceHandle.get());

  // (a) RungeKuttaPropagtator
  using RKEngine = Acts::RungeKuttaEngine<BField_t>;
  typename RKEngine::Config propConfig;
  propConfig.fieldService = bField;
  auto propEngine         = std::make_shared<RKEngine>(propConfig);
  propEngine->setLogger(ACTS_ATH_LOGGER("RungeKuttaEngine"));
  // (b) MaterialEffectsEngine
  Acts::MaterialEffectsEngine::Config matConfig;
  auto                                materialEngine
    = std::make_shared<Acts::MaterialEffectsEngine>(matConfig);
  materialEngine->setLogger(
      ACTS_ATH_LOGGER("MaterialEffectsEngine"));
  // (c) StaticNavigationEngine
  Acts::StaticNavigationEngine::Config navConfig;
  navConfig.propagationEngine     = propEngine;
  navConfig.materialEffectsEngine = materialEngine;
  navConfig.trackingGeometry      = trackingGeometry;
  auto navEngine = std::make_shared<Acts::StaticNavigationEngine>(navConfig);
  navEngine->setLogger(ACTS_ATH_LOGGER("NavigationEngine"));
  // (d) the StaticEngine
  Acts::StaticEngine::Config statConfig;
  statConfig.propagationEngine     = propEngine;
  statConfig.navigationEngine      = navEngine;
  statConfig.materialEffectsEngine = materialEngine;
  auto statEngine = std::make_shared<Acts::StaticEngine>(statConfig);
  statEngine->setLogger(ACTS_ATH_LOGGER("StaticEngine"));
  // (e) the material engine
  Acts::ExtrapolationEngine::Config exEngineConfig;
  exEngineConfig.trackingGeometry     = trackingGeometry;
  exEngineConfig.propagationEngine    = propEngine;
  exEngineConfig.navigationEngine     = navEngine;
  exEngineConfig.extrapolationEngines = {statEngine};

  m_exEngine = std::make_unique<Acts::ExtrapolationEngine>(exEngineConfig);
  m_exEngine->setLogger(ACTS_ATH_LOGGER("ExtrapolationEngine"));

  ATH_MSG_INFO("ACTS extrapolation successfully initialized");
  return StatusCode::SUCCESS;
}

Acts::ExtrapolationCode
Acts::ExtrapolationTool::extrapolate(Acts::ExCellCharged&       ecCharged,
              const Surface*       sf,
              const BoundaryCheck& bcheck) const 
{
  return m_exEngine->extrapolate(ecCharged, sf, bcheck);
}



Acts::ExtrapolationCode
Acts::ExtrapolationTool::extrapolate(Acts::ExCellNeutral&       ecNeutral,
              const Surface*       sf,
              const BoundaryCheck& bcheck) const 
{
  return m_exEngine->extrapolate(ecNeutral, sf, bcheck);
}
