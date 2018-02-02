// This file is part of the ACTS project.
//
// Copyright (C) 2017 ACTS project team
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "ACTFW/Extrapolation/ExtrapolationAlgorithm.hpp"

#include <iostream>
#include <random>
#include <stdexcept>

#include "ACTFW/Framework/WhiteBoard.hpp"
#include "ACTFW/Random/RandomNumberDistributions.hpp"
#include "ACTFW/Random/RandomNumbersSvc.hpp"
#include "ACTFW/Writers/IWriterT.hpp"
#include "ACTS/Detector/TrackingGeometry.hpp"
#include "ACTS/EventData/ParticleDefinitions.hpp"
#include "ACTS/Extrapolation/IExtrapolationEngine.hpp"
#include "ACTS/Layers/Layer.hpp"
#include "ACTS/Surfaces/PerigeeSurface.hpp"
#include "ACTS/Utilities/Definitions.hpp"
#include "ACTS/Utilities/Units.hpp"

FW::ExtrapolationAlgorithm::ExtrapolationAlgorithm(
    const FW::ExtrapolationAlgorithm::Config& cfg,
    Acts::Logging::Level                      loglevel)
  : FW::BareAlgorithm("ExtrapolationAlgorithm", loglevel), m_cfg(cfg)
{
  if (!m_cfg.extrapolationEngine) {
    throw std::invalid_argument("Missing extrapolation engine");
  } else if (m_cfg.evgenCollection.empty()) {
    throw std::invalid_argument("Missing input collection");
  } else if (m_cfg.simulatedParticlesCollection.empty()) {
    throw std::invalid_argument("Missing simulated particles collection");
  }
}

FW::ProcessCode
FW::ExtrapolationAlgorithm::execute(FW::AlgorithmContext ctx) const
{

  ACTS_DEBUG("::execute() called for event " << ctx.eventNumber);
  // read particles from input collection
  const std::vector<Acts::ProcessVertex>* evgen = nullptr;
  if (ctx.eventStore.get(m_cfg.evgenCollection, evgen)
      == FW::ProcessCode::ABORT)
    return FW::ProcessCode::ABORT;

  ACTS_DEBUG("read collection '" << m_cfg.evgenCollection << "' with "
                                 << evgen->size()
                                 << " vertices");

  // output: simulated particles attached to their process vertices
  std::vector<Acts::ProcessVertex> simulated;
  // output: the extrapolation cell collections
  std::vector<Acts::ExtrapolationCell<Acts::TrackParameters>>   cCells;
  std::vector<Acts::ExtrapolationCell<Acts::NeutralParameters>> nCells;
  // output: hits - in detector data container
  using FatrasHit
      = std::pair<std::unique_ptr<const Acts::TrackParameters>, barcode_type>;
  FW::DetectorData<geo_id_value, FatrasHit> hits;

  // loop over the vertices
  size_t evertices = 0;
  for (auto& evtx : (*evgen)) {
    ACTS_DEBUG("Processing event vertex no. " << evertices++);
    // vertex is outside cut
    if (evtx.position().perp() > m_cfg.maxD0) {
      ACTS_VERBOSE("Process vertex is outside the transverse cut. Skipping.");
      continue;
    }
    // the simulated particles associated to this vertex
    std::vector<Acts::ParticleProperties> sparticles;
    // the generated particles
    auto& gparticles = evtx.outgoingParticles();

    std::copy_if(gparticles.begin(),
                 gparticles.end(),
                 std::back_inserter(sparticles),
                 [=](const auto& particle) {
                   return (particle.charge() != 0. || !m_cfg.skipNeutral)
                       && (std::abs(particle.momentum().eta()) < m_cfg.maxEta)
                       && (m_cfg.minPt < particle.momentum().perp());
                 });
    ACTS_DEBUG("Skipped   particles: " << gparticles.size()
                   - sparticles.size());
    ACTS_DEBUG("Simulated particles: " << sparticles.size());
    // create a new process vertex for the output collection
    Acts::ProcessVertex svertex(evtx.position(),
                                evtx.interactionTime(),
                                evtx.processType(),
                                {},
                                sparticles);
    simulated.push_back(svertex);

    // the asspcoated perigee for this vertex
    Acts::PerigeeSurface surface(evtx.position());

    // loop over particles
    for (const auto& particle : sparticles) {
      double d0    = 0.;
      double z0    = 0.;
      double phi   = particle.momentum().phi();
      double theta = particle.momentum().theta();
      // treat differently for neutral particles
      double qop = particle.charge() != 0
          ? particle.charge() / particle.momentum().mag()
          : 1. / particle.momentum().mag();
      // parameters
      Acts::ActsVectorD<5> pars;
      pars << d0, z0, phi, theta, qop;
      // some screen output
      std::unique_ptr<Acts::ActsSymMatrixD<5>> cov = nullptr;
      // execute the test for charged particles
      if (particle.charge()) {
        // charged extrapolation - with hit recording
        Acts::BoundParameters startParameters(
            std::move(cov), std::move(pars), surface);
        if (executeTestT<Acts::TrackParameters>(
                startParameters, particle.barcode(), cCells, &hits)
            != FW::ProcessCode::SUCCESS)
          ACTS_VERBOSE(
              "Test of charged parameter extrapolation did not succeed.");
      } else {
        // neutral extrapolation
        Acts::NeutralBoundParameters neutralParameters(
            std::move(cov), std::move(pars), surface);
        // prepare hits for charged neutral paramters - no hit recording
        if (executeTestT<Acts::NeutralParameters>(
                neutralParameters, particle.barcode(), nCells)
            != FW::ProcessCode::SUCCESS)
          ACTS_WARNING(
              "Test of neutral parameter extrapolation did not succeed.");
      }
    }
  }
  // write simulated data to the event store
  // - the particles
  if (ctx.eventStore.add(m_cfg.simulatedParticlesCollection,
                         std::move(simulated))
      == FW::ProcessCode::ABORT) {
    return FW::ProcessCode::ABORT;
  }
  // - the extrapolation cells - charged - if configured
  if (m_cfg.simulatedChargedExCellCollection != ""
      && ctx.eventStore.add(m_cfg.simulatedChargedExCellCollection,
                            std::move(cCells))
          == FW::ProcessCode::ABORT) {
    return FW::ProcessCode::ABORT;
  }
  // - the extrapolation cells - neutral - if configured
  if (m_cfg.simulatedNeutralExCellCollection != ""
      && ctx.eventStore.add(m_cfg.simulatedNeutralExCellCollection,
                            std::move(nCells))
          == FW::ProcessCode::ABORT) {
    return FW::ProcessCode::ABORT;
  }
  // - the simulated hits - if configured
  if (m_cfg.simulatedHitsCollection != ""
      && ctx.eventStore.add(m_cfg.simulatedHitsCollection, std::move(hits))
          == FW::ProcessCode::ABORT) {
    return FW::ProcessCode::ABORT;
  }
  return FW::ProcessCode::SUCCESS;
}
