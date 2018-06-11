// This file is part of the ACTS project.
//
// Copyright (C) 2017 ACTS project team
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include <cmath>
#include <stdexcept>
#include <random>

#include "ActsGeometry/Extrapolation/ParticleGun.hpp"
#include "Acts/Utilities/Units.hpp"

#include "CLHEP/Random/RandomEngine.h"

ParticleGun::ParticleGun(const ParticleGun::Config& cfg,
    std::unique_ptr<const Acts::Logger> _logger)
  : m_cfg(cfg), m_logger(std::move(_logger))
{
  // Print chosen configuration
  ACTS_DEBUG("Particle gun settings: ");
  ACTS_VERBOSE("- d0  range: " << m_cfg.d0Range[0] << ", " << m_cfg.d0Range[1]);
  ACTS_VERBOSE("- z0  range: " << m_cfg.z0Range[0] << ", " << m_cfg.z0Range[1]);
  ACTS_VERBOSE("- phi range: " << m_cfg.phiRange[0] << ", "
                               << m_cfg.phiRange[1]);
  ACTS_VERBOSE("- eta range: " << m_cfg.etaRange[0] << ", "
                               << m_cfg.etaRange[1]);
  ACTS_VERBOSE("- pt  range: " << m_cfg.ptRange[0] << ", " << m_cfg.ptRange[1]);
}

std::vector<Acts::ProcessVertex>
ParticleGun::generate(CLHEP::HepRandomEngine* rnd) const
{

  ACTS_DEBUG("::generate() called");
  // what's written out
  std::vector<Acts::ProcessVertex> vertices;
  std::uniform_real_distribution<> dis(1.0, 2.0);

  // the particles
  std::vector<Acts::ParticleProperties> particles;
  for (size_t ip = 0; ip < m_cfg.nParticles; ip++) {
    // generate random parameters
    double d0 = random(rnd, m_cfg.d0Range.at(0), m_cfg.d0Range.at(1));
    double z0 = random(rnd, m_cfg.z0Range.at(0), m_cfg.z0Range.at(1));
    double phi = random(rnd, m_cfg.phiRange.at(0), m_cfg.phiRange.at(1));
    double eta = random(rnd, m_cfg.etaRange.at(0), m_cfg.etaRange.at(1));
    double pt = random(rnd, m_cfg.ptRange.at(0), m_cfg.ptRange.at(1));

    barcode_type bc = 42 * ip;
    
    Acts::Vector3D vertex(d0 * std::sin(phi), d0 * -std::cos(phi), z0);

    // create momentum from random parameters
    Acts::Vector3D momentum(
        pt * std::cos(phi), pt * std::sin(phi), pt * std::sinh(eta));

    // flip charge and PID if asked for
    int flip = (!m_cfg.randomCharge || rnd->flat() < 0.5) ? 1 : -1;

    // the particle should be ready now
    particles.emplace_back(
        momentum, m_cfg.mass, flip * m_cfg.charge, flip * m_cfg.pID, bc);
  }
  ACTS_DEBUG("Generated 1 vertex with " << particles.size() << " particles.");
  // the vertices
  vertices.push_back(Acts::ProcessVertex(
      Acts::Vector3D(0., 0., 0.), 0., 0, {}, std::move(particles)));


  return vertices;
}

double 
ParticleGun::random(CLHEP::HepRandomEngine* rnd, double min, double max) const {
  return rnd->flat() * std::abs(max - min) + min;
}
