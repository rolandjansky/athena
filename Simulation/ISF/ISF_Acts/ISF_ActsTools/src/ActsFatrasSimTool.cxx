/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/
#include "ActsFatrasSimTool.h"

#include <algorithm>
#include <random>

#include "ActsGeometry/ActsExtrapolationTool.h"
#include "ActsGeometry/ActsTrackingGeometryTool.h"
#include "ActsGeometryInterfaces/IActsTrackingGeometryTool.h"
#include "ActsInterop/Logger.h"
#include "ISF_Event/ISFParticle.h"
#include "ISF_FatrasInterfaces/ISimHitCreator.h"

// ACTS
#include "Acts/EventData/SingleCurvilinearTrackParameters.hpp"
#include "Acts/Propagator/AbortList.hpp"
#include "Acts/Propagator/ActionList.hpp"
#include "Acts/Propagator/EigenStepper.hpp"
#include "Acts/Propagator/Navigator.hpp"
#include "Acts/Propagator/Propagator.hpp"
#include "Acts/Propagator/detail/SteppingLogger.hpp"
#include "ActsFatras/Kernel/Simulator.hpp"
// BOOST
#include <boost/variant/apply_visitor.hpp>
#include <boost/variant/static_visitor.hpp>
#include <boost/variant/variant.hpp>

#include "CLHEP/Random/RandFlat.h"
#include "CLHEP/Units/PhysicalConstants.h"
#include "CLHEP/Units/SystemOfUnits.h"

using namespace Acts::UnitLiterals;

ISF::ActsFatrasSimTool::ActsFatrasSimTool(const std::string& type,
                                          const std::string& name,
                                          const IInterface* parent)
    : base_class(type, name, parent) {
  //  declareProperty("TrackingGeometryTool", m_trackingGeometryTool);
  //  declareProperty("ActsExtrapolationTool", m_actsExtrapolationTool);
  // hit creation tools
  declareProperty("SimHitCreatorID", m_simHitCreatorID);
  //  declareProperty( "SimHitCreatorMS", m_simHitCreatorMS);
}

ISF::ActsFatrasSimTool::~ActsFatrasSimTool() {}

StatusCode ISF::ActsFatrasSimTool::initialize() {
  ATH_CHECK(m_trackingGeometryTool.retrieve());
  ATH_CHECK(m_actsExtrapolationTool.retrieve());

  m_trackingGeometry = m_trackingGeometryTool->trackingGeometry();
  m_geoCtx = m_trackingGeometryTool->getNominalGeometryContext();
  m_logger =
      makeActsAthenaLogger(this, "ActsFatrasSimTool", "ActsFatrasSimTool");

  // retreive hit creators
  if (retrieveTool<iFatras::ISimHitCreator>(m_simHitCreatorID).isFailure())
    return StatusCode::FAILURE;

  return StatusCode::SUCCESS;
}

StatusCode ISF::ActsFatrasSimTool::finalize() { return StatusCode::SUCCESS; }

ISF::ISFParticle* ISF::ActsFatrasSimTool::process(
    const ISFParticle& isp, CLHEP::HepRandomEngine*) const {
  // copy the current particle onto the particle clipboard
  // ISF::ParticleClipboard::getInstance().setParticle(isp);

  // give a screen output that you entered ActsFatrasSimSvc
  ATH_MSG_INFO("Particle " << isp << " set for simulation.");
  ATH_MSG_INFO("Called on particle " << isp);

  // prepare simulation call parameters
  // random number generator
  Generator generator;
  // input/ output particle and hits containers
  std::vector<ActsFatras::Particle> input;
  std::vector<ActsFatras::Particle> simulatedInitial;
  std::vector<ActsFatras::Particle> simulatedFinal;
  std::vector<ActsFatras::Hit> hits;

  // create input particles. particle number should ne non-zero.
  // just create 1 particle to test
  const auto pid =
      ActsFatras::Barcode().setVertexPrimary(42).setParticle(isp.barcode());
  input.emplace_back(pid, (Acts::PdgParticle)isp.pdgCode(),
                                       isp.charge(), isp.mass() * 10e-3)
                      .setDirection(Acts::makeDirectionUnitFromPhiEta(
                          isp.momentum().phi(), isp.momentum().eta()))
                      .setAbsoluteMomentum(isp.momentum().mag() * 10e-3)
                      .setPosition4(isp.position().x(), isp.position().y(),
                                    isp.position().z(), isp.timeStamp());
  auto &particle = input.back();

  auto gctx = m_geoCtx.context();

  // Action list and abort list
  using SteppingLogger = Acts::detail::SteppingLogger;
  using EndOfWorld = Acts::EndOfWorldReached;

  using ActionList = Acts::ActionList<SteppingLogger, Acts::MaterialInteractor>;
  using AbortConditions = Acts::AbortList<EndOfWorld>;
  using Options =
      Acts::DenseStepperPropagatorOptions<ActionList, AbortConditions>;

  Options options(gctx, m_magCtx, Acts::LoggerWrapper{*m_logger});
  options.pathLimit = 100_cm;

  Acts::SingleCurvilinearTrackParameters<Acts::AnyCharge> start(
      particle.fourPosition(), particle.unitDirection(), particle.absoluteMomentum(),
      particle.charge());

  options.loopProtection =
      (Acts::VectorHelpers::perp(start.momentum()) < m_ptLoopers * 1_MeV);
  options.maxStepSize = m_maxStepSize * 1_m;
  options.maxSteps = m_maxStep;
  options.direction = Acts::forward;

  auto& mInteractor = options.actionList.get<Acts::MaterialInteractor>();
  mInteractor.multipleScattering = m_interactionMultiScatering;
  mInteractor.energyLoss = m_interactionEloss;
  mInteractor.recordInteractions = m_interactionRecord;

  auto result = m_chargedPropagator.propagate(start, options);

  if (result.ok()) {
    ATH_MSG_DEBUG("Particle charge:  " << particle.charge());
    ATH_MSG_DEBUG("Particle mass: " << particle.mass());
    ATH_MSG_DEBUG("Particle pT: " << particle.transverseMomentum());
    ATH_MSG_DEBUG("Particle position: " << particle.position()[0] << " "
                                       << particle.position()[1] << " "
                                       << particle.position()[2]);
    ATH_MSG_DEBUG("Result pathLength:  " << result.value().pathLength);
  }

  /** record hits here some how **/
  int hitCounter = 0;

  for (auto hit : hits) {
    hitCounter++;
    // TODO      //convert to athena hits and record hits
    ATH_MSG_DEBUG("Hit counter is " << hitCounter << " and Hit time "
                                   << hit.time() << " and hit X "
                                   << hit.position()[1]);
  }
  //  if (result) {
  // new particle into the stack
  // secondaries.push_back(newIsp); //don't push back acts particle for now
  //  }

  return 0;  // return nothing for now
}
