/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/StatusCode.h"

#include "xaod/PpmByteStreamReadV1V2Tool.h"
#include "ByteStreamCnvSvcBase/IROBDataProviderSvc.h"
#include "TrigT1CaloDataAccessV2.h"

#include "ToString.h"
#include "xAODTrigL1Calo/TriggerTowerAuxContainer.h"

namespace LVL1BS {

// Constructor

TrigT1CaloDataAccessV2::TrigT1CaloDataAccessV2(const std::string& name /*="TrigT1CaloDataAccessV2"*/)
                    : AsgTool(name),
 m_tool("LVL1BS::PpmByteStreamReadV1V2Tool/PpmByteStreamReadV1V2Tool")
{

}


// Initialize

#ifndef PACKAGE_VERSION
#define PACKAGE_VERSION "unknown"
#endif

StatusCode TrigT1CaloDataAccessV2::initialize()
{
	ATH_MSG_INFO("Initializing " << name() << " - package version "
                 << PACKAGE_VERSION);
	// Retrieve Tool
	CHECK(m_tool.retrieve());

  return StatusCode::SUCCESS;
}



// Return iterators to required trigger towers

StatusCode TrigT1CaloDataAccessV2::loadTriggerTowers(xAOD::TriggerTowerContainer&  container)
{
  CHECK(m_tool->convert(&container));
	return StatusCode::SUCCESS;
}

StatusCode TrigT1CaloDataAccessV2::PrintTriggerTowers()
{
  xAOD::TriggerTowerContainer ttCollection;
  xAOD::TriggerTowerAuxContainer aux;

  ttCollection.setStore(&aux);
  CHECK(loadTriggerTowers(ttCollection));
  ATH_MSG_ALWAYS(ToString(ttCollection));
  return StatusCode::SUCCESS;
}

} // end namespace
