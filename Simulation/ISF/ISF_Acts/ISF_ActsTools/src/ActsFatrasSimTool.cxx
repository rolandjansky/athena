/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/
#include <algorithm>
#include <random>

#include "ActsFatrasSimTool.h"

#include "CLHEP/Random/RandFlat.h"
#include "CLHEP/Random/RandomEngine.h"

using namespace Acts::UnitLiterals;

ISF::ActsFatrasSimTool::ActsFatrasSimTool(const std::string& type,
                                          const std::string& name,
                                          const IInterface* parent)
    : BaseSimulatorTool(type, name, parent) {}

ISF::ActsFatrasSimTool::~ActsFatrasSimTool() {}

StatusCode ISF::ActsFatrasSimTool::initialize() {
  ATH_CHECK(BaseSimulatorTool::initialize());

  // Retrieve particle filter
  if (!m_particleFilter.empty()) ATH_CHECK(m_particleFilter.retrieve());

  // setup logger
  m_logger = makeActsAthenaLogger(this->msgSvc().get(), "ActsFatrasSimTool", this->msgSvc().get()->outputLevel(), boost::optional<std::string>("ActsFatrasSimTool"));

  // retrive tracking geo tool
  ATH_CHECK(m_trackingGeometryTool.retrieve());
  m_trackingGeometry = m_trackingGeometryTool->trackingGeometry();
  
  //retrive Magnetfield tool
  ATH_MSG_VERBOSE("Using ATLAS magnetic field service");
  ATH_CHECK( m_fieldCacheCondObjInputKey.initialize());

  // Random number service
  if (m_rngSvc.retrieve().isFailure()) {
    ATH_MSG_FATAL("Could not retrieve " << m_rngSvc);
    return StatusCode::FAILURE;
  }
  // Get own engine with own seeds
  m_randomEngine = m_rngSvc->getEngine(this, m_randomEngineName.value());
  if (!m_randomEngine) {
    ATH_MSG_FATAL("Could not get random engine '" << m_randomEngineName.value() << "'");
    return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;
}

StatusCode ISF::ActsFatrasSimTool::simulate(const ISFParticle& isp, ISFParticleContainer& secondaries,
                                            McEventCollection* mcEventCollection) const {
  ATH_MSG_VERBOSE("Particle " << isp << " received for simulation.");
  // Check if particle passes filter, if there is one
  if (!m_particleFilter.empty() && !m_particleFilter->passFilter(isp)) {
    ATH_MSG_VERBOSE("ISFParticle " << isp << " does not pass selection. Ignoring.");
    return StatusCode::SUCCESS;
  }
  // Process ParticleState from particle stack
  // Wrap the input ISFParticle in an STL vector with size of 1
  const ISF::ConstISFParticleVector ispVector(1, &isp);
  ATH_CHECK(this->simulateVector(ispVector, secondaries, mcEventCollection));
  ATH_MSG_VERBOSE("Simulation done");
  return StatusCode::SUCCESS;
}

StatusCode ISF::ActsFatrasSimTool::simulateVector(
    const ConstISFParticleVector& particles,
    ISFParticleContainer& secondaries,
    McEventCollection* /*mcEventCollection*/) const {

  const EventContext& ctx = Gaudi::Hive::currentContext();
  m_randomEngine->setSeed(m_randomEngineName, ctx);
  CLHEP::HepRandomEngine* randomEngine = m_randomEngine->getEngine(ctx);
  Generator generator(CLHEP::RandFlat::shoot(randomEngine->flat()));
  ATH_MSG_VERBOSE(name() << " RNG seed " << CLHEP::RandFlat::shoot(randomEngine->flat()));
  ATH_MSG_VERBOSE(name() << " received vector of size "
               << particles.size() << " particles for simulation.");

  // construct the ACTS simulator
  Acts::Navigator navigator( Acts::Navigator::Config{ m_trackingGeometry } );
  auto bField = std::make_shared<ATLASMagneticFieldWrapper>();
  auto chargedStepper = ChargedStepper(std::move(bField));
  auto neutralStepper = NeutralStepper();
  auto chargedPropagator = ChargedPropagator(chargedStepper, navigator);
  auto neutralPropagator = NeutralPropagator(neutralStepper, navigator);
  ChargedSimulation simulatorCharged(std::move(chargedPropagator), m_logger);
  NeutralSimulation simulatorNeutral(std::move(neutralPropagator), m_logger);
  Simulation simulator=Simulation(std::move(simulatorCharged),std::move(simulatorNeutral));
  ATH_MSG_VERBOSE(name() << " Min pT for interaction " << m_interact_minPt * Acts::UnitConstants::MeV << " GeV");
  simulator.charged.interactions = ActsFatras::makeStandardChargedElectroMagneticInteractions(m_interact_minPt * Acts::UnitConstants::MeV);
  // get Geo and Mag map
  ATH_MSG_VERBOSE(name() << " Getting per event Geo and Mag map");
  Acts::MagneticFieldContext mctx = getMagneticFieldContext(ctx);
  const ActsGeometryContext& gctx = m_trackingGeometryTool->getNominalGeometryContext();
  auto anygctx = gctx.context();
  // Loop over ISFParticleVector and process each separately
  ATH_MSG_VERBOSE(name() << " Processing particles in ISFParticleVector.");
  for (const auto isfp : particles) {
    // ====ACTSFatras Simulation====
    // //  
    // input/output particle and hits containers
    // Convert to ActsFatras::Particle
    // ISF: Energy, mass, and momentum are in MeV
    // Acts: Energy, mass, and momentum are in GeV
    std::vector<ActsFatras::Particle> input = std::vector<ActsFatras::Particle>{
      ActsFatras::Particle(ActsFatras::Barcode().setVertexPrimary(0).setParticle(
        isfp->barcode()),static_cast<Acts::PdgParticle>(isfp->pdgCode()), 
        isfp->charge(),isfp->mass() * Acts::UnitConstants::MeV)
      .setDirection(Acts::makeDirectionUnitFromPhiEta(
                    isfp->momentum().phi(), isfp->momentum().eta()))
      .setAbsoluteMomentum(isfp->momentum().mag() * Acts::UnitConstants::MeV)
      .setPosition4(isfp->position().x(), isfp->position().y(),
                    isfp->position().z(), isfp->timeStamp())};
    std::vector<ActsFatras::Particle> simulatedInitial;
    std::vector<ActsFatras::Particle> simulatedFinal;
    std::vector<ActsFatras::Hit> hits;
    ATH_MSG_DEBUG(name() << " Propagating ActsFatras::Particle  vertex|particle|generation|subparticle, " << input[0]);
    // simulate
    simulator.simulate(anygctx, mctx, generator, input, simulatedInitial, simulatedFinal, hits);
    ATH_MSG_DEBUG(name() << " initial particle " << simulatedInitial[0]);
    ATH_MSG_DEBUG(name() << " No. of particles after ActsFatras simulator: " << simulatedFinal.size());
    // convert final particles to ISF::particle
    for (const auto& factsp : simulatedFinal){
      const auto pos = Amg::Vector3D(factsp.position()[Acts::ePos0],factsp.position()[Acts::ePos1],factsp.position()[Acts::ePos2]);
      const auto mom = Amg::Vector3D(factsp.fourMomentum()[Acts::eMom0] / Acts::UnitConstants::MeV,factsp.fourMomentum()[Acts::eMom1] / Acts::UnitConstants::MeV,factsp.fourMomentum()[Acts::eMom2] / Acts::UnitConstants::MeV);
      double mass = factsp.mass() / Acts::UnitConstants::MeV;
      double charge = factsp.charge();
      int pdgid = factsp.pdg();
      double properTime = factsp.properTime();

      if (factsp.particleId() == simulatedInitial[0].particleId()) {
        ATH_MSG_DEBUG(name() << " final particle " << factsp);
      }
      else {
        ATH_MSG_DEBUG(name() << " secondaries particle " << factsp);
        auto secisfp = std::make_unique<ISF::ISFParticle>(pos,mom,mass,charge,pdgid,properTime,*isfp);
        secondaries.push_back(secisfp.release());

      }
    }
    ATH_MSG_DEBUG(name() << " No. of secondaries: " << secondaries.size());
    ATH_MSG_DEBUG(name() << " ActsFatras simulator hits: " << hits.size());
    int i = 0;
    for (const auto& hit : hits) {
      ATH_MSG_DEBUG(name() << " hit pos: " << hit.position() );
      ++i;
      if (i>5) break;
    }
  std::vector<ActsFatras::Particle>().swap(input);
  std::vector<ActsFatras::Particle>().swap(simulatedInitial);
  std::vector<ActsFatras::Particle>().swap(simulatedFinal);
  std::vector<ActsFatras::Hit>().swap(hits);
  }
  return StatusCode::SUCCESS;
}

Acts::MagneticFieldContext ISF::ActsFatrasSimTool::getMagneticFieldContext(const EventContext& ctx) const {
  SG::ReadCondHandle<AtlasFieldCacheCondObj> readHandle{m_fieldCacheCondObjInputKey, ctx};
  if (!readHandle.isValid()) {
     throw std::runtime_error(name() + ": Failed to retrieve magnetic field condition data " + m_fieldCacheCondObjInputKey.key() + ".");
  }
  else ATH_MSG_DEBUG(name() << "retrieved magnetic field condition data "<< m_fieldCacheCondObjInputKey.key());
  const AtlasFieldCacheCondObj* fieldCondObj{*readHandle};

  return Acts::MagneticFieldContext(fieldCondObj);
}
