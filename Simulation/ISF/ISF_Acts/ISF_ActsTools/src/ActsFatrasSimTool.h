/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ISF_ACTSTOOLS_ACTSFATRASSIMTOOL_H
#define ISF_ACTSTOOLS_ACTSFATRASSIMTOOL_H

// Gaudi
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"

// ISF
#include "ISF_Event/ISFParticle.h"
#include "ISF_FatrasInterfaces/ISimHitCreator.h"
#include "ISF_Interfaces/BaseSimulatorTool.h"
#include "ISF_Interfaces/IParticleFilter.h"

// ACTS
#include "Acts/EventData/NeutralTrackParameters.hpp"
#include "Acts/EventData/TrackParameters.hpp"
#include "Acts/MagneticField/ConstantBField.hpp"
#include "Acts/Propagator/EigenStepper.hpp"
#include "Acts/Propagator/Navigator.hpp"
#include "Acts/Propagator/StraightLineStepper.hpp"
#include "ActsGeometry/ActsExtrapolationTool.h"
#include "ActsGeometry/ActsGeometryContext.h"
#include "ActsGeometry/ActsTrackingGeometryTool.h"
#include "ActsGeometryInterfaces/IActsExtrapolationTool.h"
#include "ActsGeometryInterfaces/IActsTrackingGeometryTool.h"
#include "Acts/Utilities/UnitVectors.hpp"
#include "ActsFatras/Kernel/InteractionList.hpp"
#include "ActsFatras/Kernel/Simulation.hpp"
#include "ActsFatras/Physics/Decay/NoDecay.hpp"
#include "ActsFatras/Physics/StandardInteractions.hpp"
#include "ActsFatras/Selectors/ParticleSelectors.hpp"
#include "ActsFatras/Selectors/SurfaceSelectors.hpp"
#include "ActsFatras/Utilities/ParticleData.hpp"
#include "ActsInterop/Logger.h"

class AtlasDetectorID;
using namespace Acts::UnitLiterals;
namespace iFatras {
class ISimHitCreator;
}

namespace ISF {

struct SplitEnergyLoss {
  double splitMomentumMin = 5_GeV;

  template <typename generator_t>
  bool operator()(generator_t&, const Acts::MaterialSlab&,
                  ActsFatras::Particle& particle,
                  std::vector<ActsFatras::Particle>& generated) const {
    const auto p = particle.absoluteMomentum();
    if (splitMomentumMin < p) {
      particle.setAbsoluteMomentum(0.5 * p);
      const auto pid = particle.particleId().makeDescendant();
      generated.push_back(particle.withParticleId(pid).setAbsoluteMomentum(0.5 * p));
    }
    // never break
    return false;
  }
};

/**
   @class ACTSFATRASSIMTOOL

   Standard ATLAS hit creator, with Acts-fatras simulation

   @author yi.fei.han at cern.ch
   @author vrpascuzzi at lbl.gov
*/

class ActsFatrasSimTool : public BaseSimulatorTool {
 public:
  using Navigator = Acts::Navigator;
  using MagneticField = Acts::ConstantBField;
  using ChargedStepper = Acts::EigenStepper<>;
  using ChargedPropagator = Acts::Propagator<ChargedStepper, Navigator>;
  using NeutralStepper = Acts::StraightLineStepper;
  using NeutralPropagator = Acts::Propagator<NeutralStepper, Navigator>;
  using Generator = std::ranlux48;
  using MinP = ActsFatras::Min<ActsFatras::Casts::P>;
  using ChargedSelector =
      ActsFatras::CombineAnd<ActsFatras::ChargedSelector, MinP>;
  using ChargedPhysicsList =
      ActsFatras::InteractionList<ActsFatras::detail::StandardScattering,
                              SplitEnergyLoss>;
  using ChargedSimulator = ActsFatras::SingleParticleSimulation<
      ChargedPropagator, ActsFatras::StandardChargedElectroMagneticInteractions,
      ActsFatras::EverySurface, ActsFatras::NoDecay>;
  using NeutralSelector =
      ActsFatras::CombineAnd<ActsFatras::NeutralSelector, MinP>;
  using NeutralSimulator = ActsFatras::SingleParticleSimulation<
      NeutralPropagator, ActsFatras::InteractionList<>, ActsFatras::NoSurface,
      ActsFatras::NoDecay>;
  using Simulator = ActsFatras::Simulation<ChargedSelector, ChargedSimulator,
                                          NeutralSelector, NeutralSimulator>;

