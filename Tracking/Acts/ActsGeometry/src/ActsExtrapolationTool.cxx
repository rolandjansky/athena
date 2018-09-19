/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#include "ActsGeometry/ActsExtrapolationTool.h"

// ATHENA
#include "GaudiKernel/IInterface.h"
#include "GeoPrimitives/GeoPrimitives.h"

// PACKAGE
#include "ActsGeometry/ActsTrackingGeometrySvc.h"
#include "ActsGeometry/ATLASMagneticFieldWrapper.h"
#include "ActsInterop/Logger.h"

// ACTS
#include "Acts/Extrapolation/ExtrapolationCell.hpp" // for excell and ecode
#include "Acts/Extrapolation/IExtrapolationEngine.hpp" // for the parameters
#include "Acts/Extrapolation/ExtrapolationEngine.hpp"
#include "Acts/Extrapolation/RungeKuttaEngine.hpp"
#include "Acts/Extrapolation/MaterialEffectsEngine.hpp"
#include "Acts/Extrapolation/StaticNavigationEngine.hpp"
#include "Acts/Extrapolation/StaticEngine.hpp"
#include "Acts/Surfaces/Surface.hpp"
#include "Acts/Surfaces/BoundaryCheck.hpp"

// STL
#include <iostream>
#include <memory>

ActsExtrapolationTool::ActsExtrapolationTool(const std::string& type, const std::string& name,
    const IInterface* parent) 
  : AthAlgTool(type, name, parent),
    m_fieldServiceHandle("AtlasFieldSvc", name)
{

}
  
StatusCode 
ActsExtrapolationTool::initialize()
{
  using namespace std::literals::string_literals;


  ATH_MSG_INFO("Initializing ACTS extrapolation");

  // (a) RungeKuttaPropagator
  std::shared_ptr<const Acts::IPropagationEngine> propEngine;
  if (m_fieldMode == "ATLAS"s) {
    // we need the field service
    ATH_CHECK( m_fieldServiceHandle.retrieve() );
    ATH_MSG_INFO("Using ATLAS magnetic field service");
    using BField_t = ATLASMagneticFieldWrapper;
    auto bField = std::make_shared<ATLASMagneticFieldWrapper>(m_fieldServiceHandle.get());

    using RKEngine = Acts::RungeKuttaEngine<BField_t>;
    typename RKEngine::Config propConfig;
    propConfig.fieldService = bField;
    auto propEngine_ = std::make_shared<RKEngine>(propConfig);
    propEngine_->setLogger(Acts::makeAthenaLogger(this, "RKEngine", "ActsExTool"));
    propEngine = propEngine_;
  }
  else if (m_fieldMode == "Constant") {
    std::vector<double> constantFieldVector = m_constantFieldVector;
    double Bx = constantFieldVector.at(0);
    double By = constantFieldVector.at(1);
    double Bz = constantFieldVector.at(2);
    ATH_MSG_INFO("Using constant magnetic field: (Bx, By, Bz) = (" << Bx << ", " << By << ", " << Bz << ")");
    using BField_t = Acts::ConstantBField;
    auto bField = std::make_shared<BField_t>(Bx, By, Bz);

    using RKEngine = Acts::RungeKuttaEngine<BField_t>;
    typename RKEngine::Config propConfig;
    propConfig.fieldService = bField;
    auto propEngine_ = std::make_shared<RKEngine>(propConfig);
    propEngine_->setLogger(Acts::makeAthenaLogger(this, "RKEngine", "ActsExTool"));
    propEngine = propEngine_;
  }

  ATH_CHECK( m_trackingGeometryTool.retrieve() );
  auto trackingGeometry = m_trackingGeometryTool->trackingGeometry();


  // (b) MaterialEffectsEngine
  Acts::MaterialEffectsEngine::Config matConfig;
  matConfig.eLossCorrection = m_matEngineDoELossCorrection;
  auto                                materialEngine
    = std::make_shared<Acts::MaterialEffectsEngine>(matConfig);
  materialEngine->setLogger(
      Acts::makeAthenaLogger(this, "MatFXEngine", "ActsExTool"));
  // (c) StaticNavigationEngine
  Acts::StaticNavigationEngine::Config navConfig;
  navConfig.propagationEngine     = propEngine;
  navConfig.materialEffectsEngine = materialEngine;
  navConfig.trackingGeometry      = trackingGeometry;
  auto navEngine = std::make_shared<Acts::StaticNavigationEngine>(navConfig);
  navEngine->setLogger(Acts::makeAthenaLogger(this, "NavEngine", "ActsExTool"));
  // (d) the StaticEngine
  Acts::StaticEngine::Config statConfig;
  statConfig.propagationEngine     = propEngine;
  statConfig.navigationEngine      = navEngine;
  statConfig.materialEffectsEngine = materialEngine;
  auto statEngine = std::make_shared<Acts::StaticEngine>(statConfig);
  statEngine->setLogger(Acts::makeAthenaLogger(this, "StaticEngine", "ActsExTool"));
  // (e) the material engine
  Acts::ExtrapolationEngine::Config exEngineConfig;
  exEngineConfig.trackingGeometry     = trackingGeometry;
  exEngineConfig.propagationEngine    = propEngine;
  exEngineConfig.navigationEngine     = navEngine;
  exEngineConfig.extrapolationEngines = {statEngine};

  m_exEngine = std::make_shared<Acts::ExtrapolationEngine>(exEngineConfig);
  m_exEngine->setLogger(Acts::makeAthenaLogger(this, "ExEngine", "ActsExTool"));

  ATH_MSG_INFO("ACTS extrapolation successfully initialized");
  return StatusCode::SUCCESS;
}

Acts::ExtrapolationCode
ActsExtrapolationTool::extrapolate(Acts::ExCellCharged&       ecCharged,
              const Acts::Surface*       sf,
              const Acts::BoundaryCheck& bcheck) const 
{
  return m_exEngine->extrapolate(ecCharged, sf, bcheck);
}



Acts::ExtrapolationCode
ActsExtrapolationTool::extrapolate(Acts::ExCellNeutral&       ecNeutral,
              const Acts::Surface*       sf,
              const Acts::BoundaryCheck& bcheck) const 
{
  return m_exEngine->extrapolate(ecNeutral, sf, bcheck);
}

std::shared_ptr<Acts::IExtrapolationEngine>
ActsExtrapolationTool::extrapolationEngine() const 
{
  return std::dynamic_pointer_cast<Acts::IExtrapolationEngine>(m_exEngine);
}
