/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonCacheCreator.h"

/// Constructor
MuonCacheCreator::MuonCacheCreator(const std::string &name,ISvcLocator *pSvcLocator):
  IDCCacheCreatorBase(name,pSvcLocator),
  m_MdtCsmCacheKey(""),
  m_CscCacheKey(""),
  m_RpcCacheKey(""),
  m_TgcCacheKey("")
{
  declareProperty("MdtCsmCacheKey", m_MdtCsmCacheKey);
  declareProperty("CscCacheKey",    m_CscCacheKey);
  declareProperty("RpcCacheKey",    m_RpcCacheKey);
  declareProperty("TgcCacheKey",    m_TgcCacheKey);
}

StatusCode MuonCacheCreator::initialize() {
  ATH_CHECK( m_idHelperSvc.retrieve() );
  ATH_CHECK( m_MdtCsmCacheKey.initialize(SG::AllowEmpty) );
  if (!m_CscCacheKey.key().empty() && !m_idHelperSvc->hasCSC()) {
    ATH_MSG_WARNING("CscCacheKey is non-empty ("<<m_CscCacheKey.key()<<") but layout has no CSC chambers! Looks like something is wrongly configured.");
  } else ATH_CHECK( m_CscCacheKey.initialize(SG::AllowEmpty) );
  ATH_CHECK( m_RpcCacheKey.initialize(SG::AllowEmpty) );
  ATH_CHECK( m_TgcCacheKey.initialize(SG::AllowEmpty) );
  if(m_disableWarning) m_disableWarningCheck.store(true, std::memory_order_relaxed);
  return StatusCode::SUCCESS;
}


StatusCode MuonCacheCreator::execute (const EventContext& ctx) const {

  ATH_CHECK(checkInsideViewOnce(ctx));
  // Create the MDT cache container
  int maxHashMDTs = m_idHelperSvc->mdtIdHelper().stationNameIndex("BME") != -1 ? m_idHelperSvc->mdtIdHelper().detectorElement_hash_max() : m_idHelperSvc->mdtIdHelper().module_hash_max();
  if (!m_MdtCsmCacheKey.key().empty()) {
    ATH_CHECK(createContainer<false>(m_MdtCsmCacheKey, maxHashMDTs, ctx));
  }
  // Create the CSC cache container
  if (m_idHelperSvc->hasCSC() && !m_CscCacheKey.key().empty()) {
    //"false" is there to prevent second check on the cache name
     ATH_CHECK(createContainer<false>(m_CscCacheKey,    m_idHelperSvc->cscIdHelper().module_hash_max(), ctx));
   }
  // Create the RPC cache container
  // Max should match 600 (hardcoded in RPC_RawDataProviderTool)
  if (!m_RpcCacheKey.key().empty()) {
     ATH_CHECK(createContainer<false>(m_RpcCacheKey,    m_idHelperSvc->rpcIdHelper().module_hash_max(), ctx));
   }
  // Create the TGC cache container
  if (!m_TgcCacheKey.key().empty()) {
    ATH_CHECK(createContainer<false>(m_TgcCacheKey,    m_idHelperSvc->tgcIdHelper().module_hash_max(), ctx));
  }
  return StatusCode::SUCCESS;
}