  ActsFatrasSimTool(const std::string& type, const std::string& name,
                    const IInterface* parent);
  virtual ~ActsFatrasSimTool();

  // Interface methods
  virtual StatusCode initialize() override;
  virtual StatusCode simulate(const ISFParticle& isp, ISFParticleContainer&,
                              McEventCollection*) const override;
  virtual StatusCode setupEvent(const EventContext&) override { return StatusCode::SUCCESS; };
  virtual StatusCode releaseEvent(const EventContext&) override { return StatusCode::SUCCESS; };
  virtual ISF::SimulationFlavor simFlavor() const override { return ISF::Fatras; };

  virtual ISF::ISFParticle* process(const ISFParticle& isp) const;

 private:
  /** templated Tool retrieval - gives unique handling & look and feel */
  template <class T>
  StatusCode retrieveTool(ToolHandle<T>& thandle) {
    if (!thandle.empty() && thandle.retrieve().isFailure()) {
      ATH_MSG_FATAL("[ fatras setup ] Cannot retrieve " << thandle
                                                        << ". Abort.");
      return StatusCode::FAILURE;
    } else
      ATH_MSG_DEBUG("[ fatras setup ] Successfully retrieved " << thandle);
    return StatusCode::SUCCESS;
  }

  // Tracking geometry tools
  ToolHandle<IActsTrackingGeometryTool> m_trackingGeometryTool{
      this, "ActsTrackingGeometryTool", "ActsTrackingGeometryTool"};
  std::shared_ptr<const Acts::TrackingGeometry> m_trackingGeometry;
  // Acts extrapolator
  ToolHandle<IActsExtrapolationTool> m_actsExtrapolationTool{
      this, "ActsExtrapolationTool", "ActsExtrapolationTool"};

  ActsGeometryContext m_geoCtx;
  Acts::MagneticFieldContext m_magCtx;

  std::unique_ptr<const Acts::Logger> m_logger{nullptr};

  // Acts propagators
  Navigator m_navigator{ Acts::Navigator::Config{m_trackingGeometry}};
  std::shared_ptr<Acts::ConstantBField> m_bField = std::make_shared<Acts::ConstantBField>(Acts::Vector3(0, 0, 1_T));
  ChargedStepper m_chargedStepper{m_bField};
  ChargedPropagator m_chargedPropagator{std::move(m_chargedStepper),
                                        m_navigator};
  NeutralPropagator m_neutralPropagator{NeutralStepper(), m_navigator};

  // Acts simulators
  ChargedSimulator m_chargedSimulator{std::move(m_chargedPropagator),
                                      Acts::getDefaultLogger("Simulation", 
                                      Acts::Logging::Level::VERBOSE)};
  NeutralSimulator m_neutralSimulator{std::move(m_neutralPropagator),
                                      Acts::getDefaultLogger("Simulation", 
                                      Acts::Logging::Level::VERBOSE)};
  Simulator m_simulator{std::move(m_chargedSimulator),
                        std::move(m_neutralSimulator)};

  // SimHit creator for Inner Detector
  ToolHandle<iFatras::ISimHitCreator> m_simHitCreatorID;
  
  // Particle filtering
  PublicToolHandle<ISF::IParticleFilter> m_particleFilter{
    this, "ParticleFilter", "", "Particle filter kinematic cuts, etc."};

  // Properties
  Gaudi::Property<bool> m_interactionMultiScatering{
      this, "InteractionMultiScatering", false,
      "Whether to consider multiple scattering in the interactor"};
  Gaudi::Property<bool> m_interactionEloss{
      this, "InteractionEloss", false,
      "Whether to consider energy loss in the interactor"};
  Gaudi::Property<bool> m_interactionRecord{
      this, "InteractionRecord", false,
      "Whether to record all material interactions"};
  Gaudi::Property<double> m_maxStepSize{this, "MaxStepSize", 10,
                                        "Max step size in Acts m unit"};
  Gaudi::Property<double> m_maxStep{this, "MaxSteps", 4000,
                                    "Max number of steps"};
  Gaudi::Property<double> m_ptLoopers{
      this, "PtLoopers", 300,
      "pT loop protection threshold, converted to Acts MeV unit"};
};

} // namespace ISF

#endif
