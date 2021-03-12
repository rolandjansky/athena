/**
 * Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
 */

#include "METJWoJPerfFex.h"
#include "StoreGate/ReadHandle.h"
#include "xAODTrigger/EnergySumRoIAuxInfo.h"
#include "GaudiKernel/SystemOfUnits.h"
#include "GTowerHelpers.h"

namespace
{
  const static SG::AuxElement::ConstAccessor<std::vector<float>> accRho("FPGARhos");
  const static SG::AuxElement::ConstAccessor<float> accArea("area");
} // namespace

namespace LVL1
{
  METJWoJPerfFex::METJWoJPerfFex(const std::string &name, ISvcLocator *pSvcLocator)
      : METPerfFexBase(name, pSvcLocator)
  {
    declareProperty("InputGBlocks", m_gblocksKey, "Input gBlocks");
    declareProperty("OutputMHT", m_mhtKey, "Output container for the hard term");
    declareProperty("OutputMST", m_mstKey, "Output container for the soft term");
    declareProperty("PtConeCut", m_ptConeCut = 25 * Gaudi::Units::GeV, "Cone threshold to separate hard and soft terms");
    declareProperty("UseNegativeTowers", m_useNegativeTowers = true);
    declareProperty("UseRho", m_useRho = false);
    declareProperty("InputRho", m_rhoKey);
    declareProperty("SumEtThresholds", m_sumEtThresholds = {}); 
    declareProperty("axValues", m_axValues = {0.98});
    declareProperty("ayValues", m_ayValues = {0.98}); 
    declareProperty("bxValues", m_bxValues = {0.4}); 
    declareProperty("byValues", m_byValues = {0.4}); 
    declareProperty("cxValues", m_cxValues = {0.0});
    declareProperty("cyValues", m_cyValues = {0.0}); 
  }

  METJWoJPerfFex::~METJWoJPerfFex() {}

  StatusCode METJWoJPerfFex::initialize()
  {
    ATH_CHECK(METPerfFexBase::initialize());
    ATH_CHECK(m_gblocksKey.initialize());
    ATH_CHECK(m_mhtKey.initialize());
    ATH_CHECK(m_mstKey.initialize());
    if (m_useRho)
      ATH_CHECK(m_rhoKey.initialize());
    if (m_axValues.size() != m_sumEtThresholds.size() + 1 ||
        m_ayValues.size() != m_sumEtThresholds.size() + 1 ||
        m_bxValues.size() != m_sumEtThresholds.size() + 1 ||
        m_byValues.size() != m_sumEtThresholds.size() + 1 ||
        m_cxValues.size() != m_sumEtThresholds.size() + 1 ||
        m_cyValues.size() != m_sumEtThresholds.size() + 1)
    {
      ATH_MSG_ERROR("Incorrect number of algorithm parameter values given number of sumEt thresholds");
      return StatusCode::FAILURE;
    }
    return StatusCode::SUCCESS;
  }

  StatusCode METJWoJPerfFex::runFex(
      const EventContext &ctx,
      xAOD::EnergySumRoI &met) const
  {
    auto gblocks = SG::makeHandle(m_gblocksKey, ctx);
    if (!gblocks.isValid())
    {
      ATH_MSG_ERROR("Failed to retrieve " << m_gblocksKey.key());
      return StatusCode::FAILURE;
    }
    std::vector<float> rhos;
    if (m_useRho)
    {
      auto rhoCont = SG::makeHandle(m_rhoKey, ctx);
      if (!rhoCont.isValid())
      {
        ATH_MSG_ERROR("Failed to retrieve " << m_rhoKey.key());
        return StatusCode::FAILURE;
      }
      rhos = accRho(*rhoCont);
    }

    float HTx = 0;
    float HTy = 0;
    float sumHT = 0;
    float STx = 0;
    float STy = 0;
    float sumST = 0;
    float sumEt = 0;

    for (const xAOD::GBlock *iblock : *gblocks)
    {
      const xAOD::JGTower *seed = iblock->seedTower();
      if (!seed)
      {
        ATH_MSG_ERROR("Could not retrieve seed tower!");
        return StatusCode::FAILURE;
      }
      float seedEt = seed->et();
      if (!m_useNegativeTowers && seedEt < 0)
        // Skip blocks with negative seeds
        continue;
      // Compute global variables
      sumEt += seedEt;
      if (iblock->pt() > m_ptConeCut)
      {
        if (m_useRho)
        {
          gFEX::FPGA fpga = gFEX::getFPGA(seed->eta());
          float rho = rhos.at(static_cast<std::size_t>(fpga));
          seedEt -= rho * accArea(*seed);
          if (seedEt < 0)
            seedEt = 0;
        }
        HTx += seedEt * std::cos(seed->phi());
        HTy += seedEt * std::sin(seed->phi());
        sumHT += seedEt;
      }
      else
      {
        STx += seedEt * std::cos(seed->phi());
        STy += seedEt * std::sin(seed->phi());
        sumST += seedEt;
      }
    }
    // Select the correct a/b/c values
    float ax = m_axValues.back();
    float ay = m_ayValues.back();
    float bx = m_bxValues.back();
    float by = m_byValues.back();
    float cx = m_cxValues.back();
    float cy = m_cyValues.back();
    for (std::size_t idx = 0; idx < m_sumEtThresholds.size(); ++idx)
    {
      if (sumEt <= m_sumEtThresholds.at(idx))
      {
        ax = m_axValues.at(idx);
        ay = m_ayValues.at(idx);
        bx = m_bxValues.at(idx);
        by = m_byValues.at(idx);
        cx = m_cxValues.at(idx);
        cy = m_cyValues.at(idx);
        break;
      }
    }
    float energyX = ax * HTx + bx * STx + cx;
    float energyY = ay * HTy + by * STy + cy;
    met.setEnergyX(energyX);
    met.setEnergyY(energyY);
    met.setEnergyT(sumEt);

    auto mht = std::make_unique<xAOD::EnergySumRoI>();
    auto mhtAux = std::make_unique<xAOD::EnergySumRoIAuxInfo>();
    mht->setStore(mhtAux.get());
    mht->setEnergyX(HTx);
    mht->setEnergyY(HTy);
    mht->setEnergyT(sumHT);
    auto mhtHandle = SG::makeHandle(m_mhtKey, ctx);
    ATH_CHECK(mhtHandle.record(std::move(mht), std::move(mhtAux)));

    auto mst = std::make_unique<xAOD::EnergySumRoI>();
    auto mstAux = std::make_unique<xAOD::EnergySumRoIAuxInfo>();
    mst->setStore(mstAux.get());
    mst->setEnergyX(STx);
    mst->setEnergyY(STy);
    mst->setEnergyT(sumST);
    auto mstHandle = SG::makeHandle(m_mstKey, ctx);
    ATH_CHECK(mstHandle.record(std::move(mst), std::move(mstAux)));

    return StatusCode::SUCCESS;
  }
} // namespace LVL1
