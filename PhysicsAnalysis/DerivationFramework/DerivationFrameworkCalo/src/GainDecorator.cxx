/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////
// GainDecorator.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// Author: Simone Mazza (simone.mazza@mi.infn.it),
//         Bruno Lenzi,
//         Giovanni Marchiori (giovanni.marchiori@cern.ch)
// Decorate egamma objects with the energy and number of cells per layer per
// gain

#include "DerivationFrameworkCalo/GainDecorator.h"

#include <regex>
#include <string>
#include <vector>
namespace {}

// Constructor
DerivationFramework::GainDecorator::GainDecorator(const std::string& t,
                                                  const std::string& n,
                                                  const IInterface* p)
  : AthAlgTool(t, n, p)
{
  declareInterface<DerivationFramework::IAugmentationTool>(this);
  declareProperty("decoration_pattern",
                  m_decorationPattern = "{info}_Lr{layer}_{gain}G");
  declareProperty("gain_names",
                  m_gainNames = { { CaloGain::LARHIGHGAIN, "Hi" },
                                  { CaloGain::LARMEDIUMGAIN, "Med" },
                                  { CaloGain::LARLOWGAIN, "Low" } });
  declareProperty("layers", m_layers = { 0, 1, 2, 3 });

  // Define the names for the decorations
  for (const auto& kv : m_gainNames)
    for (const auto layer : m_layers) {
      std::string name = m_decorationPattern;
      name.replace(name.find("{layer}"),
                   std::string("{layer}").size(),
                   std::to_string(layer));
      name.replace(
        name.find("{gain}"), std::string("{gain}").size(), kv.second);
      std::string name_E(name), name_nCells(name);
      name_E.replace(name_E.find("{info}"), std::string("{info}").size(), "E");
      name_nCells.replace(
        name_nCells.find("{info}"), std::string("{info}").size(), "nCells");

      std::pair<int, int> key(kv.first, layer);
      m_names_E[key] = name_E;
      m_names_nCells[key] = name_nCells;
    }

  for (const auto& kv : m_names_E) {
    ATH_MSG_DEBUG("Decorating (layer, gain): " << kv.first << " " << kv.second);
  }
  for (const auto& kv : m_names_nCells) {
    ATH_MSG_DEBUG("Decorating (layer, gain): " << kv.first << " " << kv.second);
  }
}

// Destructor
DerivationFramework::GainDecorator::~GainDecorator() {}

// Athena initialize and finalize
StatusCode
DerivationFramework::GainDecorator::initialize()
{
  ATH_MSG_VERBOSE("initialize() ...");

  if (m_SGKey_photons.key().empty() && m_SGKey_electrons.key().empty()) {
    ATH_MSG_FATAL("No e-gamma collection provided for thinning. At least one "
                  "egamma collection (photons/electrons) must be provided!");
    return StatusCode::FAILURE;
  }

  if (!m_SGKey_electrons.key().empty()) {
    ATH_MSG_DEBUG("Using " << m_SGKey_electrons << " for electrons");
    ATH_CHECK(m_SGKey_electrons.initialize());

    const std::string containerKey = m_SGKey_electrons.key();
    for (const auto& kv : m_gainNames) {
      for (const auto layer : m_layers) {
        std::pair<int, int> key(kv.first, layer);
        m_SGKey_electrons_decorations.emplace_back(containerKey + "." +
                                                   m_names_E[key]);
        m_SGKey_electrons_decorations.emplace_back(containerKey + "." +
                                                   m_names_nCells[key]);
      }
    }
    ATH_CHECK(m_SGKey_electrons_decorations.initialize());
  }

  if (!m_SGKey_photons.key().empty()) {
    ATH_MSG_DEBUG("Using " << m_SGKey_photons << " for photons");
    ATH_CHECK(m_SGKey_photons.initialize());

    const std::string containerKey = m_SGKey_photons.key();
    for (const auto& kv : m_gainNames) {
      for (const auto layer : m_layers) {
        std::pair<int, int> key(kv.first, layer);
        m_SGKey_photons_decorations.emplace_back(containerKey + "." +
                                                 m_names_E[key]);
        m_SGKey_photons_decorations.emplace_back(containerKey + "." +
                                                 m_names_nCells[key]);
      }
    }
    ATH_CHECK(m_SGKey_photons_decorations.initialize());
  }

  return StatusCode::SUCCESS;
}

StatusCode
DerivationFramework::GainDecorator::finalize()
{

  return StatusCode::SUCCESS;
}

