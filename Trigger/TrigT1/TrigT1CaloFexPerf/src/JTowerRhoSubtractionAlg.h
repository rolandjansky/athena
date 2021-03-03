/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGT1CALOFEXPERF_JTOWERRHOSUBTRACTIONALG_H
#define TRIGT1CALOFEXPERF_JTOWERRHOSUBTRACTIONALG_H

#include "AthenaBaseComps/AthAlgorithm.h"
#include "StoreGate/ReadHandleKey.h"
#include "StoreGate/WriteHandleKey.h"
#include "xAODTrigL1Calo/JGTowerContainer.h"
#include "xAODTrigger/EnergySumRoI.h"
#include "GaudiKernel/ToolHandle.h"
#include "IJGTowerMappingMaker.h"

namespace LVL1
{
  class JTowerRhoSubtractionAlg : public AthAlgorithm
  {
  public:
    JTowerRhoSubtractionAlg(const std::string &name, ISvcLocator *pSvcLocator);
    virtual ~JTowerRhoSubtractionAlg() override;

    virtual StatusCode initialize() override;
    virtual StatusCode execute() override;

  private:
    SG::ReadHandleKey<xAOD::JGTowerContainer> m_inputKey;
    SG::WriteHandleKey<xAOD::JGTowerContainer> m_outputKey;
    SG::WriteHandleKey<xAOD::EnergySumRoI> m_outputRhoKey;
    float m_minTowerRho;
    float m_maxTowerRho;
    float m_minOutputTowerRho;

    // Internals
    StatusCode buildFexBins();
    std::vector<std::vector<std::size_t>> m_jFEXBins;
    std::vector<std::vector<std::size_t>> m_jFEXBinsCore;
    ToolHandle<IJGTowerMappingMaker> m_mappingMaker;
  }; //> end class JTowerRhoSubtractionAlg
} // namespace LVL1

#endif //> !TRIGT1CALOFEXPERF_JTOWERRHOSUBTRACTIONALG_H