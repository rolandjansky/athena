/**
 * Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
 */

#ifndef TRIGT1CALOFEXPERF_METNOISECUTPERFFEX_H
#define TRIGT1CALOFEXPERF_METNOISECUTPERFFEX_H

#include "METPerfFexBase.h"
#include "AthContainers/AuxElement.h"
#include "StoreGate/ReadHandleKey.h"
#include "xAODTrigL1Calo/JGTowerContainer.h"
#include <memory>

namespace LVL1
{
  class METNoiseCutPerfFex : public METPerfFexBase
  {
  public:
    METNoiseCutPerfFex(const std::string &name, ISvcLocator *pSvcLocator);
    virtual ~METNoiseCutPerfFex() override;

    virtual StatusCode initialize() override;
    virtual StatusCode runFex(
        const EventContext &ctx,
        xAOD::EnergySumRoI &met) const override;

  private:
    SG::ReadHandleKey<xAOD::JGTowerContainer> m_towersKey;
    std::string m_noiseName;
    bool m_useNegativeTowers;

    std::unique_ptr<SG::AuxElement::ConstAccessor<float>> m_noiseAcc;
  }; //> end class METNoiseCutPerfFex
} // namespace LVL1

#endif //> !TRIGT1CALOFEXPERF_METNOISECUTPERFFEX_H