// The decoration itself
StatusCode
DerivationFramework::GainDecorator::addBranches() const
{
  const EventContext& ctx = Gaudi::Hive::currentContext();

  // Photon decorations

  if (!m_SGKey_photons.key().empty()) {

    // Retrieve photon container
    SG::ReadHandle<xAOD::EgammaContainer> photonContainer(m_SGKey_photons, ctx);
    const xAOD::EgammaContainer* importedPhotons = photonContainer.ptr();

    // Setup vectors of photon decorations
    std::vector<SG::WriteDecorHandle<xAOD::EgammaContainer, float>>
      decorations_E;
    std::vector<SG::WriteDecorHandle<xAOD::EgammaContainer, char>>
      decorations_nCells;
    int i(0);
    for (const auto& kv : m_gainNames) {
      for (const auto layer : m_layers) {
        std::pair<int, int> key(kv.first, layer);
        decorations_E.emplace_back(
          SG::WriteDecorHandle<xAOD::EgammaContainer, float>(
            m_SGKey_photons_decorations[i * 2], ctx));
        decorations_nCells.emplace_back(
          SG::WriteDecorHandle<xAOD::EgammaContainer, char>(
            m_SGKey_photons_decorations[i * 2 + 1], ctx));
        i++;
      }
    }

    // Decorate photons
    for (const auto* photon : *importedPhotons) {
      DerivationFramework::GainDecorator::calculation res =
        decorateObject(photon);
      i = 0;
      for (const auto& kv : m_gainNames) {
        for (const auto layer : m_layers) {
          std::pair<int, int> key(kv.first, layer);
          decorations_E[i](*photon) = res.E[key];
          decorations_nCells[i](*photon) = res.nCells[key];
          i++;
        }
      }
    }
  }

  // Electron decorations

  if (!m_SGKey_electrons.key().empty()) {

    // Retrieve electron container
    SG::ReadHandle<xAOD::EgammaContainer> electronContainer(m_SGKey_electrons,
                                                            ctx);
    const xAOD::EgammaContainer* importedElectrons = electronContainer.ptr();

    // Setup vectors of electron decorations
    std::vector<SG::WriteDecorHandle<xAOD::EgammaContainer, float>>
      decorations_E;
    std::vector<SG::WriteDecorHandle<xAOD::EgammaContainer, char>>
      decorations_nCells;
    int i(0);
    for (const auto& kv : m_gainNames) {
      for (const auto layer : m_layers) {
        std::pair<int, int> key(kv.first, layer);
        decorations_E.emplace_back(
          SG::WriteDecorHandle<xAOD::EgammaContainer, float>(
            m_SGKey_electrons_decorations[i * 2], ctx));
        decorations_nCells.emplace_back(
          SG::WriteDecorHandle<xAOD::EgammaContainer, char>(
            m_SGKey_electrons_decorations[i * 2 + 1], ctx));
        i++;
      }
    }

    // Decorate electrons
    for (const auto* electron : *importedElectrons) {
      DerivationFramework::GainDecorator::calculation res =
        decorateObject(electron);
      i = 0;
      for (const auto& kv : m_gainNames) {
        for (const auto layer : m_layers) {
          std::pair<int, int> key(kv.first, layer);
          decorations_E[i](*electron) = res.E[key];
          decorations_nCells[i](*electron) = res.nCells[key];
          i++;
        }
      }
    }
  }

  return StatusCode::SUCCESS;
}

DerivationFramework::GainDecorator::calculation
DerivationFramework::GainDecorator::decorateObject(
  const xAOD::Egamma*& egamma) const
{

  // Compute energy and number of cells per gain per layer
  // Set the initial values to 0 (needed?)
  DerivationFramework::GainDecorator::calculation result;
  for (const auto& kv : m_names_E)
    result.E[kv.first] = 0.;
  for (const auto& kv : m_names_nCells)
    result.nCells[kv.first] = 0;

  // Skip the computation for missing cell links (like topo-seeded photons)
  // but decorate anyway
  const CaloClusterCellLink* cellLinks =
    egamma->caloCluster() ? egamma->caloCluster()->getCellLinks() : nullptr;
  if (cellLinks) {
    for (const CaloCell* cell : *cellLinks) {
      if (!cell)
        continue;
      std::pair<int, int> key(static_cast<int>(cell->gain()), getLayer(cell));
      // Increment the corresponding entry (not important if it is not
      // initialised)
      result.E[key] += cell->energy();
      result.nCells[key]++;
    }
  }

  return result;
}

int
DerivationFramework::GainDecorator::getLayer(const CaloCell* cell)
{
  int sampling =
    (cell && cell->caloDDE() ? cell->caloDDE()->getSampling() : -1);
  if (sampling == CaloSampling::PreSamplerB ||
      sampling == CaloSampling::PreSamplerE)
    return 0;
  if (sampling == CaloSampling::EMB1 || sampling == CaloSampling::EME1)
    return 1;
  if (sampling == CaloSampling::EMB2 || sampling == CaloSampling::EME2)
    return 2;
  if (sampling == CaloSampling::EMB3 || sampling == CaloSampling::EME3)
    return 3;
  return -1;
}
