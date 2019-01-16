/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#include "ActsGeometry/ActsExtrapolationAlg.h"

// ATHENA
#include "MagFieldInterfaces/IMagFieldSvc.h"
#include "AthenaKernel/RNGWrapper.h"
#include "Acts/Utilities/Logger.hpp"
#include "Acts/Surfaces/PerigeeSurface.hpp"
#include "GaudiKernel/EventContext.h"
#include "GaudiKernel/ISvcLocator.h"
#include "AthenaKernel/IAthRNGSvc.h"

// ACTS
#include "Acts/Utilities/Helpers.hpp"
#include "Acts/Propagator/detail/SteppingLogger.hpp"
#include "ActsGeometry/IActsPropStepRootWriterSvc.h"

// PACKAGE
#include "ActsGeometry/ActsExtrapolationTool.h"
#include "ActsInterop/Logger.h"
//#include "ActsGeometry/IActsMaterialTrackWriterSvc.h"

// OTHER
#include "CLHEP/Random/RandomEngine.h"

// STL
#include <string>
#include <fstream>


ActsExtrapolationAlg::ActsExtrapolationAlg(const std::string& name,
                                 ISvcLocator* pSvcLocator)
    : AthReentrantAlgorithm(name, pSvcLocator),
      m_propStepWriterSvc("ActsPropStepRootWriterSvc", name),
      m_rndmGenSvc("AthRNGSvc", name)//,
      //m_materialTrackWriterSvc("ActsMaterialTrackWriterSvc", name)
{
}

StatusCode ActsExtrapolationAlg::initialize() {

  ATH_MSG_DEBUG(name() << "::" << __FUNCTION__);

  ATH_CHECK( m_rndmGenSvc.retrieve() );
  ATH_CHECK( m_extrapolationTool.retrieve() );
  ATH_CHECK( m_propStepWriterSvc.retrieve() );

  //if (m_writeMaterialTracks) {
    //ATH_CHECK( m_materialTrackWriterSvc.retrieve() );
  //}

  m_objOut = std::make_unique<std::ofstream>("steps.obj");

  return StatusCode::SUCCESS;
}

StatusCode ActsExtrapolationAlg::execute(const EventContext& ctx) const 
{

  ATH_MSG_VERBOSE(name() << "::" << __FUNCTION__);

  m_extrapolationTool->prepareAlignment();
  ATHRNG::RNGWrapper* rngWrapper = m_rndmGenSvc->getEngine(this);
  rngWrapper->setSeed( name(), ctx );
  CLHEP::HepRandomEngine* rngEngine = rngWrapper->getEngine(ctx);

  // one particle per event for now
  // since the ATLAS ParticleGun seems not to work on MT, we roll
  // our own for now.
  double d0 = 0;
  double z0 = 0;
  double phi = rngEngine->flat() * 2*M_PI - M_PI;
  std::vector<double> etaRange = m_etaRange;
  double etaMin = etaRange.at(0);
  double etaMax = etaRange.at(1);
  double eta = rngEngine->flat() * std::abs(etaMax - etaMin) + etaMin;

  std::vector<double> ptRange = m_ptRange;
  double ptMin = ptRange.at(0) * Acts::units::_GeV;
  double ptMax = ptRange.at(1) * Acts::units::_GeV;

  double pt = rngEngine->flat() * std::abs(ptMax - ptMin) + ptMin;

  Acts::Vector3D momentum(
      pt * std::cos(phi), pt * std::sin(phi), pt * std::sinh(eta));

  double theta = Acts::VectorHelpers::theta(momentum);

  double charge = rngEngine->flat() > 0.5 ? -1 : 1;

  double qop =  charge / momentum.norm();
    
  std::shared_ptr<Acts::PerigeeSurface> surface 
    = Acts::Surface::makeShared<Acts::PerigeeSurface>(Acts::Vector3D(0, 0, 0));


  Acts::ActsVectorD<5> pars;
  pars << d0, z0, phi, theta, qop;
  std::unique_ptr<Acts::ActsSymMatrixD<5>> cov = nullptr;
      
  std::vector<Acts::detail::Step> steps;

  if(charge != 0.) {
      // charged extrapolation - with hit recording
      Acts::BoundParameters startParameters(
          std::move(cov), std::move(pars), std::move(surface));
      steps = m_extrapolationTool->propagate(startParameters);
      writeStepsObj(steps);
      m_propStepWriterSvc->write(steps);
  }


  


  ATH_MSG_VERBOSE(name() << " execute done");

  return StatusCode::SUCCESS;
}

StatusCode ActsExtrapolationAlg::finalize() {
  return StatusCode::SUCCESS;
}

void ActsExtrapolationAlg::writeStepsObj(std::vector<Acts::detail::Step> steps) const
{
  std::lock_guard<std::mutex> lock(m_writeMutex);

  std::ofstream& out = *m_objOut;
  std::stringstream lstr;
  lstr << "l";
  for(const auto& step : steps) {
    const auto& pos = step.position;
    out << "v " << pos.x() << " " << pos.y() << " " << pos.z() << std::endl;
    lstr << " " << m_objVtxCount;
    m_objVtxCount++;
  }

  lstr << std::endl;

  out << lstr.str() << std::endl;
}

