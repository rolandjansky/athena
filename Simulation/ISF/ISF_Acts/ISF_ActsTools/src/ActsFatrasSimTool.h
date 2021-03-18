/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// ActsFatrasSimTool.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef ISF_ACTSTOOLS_ACTSFATRASSIMTOOL_H
#define ISF_ACTSTOOLS_ACTSFATRASSIMTOOL_H

// Gaudi
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"  // for ToolHandleArray

// ISF
#include "ISF_Interfaces/BaseSimulatorTool.h"
#include "ISF_Interfaces/IParticleFilter.h"
#include "ISF_Interfaces/IParticleProcessor.h"

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
//#include "Acts/Tests/CommonHelpers/CylindricalTrackingGeometry.hpp"
#include "Acts/Utilities/UnitVectors.hpp"
#include "ActsFatras/Kernel/PhysicsList.hpp"
#include "ActsFatras/Kernel/Simulator.hpp"
#include "ActsFatras/Physics/StandardPhysicsLists.hpp"
#include "ActsFatras/Selectors/ChargeSelectors.hpp"
#include "ActsFatras/Selectors/SurfaceSelectors.hpp"
#include "ActsFatras/Utilities/ParticleData.hpp"
#include "ActsInterop/Logger.h"
//#include "Acts/Utilities/Logger.hpp"

class AtlasDetectorID;
using namespace Acts::UnitLiterals;
namespace iFatras {
class ISimHitCreator;
}

namespace ISF {  // use fatras namespace for now, to be replaced to ACTS
                 // namespace TODO

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

   Standard ATLAS hit creator, with ACTS-fatras simulation

   @author yi.fei.han at cern.ch
   @author vrpascuzzi at lbl.gov
*/

class ActsFatrasSimTool : public extends<AthAlgTool, ISF::IParticleProcessor> {
 public:
  using Navigator = Acts::Navigator;
  using MagneticField = Acts::ConstantBField;
  using ChargedStepper = Acts::EigenStepper<MagneticField>;
  using ChargedPropagator = Acts::Propagator<ChargedStepper, Navigator>;
  using NeutralStepper = Acts::StraightLineStepper;
  using NeutralPropagator = Acts::Propagator<NeutralStepper, Navigator>;
  using Generator = std::ranlux48;
  using MinP = ActsFatras::Min<ActsFatras::Casts::P>;
  using ChargedSelector =
      ActsFatras::CombineAnd<ActsFatras::ChargedSelector, MinP>;
  using ChargedPhysicsList =
      ActsFatras::PhysicsList<ActsFatras::detail::StandardScattering,
                              SplitEnergyLoss>;
  using ChargedSimulator = ActsFatras::ParticleSimulator<
      ChargedPropagator, ActsFatras::ChargedElectroMagneticPhysicsList,
      ActsFatras::EverySurface>;
  using NeutralSelector =
      ActsFatras::CombineAnd<ActsFatras::NeutralSelector, MinP>;
  using NeutralSimulator = ActsFatras::ParticleSimulator<
      NeutralPropagator, ActsFatras::PhysicsList<>, ActsFatras::NoSurface>;
  using Simulator = ActsFatras::Simulator<ChargedSelector, ChargedSimulator,
                                          NeutralSelector, NeutralSimulator>;

  /**Constructor */
  ActsFatrasSimTool(const std::string&, const std::string&, const IInterface*);
  /**Destructor*/
  virtual ~ActsFatrasSimTool();

  /** AlgTool initailize method.*/
  virtual StatusCode initialize() override;
  /** AlgTool finalize method */
  virtual StatusCode finalize() override;

  virtual ISF::ISFParticle* process(const ISFParticle& isp,
                                    CLHEP::HepRandomEngine*) const override;

  virtual StatusCode setupEvent() { return StatusCode::SUCCESS; };  // override

  virtual StatusCode releaseEvent() {
    return StatusCode::SUCCESS;
  };  // override

  virtual ISF::SimulationFlavor simFlavor() const {
    return ISF::Fatras;
  };  // override

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

  std::string m_idHelperName;
  ToolHandle<IActsTrackingGeometryTool> m_trackingGeometryTool{
      this, "ActsTrackingGeometryTool", "ActsTrackingGeometryTool"};
  ToolHandle<IActsExtrapolationTool> m_actsExtrapolationTool{
      this, "ActsExtrapolationTool", "ActsExtrapolationTool"};

  ActsGeometryContext m_geoCtx;
  Acts::MagneticFieldContext m_magCtx;

  std::unique_ptr<const Acts::Logger> m_logger{nullptr};
  // construct the example detector
  std::shared_ptr<const Acts::TrackingGeometry> m_trackingGeometry;

  // construct the propagators
  Navigator m_navigator{std::move(m_trackingGeometry)};
  ChargedStepper m_chargedStepper{Acts::ConstantBField(0, 0, 1_T)};
  ChargedPropagator m_chargedPropagator{std::move(m_chargedStepper),
                                        m_navigator};
  NeutralPropagator m_neutralPropagator{NeutralStepper(), m_navigator};

  // construct the simulator
  ChargedSimulator m_chargedSimulator{std::move(m_chargedPropagator),
                                      Acts::Logging::Level::VERBOSE};
  NeutralSimulator m_neutralSimulator{std::move(m_neutralPropagator),
                                      Acts::Logging::Level::VERBOSE};
  Simulator m_simulator{std::move(m_chargedSimulator),
                        std::move(m_neutralSimulator)};

  // Sim Hit Creators
  ToolHandle<iFatras::ISimHitCreator> m_simHitCreatorID;
  //    ToolHandle<ISimHitCreator>          m_simHitCreatorMS;
  Gaudi::Property<double> m_ptLoopers{
      this, "PtLoopers", 300,
      "pT loop protection threshold, converted to Acts MeV unit"};
  Gaudi::Property<double> m_maxStepSize{this, "MaxStepSize", 10,
                                        "Max step size in Acts m unit"};
  Gaudi::Property<double> m_maxStep{this, "MaxSteps", 4000,
                                    "Max number of steps"};

  // Material inteaction option
  Gaudi::Property<bool> m_interactionMultiScatering{
      this, "InteractionMultiScatering", false,
      "Whether to consider multiple scattering in the interactor"};
  Gaudi::Property<bool> m_interactionEloss{
      this, "InteractionEloss", false,
      "Whether to consider energy loss in the interactor"};
  Gaudi::Property<bool> m_interactionRecord{
      this, "InteractionRecord", false,
      "Whether to record all material interactions"};
};
}  // namespace ISF

#endif
