/**
 * Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
 */

#ifndef TRIGT1CALOFEXPERF_METPERFFEXBASE_H
#define TRIGT1CALOFEXPERF_METPERFFEXBASE_H

#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "GaudiKernel/EventContext.h"
#include "StoreGate/WriteHandleKey.h"
#include "xAODTrigger/EnergySumRoI.h"

namespace LVL1
{
  class METPerfFexBase : public AthReentrantAlgorithm
  {
  public:
    METPerfFexBase(const std::string &name, ISvcLocator *pSvcLocator);
    virtual ~METPerfFexBase() override;

    virtual StatusCode initialize() override;
    virtual StatusCode execute(const EventContext& ctx) const final;
    virtual StatusCode runFex(
        const EventContext &ctx,
        xAOD::EnergySumRoI &met) const = 0;

  protected:
    SG::WriteHandleKey<xAOD::EnergySumRoI> m_metKey;
  }; //> end class METPerfFexBase
} // namespace LVL1

#endif //> !TRIGT1CALOFEXPERF_METPERFFEXBASE_H
