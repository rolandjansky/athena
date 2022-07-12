/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ISF_ACTSTOOLS_ACTSFATRASSIMTOOL_H
#define ISF_ACTSTOOLS_ACTSFATRASSIMTOOL_H

// Gaudi
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"

// Athena
#include "AthenaKernel/IAthRNGSvc.h"
#include "AthenaKernel/RNGWrapper.h"

// ISF
#include "ISF_Event/ISFParticle.h"
#include "ISF_Interfaces/BaseSimulatorTool.h"
#include "ISF_Interfaces/IParticleFilter.h"

// ACTS
#include "Acts/Utilities/UnitVectors.hpp"
#include "ActsInterop/Logger.h"
#include "Acts/Geometry/GeometryContext.hpp"
#include "Acts/MagneticField/MagneticFieldContext.hpp"
#include "Acts/EventData/SingleCurvilinearTrackParameters.hpp"
#include "Acts/Propagator/Navigator.hpp"
#include "Acts/Propagator/EigenStepper.hpp"
#include "Acts/Propagator/StraightLineStepper.hpp"
#include "Acts/Propagator/detail/SteppingLogger.hpp"
#include "Acts/Propagator/ActionList.hpp"
#include "Acts/Propagator/Propagator.hpp"
#include "ActsFatras/Kernel/InteractionList.hpp"
#include "ActsFatras/Kernel/Simulation.hpp"
#include "ActsFatras/Kernel/SimulationResult.hpp"
#include "ActsFatras/Physics/Decay/NoDecay.hpp"
#include "ActsFatras/Physics/StandardInteractions.hpp"
#include "ActsFatras/Selectors/SurfaceSelectors.hpp"
#include "ActsFatras/Utilities/ParticleData.hpp"
// Tracking
#include "ActsGeometryInterfaces/IActsTrackingGeometryTool.h"
#include "ActsGeometry/ATLASMagneticFieldWrapper.h"

#include <algorithm>
#include <cassert>
#include <vector>

class AtlasDetectorID;
using namespace Acts::UnitLiterals;

namespace iFatras {
  class ISimHitCreator;
}

namespace ISF {
class IParticleHelper;

/**
   @class ACTSFATRASSIMTOOL

   Standard ATLAS hit creator, with Acts-fatras simulation
*/

class ActsFatrasSimTool : public BaseSimulatorTool {
 
 public:
  /// Simple struct to select surfaces where hits should be generated.
  struct HitSurfaceSelector {
    /// Check if the surface should be used.
    bool operator()(const Acts::Surface &surface) const {
      bool isSensitive = surface.associatedDetectorElement();
      return isSensitive;
    }
  };
  // SingleParticleSimulation
  /// Single particle simulation with fixed propagator, interactions, and decay.
  ///
  /// @tparam generator_t random number generator
  /// @tparam interactions_t interaction list
  /// @tparam hit_surface_selector_t selector for hit surfaces
  /// @tparam decay_t decay module
  template <typename propagator_t, typename interactions_t,
            typename hit_surface_selector_t, typename decay_t>
  struct SingleParticleSimulation {
    /// How and within which geometry to propagate the particle.
    propagator_t propagator;
    /// Decay module.
    decay_t decay;
    /// Interaction list containing the simulated interactions.
    interactions_t interactions;
    /// Selector for surfaces that should generate hits.
    hit_surface_selector_t selectHitSurface;
    /// parameters for propagator options
    double maxStepSize = 3.0; // leght in m
    double maxStep = 1000;
    double pathLimit = 100.0; // lenght in cm
    double ptLoopers = 300.0; // pT in MeV

    /// Wrapped logger for debug output.
    Acts::LoggerWrapper loggerWrapper = Acts::getDummyLogger();
    /// Local logger for debug output.
    std::shared_ptr<const Acts::Logger> localLogger = nullptr;

    /// Alternatively construct the simulator with an external logger.
    SingleParticleSimulation(propagator_t &&propagator_,
                            std::shared_ptr<const Acts::Logger> localLogger_)
        : propagator(propagator_), localLogger(localLogger_) {}

    /// Provide access to the local logger instance, e.g. for logging macros.
    const Acts::Logger &logger() const { return *localLogger; }
    
