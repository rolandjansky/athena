/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Gaudi/Athena include(s):
#include "AthenaKernel/errorcheck.h"

// Old mbts (tile) info!
#include "TileEvent/MBTSCollisionTime.h"

//New egamma
#include "xAODForward/ForwardEventInfoContainer.h"
#include "xAODForward/ForwardEventInfoAuxContainer.h"

// Local include(s):
#include "ForwardEventInfoCnvAlg.h"

namespace xAODMaker
{

  ForwardEventInfoCnvAlg::ForwardEventInfoCnvAlg(const std::string &name,
                                                 ISvcLocator *svcLoc)
      : AthReentrantAlgorithm(name, svcLoc) {}

  StatusCode ForwardEventInfoCnvAlg::initialize()
  {

    ATH_CHECK(m_inputKey.initialize());
    ATH_CHECK(m_outputKey.initialize());
    ATH_CHECK(m_cnvTool.retrieve());

    // Return gracefully:
    return StatusCode::SUCCESS;
  }

  StatusCode ForwardEventInfoCnvAlg::execute(const EventContext &ctx) const
  {

    auto mbtsCollisionsTimeHandle = SG::makeHandle(m_inputKey, ctx);
    ATH_MSG_INFO("Retrieved particles with key: " << m_inputKey.key());

    auto fwdEventInfo = std::make_unique<xAOD::ForwardEventInfoContainer>();
    auto fwdEventInfoAux = std::make_unique<xAOD::ForwardEventInfoAuxContainer>();
    fwdEventInfo->setStore(fwdEventInfoAux.get());

    ATH_CHECK(m_cnvTool->convert(mbtsCollisionsTimeHandle.ptr(), fwdEventInfo.get()));

    auto fwdEventInfoHandle = SG::makeHandle(m_outputKey, ctx);
    ATH_CHECK(fwdEventInfoHandle.record(std::move(fwdEventInfo), std::move(fwdEventInfoAux)));
    return StatusCode::SUCCESS;
  }

} // namespace xAODMaker
