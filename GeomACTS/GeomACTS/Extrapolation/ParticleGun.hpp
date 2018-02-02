// This file is part of the ACTS project.
//
// Copyright (C) 2017 ACTS project team
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#ifndef ACTFW_ALGORITHMS_FATRAS_PARTICLEGUN_H
#define ACTFW_ALGORITHMS_FATRAS_PARTICLEGUN_H

#include <random>

#include <ACTS/Utilities/Units.hpp>
#include <array>
#include "ACTS/EventData/ParticleDefinitions.hpp"
#include "ACTS/Utilities/Logger.hpp"

using range = std::array<double, 2>;


/// @class ParticleGun
///
/// Particle gun implementation of the IParticleReader. It can be used as
/// particle input for the Fatras example.
/// It generates particles of given type with random momentum and random vertex
/// in a given range. It fills a vector of particle properties for feeding into
/// fast simulation.
///
class ParticleGun
{
public:
  struct Config
  {
    /// number of particles
    size_t nParticles = 0;
    /// low, high for d0 range
    range d0Range = {{0., 1 * Acts::units::_mm}};
    /// low, high for z0 range
    range z0Range = {{-100 * Acts::units::_mm, 100 * Acts::units::_mm}};
    /// low, high for phi range
    range phiRange = {{-M_PI, M_PI}};
    /// low, high for eta range
    range etaRange = {{-3., 3.}};
    /// low, high for pt range
    range ptRange = {{100 * Acts::units::_MeV, 10 * Acts::units::_GeV}};
    /// the mass of the particle
    double mass = 0.;
    /// the charge of the particle
    double charge = 0.;
    /// the pdg type of the particle
    pdg_type pID = 0.;
    // randomize the charge (indicates PID flip)
    bool randomCharge = false;
    std::shared_ptr<std::mt19937> rng;
    // FW random number service
    //std::shared_ptr<FW::RandomNumbersSvc> randomNumbers = nullptr;
    //std::shared_ptr<FW::BarcodeSvc>       barcodes      = nullptr;
  };

  /// Constructor
  /// @param cfg is the configuration class
  ParticleGun(const Config&        cfg,
    std::unique_ptr<const Acts::Logger> logger  = Acts::getDefaultLogger("ParticleGun", Acts::Logging::INFO));

  
  std::vector<Acts::ProcessVertex>
  generate() const;

private:
  Config m_cfg;
  
  std::unique_ptr<const Acts::Logger> m_logger;

  const Acts::Logger&
  logger() const
  {
    return *(m_logger.get());
  }
};


#endif  // ACTFW_ALGORITHMS_FATRAS_PARTICLEGUN_H

