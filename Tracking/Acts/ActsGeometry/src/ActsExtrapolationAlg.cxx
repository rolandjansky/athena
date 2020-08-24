/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "ActsGeometry/ActsExtrapolationAlg.h"

// ATHENA
#include "AthenaKernel/IAthRNGSvc.h"
#include "AthenaKernel/RNGWrapper.h"
#include "GaudiKernel/EventContext.h"
#include "GaudiKernel/ISvcLocator.h"

// ACTS
#include "Acts/Propagator/MaterialInteractor.hpp"
#include "Acts/Propagator/detail/SteppingLogger.hpp"
#include "Acts/Surfaces/PerigeeSurface.hpp"
#include "Acts/Utilities/Helpers.hpp"
#include "Acts/Utilities/Units.hpp"
#include "Acts/Utilities/Logger.hpp"

// PACKAGE
#include "ActsGeometry/ActsGeometryContext.h"
#include "ActsGeometry/IActsPropStepRootWriterSvc.h"
#include "ActsGeometryInterfaces/IActsExtrapolationTool.h"
#include "ActsGeometryInterfaces/IActsMaterialTrackWriterSvc.h"
#include "ActsGeometryInterfaces/IActsTrackingGeometryTool.h"
#include "ActsInterop/Logger.h"

// OTHER
#include "CLHEP/Random/RandomEngine.h"

// STL
#include <fstream>
#include <string>

using namespace Acts::UnitLiterals;

namespace Acts{
  /// Recorded material track
  /// - this is start:  position, start momentum
  ///   and the Recorded material
  using RecordedMaterialTrack =
      std::pair<std::pair<Acts::Vector3D, Acts::Vector3D>, RecordedMaterial>;
}

ActsExtrapolationAlg::ActsExtrapolationAlg(const std::string &name,
                                           ISvcLocator *pSvcLocator)
    : AthReentrantAlgorithm(name, pSvcLocator),
      m_propStepWriterSvc("ActsPropStepRootWriterSvc", name),
      m_rndmGenSvc("AthRNGSvc", name) ,
      m_materialTrackWriterSvc("ActsMaterialTrackWriterSvc", name)
{}

StatusCode ActsExtrapolationAlg::initialize() {

  ATH_MSG_DEBUG(name() << "::" << __FUNCTION__);

  ATH_CHECK(m_rndmGenSvc.retrieve());
  ATH_CHECK(m_extrapolationTool.retrieve());
  ATH_CHECK(m_propStepWriterSvc.retrieve());

  if (m_writeMaterialTracks) {
  ATH_CHECK( m_materialTrackWriterSvc.retrieve() );
  }

  m_objOut = std::make_unique<std::ofstream>("steps.obj");

  return StatusCode::SUCCESS;
}

StatusCode ActsExtrapolationAlg::execute(const EventContext &ctx) const {

  ATH_MSG_VERBOSE(name() << "::" << __FUNCTION__);

  ATHRNG::RNGWrapper *rngWrapper = m_rndmGenSvc->getEngine(this);
  rngWrapper->setSeed(name(), ctx);
  CLHEP::HepRandomEngine *rngEngine = rngWrapper->getEngine(ctx);

  ATH_MSG_VERBOSE("Extrapolating " << m_nParticlePerEvent << " particles");

  for (size_t i = 0; i < m_nParticlePerEvent; i++) {
    double d0 = 0;
    double z0 = 0;
    double phi = rngEngine->flat() * 2 * M_PI - M_PI;
    std::vector<double> etaRange = m_etaRange;
    double etaMin = etaRange.at(0);
    double etaMax = etaRange.at(1);
    double eta = rngEngine->flat() * std::abs(etaMax - etaMin) + etaMin;

    std::vector<double> ptRange = m_ptRange;
    double ptMin = ptRange.at(0) * 1_GeV;
    double ptMax = ptRange.at(1) * 1_GeV;

    double pt = rngEngine->flat() * std::abs(ptMax - ptMin) + ptMin;

    Acts::Vector3D momentum(pt * std::cos(phi), pt * std::sin(phi),
                            pt * std::sinh(eta));

    double theta = Acts::VectorHelpers::theta(momentum);

    double charge = rngEngine->flat() > 0.5 ? -1 : 1;

    double qop = charge / momentum.norm();

    std::shared_ptr<Acts::PerigeeSurface> surface =
        Acts::Surface::makeShared<Acts::PerigeeSurface>(
            Acts::Vector3D(0, 0, 0));

    double t = 0;
    ATH_MSG_VERBOSE("Pseudo-particle: eta: " << eta << " phi: " << phi);

    Acts::BoundVector pars;
    pars << d0, z0, phi, theta, qop, t;
    std::optional<Acts::BoundSymMatrix> cov = std::nullopt;

    ActsPropagationOutput output;

    if (charge != 0.) {
      // Perigee, no alignment -> default geo context
      ActsGeometryContext gctx = m_extrapolationTool->trackingGeometryTool()
                                     ->getNominalGeometryContext();
      auto anygctx = gctx.any();
      Acts::BoundParameters startParameters(
          anygctx, std::move(cov), std::move(pars), std::move(surface));
      output = m_extrapolationTool->propagationSteps(ctx, startParameters);
      if(output.first.size() == 0) {
        ATH_MSG_WARNING("Got ZERO steps from the extrapolation tool");
      }
      m_propStepWriterSvc->write(output.first);
      if(m_writeMaterialTracks){
        Acts::RecordedMaterialTrack track;
        track.first.first = Acts::Vector3D(0,0,0);
        track.first.second = momentum;
        track.second = std::move(output.second);
        m_materialTrackWriterSvc->write(track);
      }
    }

    ATH_MSG_VERBOSE(name() << " execute done");
  }

  return StatusCode::SUCCESS;
}

StatusCode ActsExtrapolationAlg::finalize() { return StatusCode::SUCCESS; }

void ActsExtrapolationAlg::writeStepsObj(
    std::vector<Acts::detail::Step> steps) const {
  std::lock_guard<std::mutex> lock(m_writeMutex);

  std::ofstream &out = *m_objOut;
  std::stringstream lstr;
  lstr << "l";
  for (const auto &step : steps) {
    const auto &pos = step.position;
    out << "v " << pos.x() << " " << pos.y() << " " << pos.z() << std::endl;
    lstr << " " << m_objVtxCount;
    m_objVtxCount++;
  }

  lstr << std::endl;

  out << lstr.str() << std::endl;
}