    /// Simulate a single particle without secondaries.
    ///
    /// @tparam generator_t is the type of the random number generator
    ///
    /// @param geoCtx is the geometry context to access surface geometries
    /// @param magCtx is the magnetic field context to access field values
    /// @param generator is the random number generator
    /// @param particle is the initial particle state
    /// @returns Simulated particle state, hits, and generated particles.
    template <typename generator_t>
    Acts::Result<ActsFatras::SimulationResult> simulate(
        const Acts::GeometryContext &geoCtx,
        const Acts::MagneticFieldContext &magCtx, generator_t &generator,
        const ActsFatras::Particle &particle) const {
      assert(localLogger and "Missing local logger");
      ACTS_VERBOSE("Using ActsFatrasSimTool simulate()");
      // propagator-related additional types
      using SteppingLogger = Acts::detail::SteppingLogger;
      using Actor = ActsFatras::detail::SimulationActor<generator_t, decay_t, interactions_t, hit_surface_selector_t>;
      using Aborter = typename Actor::ParticleNotAlive;
      using Result = typename Actor::result_type;
      using Actions = Acts::ActionList<SteppingLogger, Actor>;
      using Abort = Acts::AbortList<Aborter, Acts::EndOfWorldReached>;
      using PropagatorOptions = Acts::DenseStepperPropagatorOptions<Actions, Abort>;

      // Construct per-call options.
      PropagatorOptions options(geoCtx, magCtx, Acts::LoggerWrapper{*localLogger});
      // setup the interactor as part of the propagator options
      auto &actor = options.actionList.template get<Actor>();
      actor.generator = &generator;
      actor.decay = decay;
      actor.interactions = interactions;
      actor.selectHitSurface = selectHitSurface;
      actor.initialParticle = particle;
      // use AnyCharge to be able to handle neutral and charged parameters
      Acts::SingleCurvilinearTrackParameters<Acts::AnyCharge> startPoint(
          particle.fourPosition(), particle.unitDirection(),
          particle.absoluteMomentum(), particle.charge());
      options.absPdgCode = Acts::makeAbsolutePdgParticle(particle.pdg());
      options.mass = particle.mass();
      options.pathLimit = pathLimit * Acts::UnitConstants::cm;
      options.loopProtection = Acts::VectorHelpers::perp(startPoint.momentum()) 
                              < ptLoopers * Acts::UnitConstants::MeV;
      options.maxStepSize = maxStepSize * Acts::UnitConstants::m;
      options.maxSteps = maxStep;

      auto result = propagator.propagate(startPoint, options);
      if (not result.ok()) {
        return result.error();
      }
      auto &value = result.value().template get<Result>();
      return std::move(value);
    }
  };// end of SingleParticleSimulation

  // Standard generator
  using Generator = std::ranlux48;
  // Use default navigator
  using Navigator = Acts::Navigator;
  // Propagate charged particles numerically in the B-field
  using ChargedStepper = Acts::EigenStepper<>;
  using ChargedPropagator = Acts::Propagator<ChargedStepper, Navigator>;
  // Propagate neutral particles in straight lines
  using NeutralStepper = Acts::StraightLineStepper;
  using NeutralPropagator = Acts::Propagator<NeutralStepper, Navigator>;

  // ===============================
  // Setup ActsFatras simulator types
  // Charged
  using ChargedSelector = ActsFatras::EveryParticle;
  using ChargedInteractions =
          ActsFatras::StandardChargedElectroMagneticInteractions;
  using ChargedSimulation = SingleParticleSimulation<
          ChargedPropagator, ChargedInteractions, HitSurfaceSelector,
          ActsFatras::NoDecay>;
  // Neutral
  using NeutralSelector = ActsFatras::EveryParticle;
  using NeutralInteractions = ActsFatras::InteractionList<>;
  using NeutralSimulation = SingleParticleSimulation<
          NeutralPropagator, NeutralInteractions, ActsFatras::NoSurface,
          ActsFatras::NoDecay>;
  // Combined
  using Simulation = ActsFatras::Simulation<ChargedSelector, ChargedSimulation,
                                            NeutralSelector, NeutralSimulation>;
  // ===============================

  ActsFatrasSimTool(const std::string& type, const std::string& name,
                    const IInterface* parent);
  virtual ~ActsFatrasSimTool();

  // ISF BaseSimulatorTool Interface methods
  virtual StatusCode initialize() override;
  virtual StatusCode simulate(const ISFParticle& isp, ISFParticleContainer&,
                              McEventCollection*) const override;
  virtual StatusCode simulateVector(
            const ConstISFParticleVector& particles,
            ISFParticleContainer& secondaries,
            McEventCollection* mcEventCollection) const override;
  virtual StatusCode setupEvent(const EventContext&) override {
    return StatusCode::SUCCESS; };
  virtual StatusCode releaseEvent(const EventContext&) override {
    return StatusCode::SUCCESS; };
  virtual ISF::SimulationFlavor simFlavor() const override{
    return ISF::Fatras; };

  virtual Acts::MagneticFieldContext getMagneticFieldContext(
    const EventContext&) const;

 private:
  // Templated tool retrieval
  template <class T>
  StatusCode retrieveTool(ToolHandle<T>& thandle) {
    if (!thandle.empty() && thandle.retrieve().isFailure()) {
      ATH_MSG_FATAL("Cannot retrieve " << thandle << ". Abort.");
      return StatusCode::FAILURE;
    } else ATH_MSG_DEBUG("Successfully retrieved " << thandle);
    return StatusCode::SUCCESS;
  }

  // Random number service
  ServiceHandle<IAthRNGSvc> m_rngSvc{this, "RNGServec", "AthRNGSvc"};
  ATHRNG::RNGWrapper* m_randomEngine;
  Gaudi::Property<std::string> m_randomEngineName{this, "RandomEngineName",
    "RandomEngineName", "Name of random number stream"};

  // Tracking geometry
  ToolHandle<IActsTrackingGeometryTool> m_trackingGeometryTool{
      this, "TrackingGeometryTool", "ActsTrackingGeometryTool"};
  std::shared_ptr<const Acts::TrackingGeometry> m_trackingGeometry;

  // Magnetic field
  SG::ReadCondHandleKey<AtlasFieldCacheCondObj> m_fieldCacheCondObjInputKey {this, "AtlasFieldCacheCondObj", "fieldCondObj", "Name of the Magnetic Field conditions object key"};

  // Logging
  std::shared_ptr<const Acts::Logger> m_logger{nullptr};

  // ISF Tools
  PublicToolHandle<ISF::IParticleFilter> m_particleFilter{
      this, "ParticleFilter", "", "Particle filter kinematic cuts, etc."};

  Gaudi::Property<double> m_interact_minPt{this, "Interact_MinPt", 50.0,
    "Min pT of the interactions (MeV)"};

};

}  // namespace ISF

#endif
