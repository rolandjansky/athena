/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/StatusCode.h"

#include "ZdcByteStream/ZdcByteStreamReadV1V2Tool.h"
#include "ByteStreamCnvSvcBase/IROBDataProviderSvc.h"
#include "ZdcByteStream/ZdcDataAccessV2.h"

#include "ZdcByteStream/ZdcToString.h"
#include "xAODTrigL1Calo/TriggerTowerAuxContainer.h"

//namespace LVL1BS {

// Constructor

ZdcDataAccessV2::ZdcDataAccessV2(const std::string& name /*="ZdcDataAccessV2"*/)
                    : AsgTool(name),
 m_tool("ZdcByteStreamReadV1V2Tool/ZdcByteStreamReadV1V2Tool")
{

}


// Initialize

#ifndef PACKAGE_VERSION
#define PACKAGE_VERSION "unknown"
#endif

StatusCode ZdcDataAccessV2::initialize()
{
	ATH_MSG_INFO("Initializing " << name() << " - package version "
                 << PACKAGE_VERSION);
	// Retrieve Tool
	CHECK(m_tool.retrieve());

  return StatusCode::SUCCESS;
}



// Return iterators to required trigger towers

StatusCode ZdcDataAccessV2::loadTriggerTowers(xAOD::TriggerTowerContainer&  container)
{
	CHECK((m_tool->convert(&container)));
	return StatusCode::SUCCESS;
}

StatusCode ZdcDataAccessV2::PrintTriggerTowers()
{
  xAOD::TriggerTowerContainer ttCollection;
  xAOD::TriggerTowerAuxContainer aux;

  ttCollection.setStore(&aux);
  CHECK(loadTriggerTowers(ttCollection));
  ATH_MSG_ALWAYS(ZdcToString(ttCollection));
  return StatusCode::SUCCESS;
}

//} // end namespace
