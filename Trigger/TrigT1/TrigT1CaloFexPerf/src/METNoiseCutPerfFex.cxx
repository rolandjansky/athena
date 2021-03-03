/**
 * Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
 */

#include "METNoiseCutPerfFex.h"
#include "StoreGate/ReadHandle.h"

namespace LVL1
{
  METNoiseCutPerfFex::METNoiseCutPerfFex(const std::string &name, ISvcLocator *pSvcLocator)
      : METPerfFexBase(name, pSvcLocator)
  {
    declareProperty("InputTowers", m_towersKey, "Input JG towers");
    declareProperty("InputTowerNoise", m_noiseName = "", "Input JG tower noise values (if used)");
    declareProperty("UseNegativeTowers", m_useNegativeTowers = true, "Whether to use negative towers");
  }

  METNoiseCutPerfFex::~METNoiseCutPerfFex()
  {
  }

  StatusCode METNoiseCutPerfFex::initialize()
  {
    ATH_CHECK(METPerfFexBase::initialize());
    ATH_CHECK(m_towersKey.initialize());
    if (!m_noiseName.empty())
      m_noiseAcc = std::make_unique<SG::AuxElement::ConstAccessor<float>>(m_noiseName);
    return StatusCode::SUCCESS;
  }

  StatusCode METNoiseCutPerfFex::runFex(
      const EventContext &ctx,
      xAOD::EnergySumRoI &met) const
  {
    auto towers = SG::makeHandle(m_towersKey, ctx);
    if (!towers.isValid())
    {
      ATH_MSG_ERROR("Failed to retrieve " << towers);
      return StatusCode::FAILURE;
    }
    float mpx = 0;
    float mpy = 0;
    float sumEt = 0;
    for (const xAOD::JGTower *tower : *towers)
    {
      // Negative tower selection
      if (!m_useNegativeTowers && tower->et() < 0)
        continue;

      // Tower noise selection
      if (m_noiseAcc)
      {
        if (tower->sampling() == 0)
        {
          if (std::abs(tower->eta()) < 3.2 && std::abs(tower->et()) < 4.5 * (*m_noiseAcc)(*tower))
            continue;
        }
        else if (tower->et() < 5 * (*m_noiseAcc)(*tower))
          continue;
      }
      sumEt += tower->et();
      mpx -= tower->et() * std::cos(tower->phi());
      mpy -= tower->et() * std::sin(tower->phi());
    }

    // Technically the energy X value should be the negative of mpx but I think
    // that this is how it's done in the current code
    met.setEnergyX(mpx);
    met.setEnergyY(mpy);
    met.setEnergyT(sumEt);
    return StatusCode::SUCCESS;
  }
} // namespace LVL1
