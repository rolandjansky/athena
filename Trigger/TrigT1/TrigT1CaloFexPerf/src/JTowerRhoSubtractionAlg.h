/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGT1CALOFEXPERF_JTOWERRHOSUBTRACTIONALG_H
#define TRIGT1CALOFEXPERF_JTOWERRHOSUBTRACTIONALG_H

#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "StoreGate/ReadHandleKey.h"
#include "StoreGate/WriteHandleKey.h"
#include "StoreGate/ReadCondHandleKey.h"
#include "xAODTrigL1Calo/JGTowerContainer.h"
#include "xAODTrigger/EnergySumRoI.h"
#include "TrigT1CaloFexPerf/JGTowerMappingData.h"
#include <atomic>
#include <mutex>

namespace LVL1
{
  class JTowerRhoSubtractionAlg : public AthReentrantAlgorithm
  {
  public:
    JTowerRhoSubtractionAlg(const std::string &name, ISvcLocator *pSvcLocator);
    virtual ~JTowerRhoSubtractionAlg() override;

    virtual StatusCode initialize() override;
    virtual StatusCode execute(const EventContext &ctx) const override;

  private:
    SG::ReadHandleKey<xAOD::JGTowerContainer> m_inputKey;
    SG::WriteHandleKey<xAOD::JGTowerContainer> m_outputKey;
    SG::WriteHandleKey<xAOD::EnergySumRoI> m_outputRhoKey;

    float m_minTowerRho;
    float m_maxTowerRho;
    float m_minOutputTowerRho;

    // Internals
    StatusCode buildFexBins(const xAOD::JGTowerContainer *towers) const;
    mutable std::mutex m_mutex;
    mutable std::atomic_bool m_builtFexBins{false};
    mutable std::vector<std::vector<std::size_t>> m_jFEXBins;
    mutable std::vector<std::vector<std::size_t>> m_jFEXBinsCore;
  }; //> end class JTowerRhoSubtractionAlg
} // namespace LVL1

#endif //> !TRIGT1CALOFEXPERF_JTOWERRHOSUBTRACTIONALG_H