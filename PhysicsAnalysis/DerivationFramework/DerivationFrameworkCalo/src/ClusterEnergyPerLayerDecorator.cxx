/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////
// ClusterEnergyPerLayerDecorator.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
// Author: Bruno Lenzi,
//         Giovanni Marchiori (giovanni.marchiori@cern.ch)
// Decorate egamma objects with the energy per layer for a rectangular cluster
// of size neta X nphi built on the fly

#include "DerivationFrameworkCalo/ClusterEnergyPerLayerDecorator.h"
#include "CaloUtils/CaloClusterStoreHelper.h"

#include <TString.h>

// Constructor
DerivationFramework::ClusterEnergyPerLayerDecorator::
  ClusterEnergyPerLayerDecorator(const std::string& t,
                                 const std::string& n,
                                 const IInterface* p)
  : AthAlgTool(t, n, p)
{
  declareInterface<DerivationFramework::IAugmentationTool>(this);
  declareProperty("neta", m_eta_size);
  declareProperty("nphi", m_phi_size);
  declareProperty("layers", m_layers = { 0, 1, 2, 3 });
}

// Destructor
DerivationFramework::ClusterEnergyPerLayerDecorator::
  ~ClusterEnergyPerLayerDecorator()
{}

// Athena initialize and finalize
StatusCode
DerivationFramework::ClusterEnergyPerLayerDecorator::initialize()
{
  ATH_MSG_VERBOSE("initialize() ...");

  if (m_SGKey_photons.empty() and m_SGKey_electrons.empty()) {
    ATH_MSG_FATAL("No e-gamma collection provided for thinning. At least one "
                  "egamma collection (photons/electrons) must be provided!");
    return StatusCode::FAILURE;
  }

  ATH_CHECK(m_caloFillRectangularTool.retrieve());
  // how to add a statement to theck that the tool indeed has the size matching
  // the neta and nphi properties? ATH_MSG_DEBUG("CaloFillRectangularCluster
  // print size ...
  m_tool =
    dynamic_cast<const CaloFillRectangularCluster*>(
      &(*m_caloFillRectangularTool));
  if (m_tool==nullptr) {
    ATH_MSG_ERROR("Pointer to CaloFillRectantularCluster tool is invalid");
    return StatusCode::FAILURE;
  }

  ATH_CHECK(m_SGKey_caloCells.initialize());

  if (!m_SGKey_electrons.key().empty()) {
    ATH_MSG_DEBUG("Using " << m_SGKey_electrons << " for electrons");
    ATH_CHECK(m_SGKey_electrons.initialize());

    const char* containerKey = m_SGKey_electrons.key().c_str();
    for (int layer : m_layers) {
      m_SGKey_electrons_decorations.emplace_back(
        Form("%s.E%dx%d_Lr%d", containerKey, m_eta_size, m_phi_size, layer));
    }
    ATH_CHECK(m_SGKey_electrons_decorations.initialize());
  }

  if (!m_SGKey_photons.key().empty()) {
    ATH_MSG_DEBUG("Using " << m_SGKey_photons << " for photons");
    ATH_CHECK(m_SGKey_photons.initialize());

    const char* containerKey = m_SGKey_photons.key().c_str();
    for (int layer : m_layers) {
      m_SGKey_photons_decorations.emplace_back(
        Form("%s.E%dx%d_Lr%d", containerKey, m_eta_size, m_phi_size, layer));
    }
    ATH_CHECK(m_SGKey_photons_decorations.initialize());
  }

  return StatusCode::SUCCESS;
}

StatusCode
DerivationFramework::ClusterEnergyPerLayerDecorator::finalize()
{

  return StatusCode::SUCCESS;
}

// The decoration itself
StatusCode
DerivationFramework::ClusterEnergyPerLayerDecorator::addBranches() const
{
  const EventContext& ctx = Gaudi::Hive::currentContext();

  // Retrieve cell container

  SG::ReadHandle<CaloCellContainer> cellContainer(m_SGKey_caloCells, ctx);
  const CaloCellContainer* cellCont = cellContainer.ptr();

  // Photon decorations

  if (!m_SGKey_photons.key().empty()) {

    // Retrieve photon container
    SG::ReadHandle<xAOD::EgammaContainer> photonContainer(m_SGKey_photons, ctx);
    const xAOD::EgammaContainer* importedPhotons = photonContainer.ptr();

    // Setup vectors of photon decorations
    std::vector<SG::WriteDecorHandle<xAOD::EgammaContainer, float>> decorations;
    for (unsigned int i = 0; i < m_layers.size(); i++) {
      decorations.emplace_back(
        SG::WriteDecorHandle<xAOD::EgammaContainer, float>(
          m_SGKey_photons_decorations[i], ctx));
    }

    // Decorate photons
    for (const auto* photon : *importedPhotons) {
      std::vector<float> result = decorateObject(ctx, photon, cellCont);
      for (unsigned int i = 0; i < m_layers.size(); i++) {
        decorations[i](*photon) = result[i];
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
    std::vector<SG::WriteDecorHandle<xAOD::EgammaContainer, float>> decorations;
    for (unsigned int i = 0; i < m_layers.size(); i++) {
      decorations.emplace_back(
        SG::WriteDecorHandle<xAOD::EgammaContainer, float>(
          m_SGKey_electrons_decorations[i], ctx));
    }

    // Decorate electrons
    for (const auto* electron : *importedElectrons) {
      std::vector<float> result = decorateObject(ctx, electron, cellCont);
      for (unsigned int i = 0; i < m_layers.size(); i++) {
        decorations[i](*electron) = result[i];
      }
    }
  }

  return StatusCode::SUCCESS;
}

std::vector<float>
DerivationFramework::ClusterEnergyPerLayerDecorator::decorateObject(
  const EventContext& ctx,
  const xAOD::Egamma* egamma,
  const CaloCellContainer* cellCont) const
{
  std::vector<float> result;
  result.clear();
  if (not egamma or not egamma->caloCluster())
    return result;

  xAOD::CaloCluster* egcClone(nullptr);
  if (not egamma->author(xAOD::EgammaParameters::AuthorCaloTopo35)) {
    egcClone =
      CaloClusterStoreHelper::makeCluster(cellCont,
                                          egamma->caloCluster()->eta0(),
                                          egamma->caloCluster()->phi0(),
                                          egamma->caloCluster()->clusterSize());
    m_tool->makeCorrection(ctx, egcClone);
  }

  for (unsigned int layer : m_layers) {
    result.emplace_back(egcClone ? egcClone->energyBE(layer) : 0.);
  }
  delete egcClone;

  return result;
}
