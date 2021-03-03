/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGT1CALOFEXPERF_GBLOCKBUILDER_H
#define TRIGT1CALOFEXPERF_GBLOCKBUILDER_H

#include "AthenaBaseComps/AthAlgorithm.h"
#include "StoreGate/ReadHandleKey.h"
#include "StoreGate/WriteHandleKey.h"
#include "xAODTrigL1Calo/JGTowerContainer.h"
#include "xAODTrigL1Calo/GBlockContainer.h"

namespace LVL1
{
  class GBlockBuilder : public AthAlgorithm
  {
  public:
    GBlockBuilder(const std::string &name, ISvcLocator *pSvcLocator);
    virtual ~GBlockBuilder() override;

    virtual StatusCode initialize() override;
    virtual StatusCode execute() override;

  private:
    SG::ReadHandleKey<xAOD::JGTowerContainer> m_towersKey;
    SG::WriteHandleKey<xAOD::GBlockContainer> m_gBlocksKey;
    float m_forwardEta;
  }; //> end class GBlockBuilder
} // namespace LVL1

#endif //> !TRIGT1CALOFEXPERF_GBLOCKBUILDER_H