/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////
// MaxCellDecorator.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#include "DerivationFrameworkCalo/MaxCellDecorator.h"
#include "CaloIdentifier/CaloCell_ID.h"
#include "CaloUtils/CaloClusterStoreHelper.h"
#include "xAODEgamma/EgammaContainer.h"

#include <string>
#include <vector>
namespace {}

// Constructor
DerivationFramework::MaxCellDecorator::MaxCellDecorator(const std::string& t,
                                                        const std::string& n,
                                                        const IInterface* p)
  : AthAlgTool(t, n, p)
{
  declareInterface<DerivationFramework::IAugmentationTool>(this);
}

// Destructor
DerivationFramework::MaxCellDecorator::~MaxCellDecorator() {}

// Athena initialize and finalize
StatusCode
DerivationFramework::MaxCellDecorator::initialize()
{
  ATH_MSG_VERBOSE("initialize() ...");

  ATH_CHECK(m_cablingKey.initialize());

  if (!m_SGKey_electrons.key().empty()) {
    const std::string key = m_SGKey_electrons.key();
    ATH_MSG_INFO("Using " << key << " for electrons");
    ATH_CHECK(m_SGKey_electrons.initialize());

    // setup vector of decorators
    m_SGKey_electrons_decorations.emplace_back(key + ".maxEcell_time");
    m_SGKey_electrons_decorations.emplace_back(key + ".maxEcell_energy");
    m_SGKey_electrons_decorations.emplace_back(key + ".maxEcell_gain");
    m_SGKey_electrons_decorations.emplace_back(key + ".maxEcell_onlId");
    m_SGKey_electrons_decorations.emplace_back(key + ".maxEcell_x");
    m_SGKey_electrons_decorations.emplace_back(key + ".maxEcell_y");
    m_SGKey_electrons_decorations.emplace_back(key + ".maxEcell_z");
    ATH_CHECK(m_SGKey_electrons_decorations.initialize());
  }

  if (!m_SGKey_photons.key().empty()) {
    const std::string key = m_SGKey_photons.key();
    ATH_MSG_INFO("Using " << key << " for photons");
    ATH_CHECK(m_SGKey_photons.initialize());

    // setup vector of decorators
    m_SGKey_photons_decorations.emplace_back(key + ".maxEcell_time");
    m_SGKey_photons_decorations.emplace_back(key + ".maxEcell_energy");
    m_SGKey_photons_decorations.emplace_back(key + ".maxEcell_gain");
    m_SGKey_photons_decorations.emplace_back(key + ".maxEcell_onlId");
    m_SGKey_photons_decorations.emplace_back(key + ".maxEcell_x");
    m_SGKey_photons_decorations.emplace_back(key + ".maxEcell_y");
    m_SGKey_photons_decorations.emplace_back(key + ".maxEcell_z");
    ATH_CHECK(m_SGKey_photons_decorations.initialize());
  }

  return StatusCode::SUCCESS;
}

StatusCode
DerivationFramework::MaxCellDecorator::finalize()
{
  return StatusCode::SUCCESS;
}

