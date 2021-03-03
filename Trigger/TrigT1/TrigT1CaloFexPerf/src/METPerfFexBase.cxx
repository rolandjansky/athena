/**
 * Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
 */

#include "METPerfFexBase.h"
#include "StoreGate/ReadHandle.h"
#include "StoreGate/WriteHandle.h"
#include "GaudiKernel/ThreadLocalContext.h"
#include <memory>
#include "xAODTrigger/EnergySumRoIAuxInfo.h"

namespace LVL1
{
  METPerfFexBase::METPerfFexBase(const std::string &name, ISvcLocator *pSvcLocator)
      : AthAlgorithm(name, pSvcLocator)
  {
    declareProperty("OutputMET", m_metKey, "Output MET");
  }

  METPerfFexBase::~METPerfFexBase() {}

  StatusCode METPerfFexBase::initialize()
  {
    ATH_CHECK(m_metKey.initialize());
    return StatusCode::SUCCESS;
  }

  StatusCode METPerfFexBase::execute()
  {
    const EventContext &ctx = Gaudi::Hive::currentContext();

    auto met = std::make_unique<xAOD::EnergySumRoI>();
    auto metAux = std::make_unique<xAOD::EnergySumRoIAuxInfo>();
    met->setStore(metAux.get());
    ATH_CHECK(runFex(ctx, *met));
    auto metHandle = SG::makeHandle(m_metKey, ctx);
    ATH_CHECK(metHandle.record(std::move(met), std::move(metAux)));

    return StatusCode::SUCCESS;
  }

} // namespace LVL1
