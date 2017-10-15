/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "SCT_MonitorConditionsCondAlg.h"

#include "GaudiKernel/EventIDRange.h"

SCT_MonitorConditionsCondAlg::SCT_MonitorConditionsCondAlg(const std::string& name, ISvcLocator* pSvcLocator)
  : ::AthAlgorithm(name, pSvcLocator)
  , m_readKey{"/SCT/Derived/Monitoring"}
  , m_writeKey{"SCT_MonitorConditionsCondData", "SCT_MonitorConditionsCondData"}
  , m_condSvc{"CondSvc", name}
{
  declareProperty("ReadKey", m_readKey);
  declareProperty("WriteKey", m_writeKey);
}

SCT_MonitorConditionsCondAlg::~SCT_MonitorConditionsCondAlg()
{
}

StatusCode SCT_MonitorConditionsCondAlg::initialize()
{
  ATH_MSG_DEBUG("initialize " << name());

  // CondSvc
  ATH_CHECK(m_condSvc.retrieve());

  // Read Cond Handle
  ATH_CHECK(m_readKey.initialize());

  // Write Cond Handle
  ATH_CHECK(m_writeKey.initialize());
  // Register write handle
  if(m_condSvc->regHandle(this, m_writeKey).isFailure()) {
    ATH_MSG_ERROR("unable to register WriteCondHandle " << m_writeKey.fullKey() << " with CondSvc");
    return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;
}

StatusCode SCT_MonitorConditionsCondAlg::execute()
{
  ATH_MSG_DEBUG("execute " << name());

  // Write Cond Handle
  SG::WriteCondHandle<SCT_MonitorConditionsCondData> writeHandle{m_writeKey};

  // Do we have a valid Write Cond Handle for current time?
  if(writeHandle.isValid()) {
    // in theory this should never be called in MT
    writeHandle.updateStore();
    ATH_MSG_DEBUG("CondHandle " << writeHandle.fullKey() << " is already valid."
                  << ". In theory this should not be called, but may happen"
                  << " if multiple concurrent events are being processed out of order."
                  << " Forcing update of Store contents");
    return StatusCode::SUCCESS;
  }

  // Read Cond Handle
  SG::ReadCondHandle<CondAttrListCollection> readHandle{m_readKey};
  const CondAttrListCollection* readCdo{*readHandle};
  if(readCdo==nullptr) {
    ATH_MSG_ERROR("Null pointer to the read conditions object");
    return StatusCode::FAILURE;
  }
  ATH_MSG_INFO("Size of CondAttrListCollection readCdo->size()= " << readCdo->size());

  // Define validity of the output cond obbject
  EventIDRange rangeW;
  if(not readHandle.range(rangeW)) {
    ATH_MSG_ERROR("Failed to retrieve validity range for " << readHandle.key());
    return StatusCode::FAILURE;
  }

  // Construct the output Cond Object and fill it in
  SCT_MonitorConditionsCondData* writeCdo{new SCT_MonitorConditionsCondData()};

  // Fill Write Cond Handle
  static const unsigned int defectListIndex{7};
  CondAttrListCollection::const_iterator iter{readCdo->begin()};
  CondAttrListCollection::const_iterator last{readCdo->end()};
  for(; iter!=last; ++iter) {
    const AthenaAttributeList& list{iter->second};
    if(list.size()>defectListIndex) {
      writeCdo->insert(iter->first, list[defectListIndex].data<std::string>());
    }
  }

  // Record validity of the output cond obbject
  if(writeHandle.record(rangeW, writeCdo).isFailure()) {
    ATH_MSG_ERROR("Could not record SCT_TdaqEnabledCondData " << writeHandle.key()
                  << " with EventRange " << rangeW
                  << " into Conditions Store");
    return StatusCode::FAILURE;
  }
  ATH_MSG_INFO("recorded new CDO " << writeHandle.key() << " with range " << rangeW << " into Conditions Store");

  return StatusCode::SUCCESS;
}

StatusCode SCT_MonitorConditionsCondAlg::finalize()
{
  ATH_MSG_DEBUG("finalize " << name());
  return StatusCode::SUCCESS;
}
