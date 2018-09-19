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
// ACTS
#include "Acts/Plugins/MaterialMapping/MaterialTrack.hpp"

// PACKAGE
#include "ActsGeometry/ActsExtrapolationTool.h"
#include "ActsGeometry/IActsExCellWriterSvc.h"
#include "ActsInterop/Logger.h"
#include "ActsGeometry/IActsMaterialTrackWriterSvc.h"

// OTHER
#include "CLHEP/Random/RandomEngine.h"

// STL
#include <string>


ActsExtrapolationAlg::ActsExtrapolationAlg(const std::string& name,
                                 ISvcLocator* pSvcLocator)
    : AthReentrantAlgorithm(name, pSvcLocator),
      m_exCellWriterSvc("ActsExCellWriterSvc", name),
      m_rndmGenSvc("AthRNGSvc", name),
      m_materialTrackWriterSvc("ActsMaterialTrackWriterSvc", name)
{
}

StatusCode ActsExtrapolationAlg::initialize() {

  ATH_MSG_DEBUG(name() << "::" << __FUNCTION__);

  ATH_CHECK( m_exCellWriterSvc.retrieve() );
  ATH_CHECK( m_rndmGenSvc.retrieve() );
  ATH_CHECK( m_extrapolationTool.retrieve() );

  if (m_writeMaterialTracks) {
    ATH_CHECK( m_materialTrackWriterSvc.retrieve() );
  }

  return StatusCode::SUCCESS;
}

StatusCode ActsExtrapolationAlg::execute_r(const EventContext& ctx) const 
{

  ATH_MSG_VERBOSE(name() << "::" << __FUNCTION__);

  m_extrapolationTool->prepareAlignment();
  ATHRNG::RNGWrapper* rngWrapper = m_rndmGenSvc->getEngine(this);
  rngWrapper->setSeed( name(), ctx );
  CLHEP::HepRandomEngine* rngEngine = rngWrapper->getEngine(ctx);

  // one particle per event for now
  // since the ATLAS ParticleGun seems not to working on MT, we roll
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

  double theta = momentum.theta();

  double charge = rngEngine->flat() > 0.5 ? -1 : 1;

  double qop =  charge / momentum.norm();
    
  Acts::PerigeeSurface surface(Acts::Vector3D(0, 0, 0));

  std::vector<Acts::ExtrapolationCell<Acts::TrackParameters>> ecells;

  Acts::ActsVectorD<5> pars;
  pars << d0, z0, phi, theta, qop;
  std::unique_ptr<Acts::ActsSymMatrixD<5>> cov = nullptr;

  if(charge != 0.) {
      // charged extrapolation - with hit recording
      Acts::BoundParameters startParameters(
          std::move(cov), std::move(pars), surface);
      Acts::ExtrapolationCell<Acts::TrackParameters> ecc(startParameters);

      ecc.searchMode = m_searchMode;

      if (m_stopAtBoundary) {
      ATH_MSG_VERBOSE("StopAtBoundary set");
      ecc.addConfigurationMode(Acts::ExtrapolationMode::StopAtBoundary);
      }

      if (m_FATRAS) {
      ATH_MSG_VERBOSE("FATRAS set");
      ecc.addConfigurationMode(Acts::ExtrapolationMode::FATRAS);
      }

      if (m_collectSensitive) {
      ATH_MSG_VERBOSE("CollectSensitive set");
      ecc.addConfigurationMode(Acts::ExtrapolationMode::CollectSensitive);
      }

      if (m_collectPassive) {
        ATH_MSG_VERBOSE("CollectPassive set");
        ecc.addConfigurationMode(Acts::ExtrapolationMode::CollectPassive);
      }

      if (m_collectBoundary) {
        ATH_MSG_VERBOSE("CollectBoundary set");
        ecc.addConfigurationMode(Acts::ExtrapolationMode::CollectBoundary);
      }

      if (m_collectMaterial) {
        ATH_MSG_VERBOSE("CollectMaterial set");
        ecc.addConfigurationMode(Acts::ExtrapolationMode::CollectMaterial);
      }

      m_extrapolationTool->extrapolate(ecc);

      if (m_writeMaterialTracks) {
        Acts::MaterialTrack mTrack = makeMaterialTrack(ecc);
        m_materialTrackWriterSvc->write(std::move(mTrack));
      }

      ecells.push_back(std::move(ecc));
  }

  m_exCellWriterSvc->store(ecells);

  ATH_MSG_VERBOSE(name() << " execute done");

  return StatusCode::SUCCESS;
}

StatusCode ActsExtrapolationAlg::finalize() {
  return StatusCode::SUCCESS;
}

