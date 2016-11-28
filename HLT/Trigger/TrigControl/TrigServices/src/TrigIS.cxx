/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file   TrigIS.cxx
 * @brief  Implementation of TrigIS service
 * @author Frank Winklmeier
 *
 * $Id: TrigIS.cxx 5 2013-05-14 10:33:04Z ricab $
 */

#include "TrigServices/TrigIS.h"
#include "is/info.h"

TrigIS::TrigIS(const std::string& name, ISvcLocator* pSvcLocator) :
  AthService(name, pSvcLocator),
  m_validPartition(false)
{
  declareProperty("Partition", m_partitionName = "NONE", "Partition name");
}

TrigIS::~TrigIS()
{
}

StatusCode TrigIS::queryInterface(const InterfaceID& riid, void** ppvInterface)
{
  if ( ppvInterface==0 ) return StatusCode::FAILURE;

  if ( ITrigIS::interfaceID().versionMatch(riid) ) {
    *ppvInterface = static_cast<ITrigIS*>(this);
    addRef();
    return StatusCode::SUCCESS;
  }
  else {
    return AthService::queryInterface(riid, ppvInterface);
  }
}

StatusCode TrigIS::initialize()
{
  if (m_partitionName=="NONE") {
    ATH_MSG_WARNING("No partition specified. Will not publish any data to IS.");
    return StatusCode::SUCCESS;
  }
  
  // Try to initialize IPC core
  try {
    int nIPCOpts = 0;
    IPCCore::init(nIPCOpts, 0);
    m_partition = IPCPartition(m_partitionName);
    m_isDict = ISInfoDictionary(m_partition);
  }
  catch ( daq::ipc::Exception & ex ) {
    ATH_MSG_ERROR("Failure during initialize: " << ex.what());
    return StatusCode::FAILURE;
  }

  m_validPartition = true;
  return StatusCode::SUCCESS;
}


StatusCode TrigIS::publish(const std::string& name, const ISInfo& info, bool keep_history)
{
  if (m_validPartition) {
    try {
      m_isDict.checkin(name, info, keep_history);
    }
    catch ( daq::is::Exception& ex ) {
      ATH_MSG_ERROR("Failure publishing " << name << ": " << ex.what());
      return StatusCode::FAILURE;
    }
  }
  return StatusCode::SUCCESS;
}

// Dummy implementation for now
void TrigIS::publish_async(const std::string& name, ISInfo* info, bool keep_history)
{
  publish(name, *info, keep_history).ignore();
  delete info;
}

