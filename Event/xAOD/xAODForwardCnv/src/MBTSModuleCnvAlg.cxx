/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Gaudi/Athena include(s):
#include "AthenaKernel/errorcheck.h"

// Old mbts (tile) info!
#include "TileEvent/TileCellContainer.h"

//New egamma
#include "xAODForward/MBTSModuleContainer.h"
#include "xAODForward/MBTSModuleAuxContainer.h"

// Local include(s):
#include "MBTSModuleCnvAlg.h"

namespace xAODMaker
{

  MBTSModuleCnvAlg::MBTSModuleCnvAlg(const std::string &name,
                                     ISvcLocator *svcLoc)
      : AthReentrantAlgorithm(name, svcLoc) {}

  StatusCode MBTSModuleCnvAlg::initialize()
  {
    ATH_CHECK(m_inputKey.initialize());
    ATH_CHECK(m_outputKey.initialize());
    ATH_CHECK(m_cnvTool.retrieve());
    return StatusCode::SUCCESS;
  }

  StatusCode MBTSModuleCnvAlg::execute(const EventContext &ctx) const
  {

    auto tileCellsHandle = SG::makeHandle(m_inputKey, ctx);
    ATH_CHECK(tileCellsHandle.isValid()); // technically not needed
    ATH_MSG_DEBUG("Retrieved tile cells with key: " << m_inputKey.key() << " of size " << tileCellsHandle->size());

    auto modulesContainer = std::make_unique<xAOD::MBTSModuleContainer>();
    auto modulesAuxContainer = std::make_unique<xAOD::MBTSModuleAuxContainer>();
    modulesContainer->setStore(modulesAuxContainer.get());

    ATH_CHECK(m_cnvTool->convert(tileCellsHandle.ptr(), modulesContainer.get()));

    auto modulesHandle = SG::makeHandle(m_outputKey, ctx);
    ATH_CHECK(modulesHandle.record(std::move(modulesContainer), std::move(modulesAuxContainer)));
    ATH_MSG_DEBUG("Recorded xAODMBTSModules with key: " << m_outputKey.key() << " of size " << modulesContainer->size());

    return StatusCode::SUCCESS;
  }
}