// The decoration itself
StatusCode
DerivationFramework::MaxCellDecorator::addBranches() const
{
  const EventContext& ctx = Gaudi::Hive::currentContext();

  if (!m_SGKey_photons.key().empty()) {
    // Retrieve photon container
    SG::ReadHandle<xAOD::EgammaContainer> photonContainer(m_SGKey_photons, ctx);
    // setup vector of decorators
    SG::WriteDecorHandle<xAOD::EgammaContainer, float> decorationPh0(
      m_SGKey_photons_decorations[0], ctx);
    SG::WriteDecorHandle<xAOD::EgammaContainer, float> decorationPh1(
      m_SGKey_photons_decorations[1], ctx);
    SG::WriteDecorHandle<xAOD::EgammaContainer, int> decorationPh2(
      m_SGKey_photons_decorations[2], ctx);
    SG::WriteDecorHandle<xAOD::EgammaContainer, uint64_t> decorationPh3(
      m_SGKey_photons_decorations[3], ctx);
    SG::WriteDecorHandle<xAOD::EgammaContainer, float> decorationPh4(
      m_SGKey_photons_decorations[4], ctx);
    SG::WriteDecorHandle<xAOD::EgammaContainer, float> decorationPh5(
      m_SGKey_photons_decorations[5], ctx);
    SG::WriteDecorHandle<xAOD::EgammaContainer, float> decorationPh6(
      m_SGKey_photons_decorations[6], ctx);

    const xAOD::EgammaContainer* importedPhotons = photonContainer.ptr();
    for (const auto* egamma : *importedPhotons) {
      DerivationFramework::MaxCellDecorator::calculation res =
        decorateObject(egamma, ctx);
      decorationPh0(*egamma) = res.maxEcell_time;
      decorationPh1(*egamma) = res.maxEcell_energy;
      decorationPh2(*egamma) = res.maxEcell_gain;
      decorationPh3(*egamma) = res.maxEcell_onlId;
      decorationPh4(*egamma) = res.maxEcell_x;
      decorationPh5(*egamma) = res.maxEcell_y;
      decorationPh6(*egamma) = res.maxEcell_z;
    }
  }

  if (!m_SGKey_electrons.key().empty()) {
    // Retrieve electron container
    SG::ReadHandle<xAOD::EgammaContainer> electronContainer(m_SGKey_electrons,
                                                            ctx);

    // setup vector of decorators
    SG::WriteDecorHandle<xAOD::EgammaContainer, float> decorationEl0(
      m_SGKey_electrons_decorations[0], ctx);
    SG::WriteDecorHandle<xAOD::EgammaContainer, float> decorationEl1(
      m_SGKey_electrons_decorations[1], ctx);
    SG::WriteDecorHandle<xAOD::EgammaContainer, int> decorationEl2(
      m_SGKey_electrons_decorations[2], ctx);
    SG::WriteDecorHandle<xAOD::EgammaContainer, uint64_t> decorationEl3(
      m_SGKey_electrons_decorations[3], ctx);
    SG::WriteDecorHandle<xAOD::EgammaContainer, float> decorationEl4(
      m_SGKey_electrons_decorations[4], ctx);
    SG::WriteDecorHandle<xAOD::EgammaContainer, float> decorationEl5(
      m_SGKey_electrons_decorations[5], ctx);
    SG::WriteDecorHandle<xAOD::EgammaContainer, float> decorationEl6(
      m_SGKey_electrons_decorations[6], ctx);

    const xAOD::EgammaContainer* importedElectrons = electronContainer.ptr();
    for (const auto* egamma : *importedElectrons) {
      DerivationFramework::MaxCellDecorator::calculation res =
        decorateObject(egamma, ctx);
      decorationEl0(*egamma) = res.maxEcell_time;
      decorationEl1(*egamma) = res.maxEcell_energy;
      decorationEl2(*egamma) = res.maxEcell_gain;
      decorationEl3(*egamma) = res.maxEcell_onlId;
      decorationEl4(*egamma) = res.maxEcell_x;
      decorationEl5(*egamma) = res.maxEcell_y;
      decorationEl6(*egamma) = res.maxEcell_z;
    }
  }

  return StatusCode::SUCCESS;
}

DerivationFramework::MaxCellDecorator::calculation
DerivationFramework::MaxCellDecorator::decorateObject(
  const xAOD::Egamma* egamma,
  const EventContext& ctx) const
{

  DerivationFramework::MaxCellDecorator::calculation result;

  const xAOD::CaloCluster* cluster = egamma->caloCluster();

  if (cluster) {
    if (!cluster->getCellLinks()) {
      ATH_MSG_WARNING("CellLinks not found");
    }

    SG::ReadCondHandle<LArOnOffIdMapping> cablingHdl{ m_cablingKey, ctx };
    const LArOnOffIdMapping* cabling{ *cablingHdl };
    if (!cabling) {
      ATH_MSG_ERROR("Do not have mapping object " << m_cablingKey.key());
      return result;
    }

    float emax = -9999.;

    const CaloCell* cell_maxE = nullptr;
    for (const CaloCell* cell : *cluster) {
      int sampling = cell->caloDDE()->getSampling();
      if (sampling == CaloCell_ID::EMB2 || sampling == CaloCell_ID::EME2) {
        if ((cell->provenance() & 0x2000)) {
          if (cell->energy() > emax) {
            emax = cell->energy();
            cell_maxE = cell;
          }
        }
      }
    }

    if (cell_maxE) {
      const CaloDetDescrElement* caloDDEl = cell_maxE->caloDDE();
      result.maxEcell_time = cell_maxE->time();
      result.maxEcell_energy = cell_maxE->energy();
      result.maxEcell_gain = (int)cell_maxE->gain();
      result.maxEcell_onlId =
        (uint64_t)(cabling->createSignalChannelID(caloDDEl->identify()))
          .get_compact();
      result.maxEcell_x = caloDDEl->x();
      result.maxEcell_y = caloDDEl->y();
      result.maxEcell_z = caloDDEl->z();
    }
  }
  return result;
}
