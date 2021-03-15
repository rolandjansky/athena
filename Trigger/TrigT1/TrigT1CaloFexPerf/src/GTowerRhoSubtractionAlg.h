/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGT1CALOFEXPERF_GTOWERRHOSUBTRACTIONALG_H
#define TRIGT1CALOFEXPERF_GTOWERRHOSUBTRACTIONALG_H

#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "StoreGate/ReadHandleKey.h"
#include "StoreGate/WriteHandleKey.h"
#include "xAODTrigL1Calo/JGTowerContainer.h"
#include "xAODTrigger/EnergySumRoI.h"

namespace LVL1
{
  class GTowerRhoSubtractionAlg : public AthReentrantAlgorithm
  {
  public:
    GTowerRhoSubtractionAlg(const std::string &name, ISvcLocator *pSvcLocator);
    virtual ~GTowerRhoSubtractionAlg() override;

    virtual StatusCode initialize() override;
    virtual StatusCode execute(const EventContext& ctx) const override;

  private:
    SG::ReadHandleKey<xAOD::JGTowerContainer> m_inputKey;
    SG::WriteHandleKey<xAOD::JGTowerContainer> m_outputKey;
    SG::WriteHandleKey<xAOD::EnergySumRoI> m_outputRhoKey;
    bool m_useNegativeTowers;
    float m_maxTowerEt;
    bool m_forcePosRho;

    float calculateRho(const std::vector<const xAOD::JGTower *> &towers) const;
  }; //> end class GTowerRhoSubtractionAlg
} // namespace LVL1

#endif //> !TRIGT1CALOFEXPERF_JTOWERRHOSUBTRACTIONALG_H