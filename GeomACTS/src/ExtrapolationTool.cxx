#include "GaudiKernel/IInterface.h"

#include "ACTS/Extrapolation/ExtrapolationCell.hpp" // for excell and ecode
//#include "ACTS/EventData/NeutralParameters.hpp"
//#include "ACTS/EventData/TrackParameters.hpp"
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

  ATH_MSG_VERBOSE("Initializing ACTS extrapolation");

  // we need the field service
  ATH_CHECK( m_fieldServiceHandle.retrieve() );
  ATH_CHECK( m_trackingGeometrySvc.retrieve() );

  auto trackingGeometry = m_trackingGeometrySvc->trackingGeometry();

  using BField_t = ATLASMagneticFieldWrapper;
  auto bField = std::make_shared<ATLASMagneticFieldWrapper>(m_fieldServiceHandle.get());

  Acts::Logging::Level extrapLogLevel = Acts::Logging::INFO;

  // (a) RungeKuttaPropagtator
  using RKEngine = Acts::RungeKuttaEngine<BField_t>;
  typename RKEngine::Config propConfig;
  propConfig.fieldService = bField;
  auto propEngine         = std::make_shared<RKEngine>(propConfig);
  propEngine->setLogger(Acts::getDefaultLogger("RungeKuttaEngine", extrapLogLevel));
  // (b) MaterialEffectsEngine
  Acts::MaterialEffectsEngine::Config matConfig;
  auto                                materialEngine
    = std::make_shared<Acts::MaterialEffectsEngine>(matConfig);
  materialEngine->setLogger(
      Acts::getDefaultLogger("MaterialEffectsEngine", extrapLogLevel));
  // (c) StaticNavigationEngine
  Acts::StaticNavigationEngine::Config navConfig;
  navConfig.propagationEngine     = propEngine;
  navConfig.materialEffectsEngine = materialEngine;
  navConfig.trackingGeometry      = trackingGeometry;
  auto navEngine = std::make_shared<Acts::StaticNavigationEngine>(navConfig);
  navEngine->setLogger(Acts::getDefaultLogger("NavigationEngine", extrapLogLevel));
  // (d) the StaticEngine
  Acts::StaticEngine::Config statConfig;
  statConfig.propagationEngine     = propEngine;
  statConfig.navigationEngine      = navEngine;
  statConfig.materialEffectsEngine = materialEngine;
  auto statEngine = std::make_shared<Acts::StaticEngine>(statConfig);
  statEngine->setLogger(Acts::getDefaultLogger("StaticEngine", extrapLogLevel));
  // (e) the material engine
  Acts::ExtrapolationEngine::Config exEngineConfig;
  exEngineConfig.trackingGeometry     = trackingGeometry;
  exEngineConfig.propagationEngine    = propEngine;
  exEngineConfig.navigationEngine     = navEngine;
  exEngineConfig.extrapolationEngines = {statEngine};

  m_exEngine = std::make_unique<Acts::ExtrapolationEngine>(exEngineConfig);
  m_exEngine->setLogger(Acts::getDefaultLogger("ExtrapolationEngine", extrapLogLevel));

  ATH_MSG_VERBOSE("ACTS extrapolation successfully initialized");
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
