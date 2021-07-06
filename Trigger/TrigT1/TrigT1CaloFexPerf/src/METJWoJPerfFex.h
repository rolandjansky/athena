/**
 * Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
 */

#ifndef TRIGT1CALOFEXPERF_METJWOJPERFFEX_H
#define TRIGT1CALOFEXPERF_METJWOJPERFFEX_H

#include "METPerfFexBase.h"
#include "StoreGate/ReadHandleKey.h"
#include "StoreGate/WriteHandleKey.h"
#include "xAODTrigL1Calo/GBlockContainer.h"
#include "xAODTrigger/EnergySumRoI.h"

namespace LVL1
{
  class METJWoJPerfFex : public METPerfFexBase
  {
  public:
    METJWoJPerfFex(const std::string &name, ISvcLocator *pSvcLocator);
    virtual ~METJWoJPerfFex() override;

    virtual StatusCode initialize() override;
    virtual StatusCode runFex(
        const EventContext &ctx,
        xAOD::EnergySumRoI &met) const override;

  private:
    SG::ReadHandleKey<xAOD::GBlockContainer> m_gblocksKey;
    SG::WriteHandleKey<xAOD::EnergySumRoI> m_mhtKey;
    SG::WriteHandleKey<xAOD::EnergySumRoI> m_mstKey;
    float m_ptConeCut;
    bool m_useNegativeTowers;
    bool m_useRho;
    SG::ReadHandleKey<xAOD::EnergySumRoI> m_rhoKey;
    std::vector<float> m_sumEtThresholds;
    std::vector<float> m_axValues;
    std::vector<float> m_ayValues;
    std::vector<float> m_bxValues;
    std::vector<float> m_byValues;
    std::vector<float> m_cxValues;
    std::vector<float> m_cyValues;
  }; //> end class METJWoJPerfFex
} // namespace LVL1
#endif //> !TRIGT1CALOFEXPERF_METJWOJPERFFEX_H