/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonCacheCreator.h"

#include "AthViews/View.h"

/// Constructor
MuonCacheCreator::MuonCacheCreator(const std::string &name,ISvcLocator *pSvcLocator):
  AthReentrantAlgorithm(name,pSvcLocator),
  m_MdtCsmCacheKey(""),
  m_CscCacheKey(""),
  m_RpcCacheKey(""),
  m_TgcCacheKey(""),
  m_disableWarningCheck(false)
{
  declareProperty("MdtCsmCacheKey", m_MdtCsmCacheKey);
  declareProperty("CscCacheKey",    m_CscCacheKey);
  declareProperty("RpcCacheKey",    m_RpcCacheKey);
  declareProperty("TgcCacheKey",    m_TgcCacheKey);
}

StatusCode MuonCacheCreator::initialize() {
  ATH_CHECK( m_MdtCsmCacheKey.initialize(!m_MdtCsmCacheKey.key().empty()) );
  ATH_CHECK( m_CscCacheKey.initialize(!m_CscCacheKey.key().empty()) );
  ATH_CHECK( m_RpcCacheKey.initialize(!m_RpcCacheKey.key().empty()) );
  ATH_CHECK( m_TgcCacheKey.initialize(!m_TgcCacheKey.key().empty()) );
  ATH_CHECK( m_idHelperSvc.retrieve() );
  return StatusCode::SUCCESS;
}

bool MuonCacheCreator::isInsideView(const EventContext& context) const
{
   const IProxyDict* proxy = Atlas::getExtendedEventContext(context).proxy();
   const SG::View* view = dynamic_cast<const SG::View*>(proxy);
   return view != nullptr;
}

StatusCode MuonCacheCreator::execute (const EventContext& ctx) const {

  if(!m_disableWarningCheck and !m_disableWarning.value()){
     if(isInsideView(ctx)){
        ATH_MSG_ERROR("CacheCreator is running inside a view, this is probably a misconfiguration");
        return StatusCode::FAILURE;
     }
     m_disableWarningCheck = true; //only check once
  }
  // Create the MDT cache container
  int maxHashMDTs = m_idHelperSvc->mdtIdHelper().stationNameIndex("BME") != -1 ? m_idHelperSvc->mdtIdHelper().detectorElement_hash_max() : m_idHelperSvc->mdtIdHelper().module_hash_max();
  if (!m_MdtCsmCacheKey.key().empty()) {
    ATH_CHECK(createContainer(m_MdtCsmCacheKey, maxHashMDTs, ctx));
    ATH_MSG_DEBUG("Created cache container " << m_MdtCsmCacheKey);
  }
  // Create the CSC cache container
  if (!m_CscCacheKey.key().empty()) {
     ATH_CHECK(createContainer(m_CscCacheKey,    m_idHelperSvc->cscIdHelper().module_hash_max(), ctx));
     ATH_MSG_DEBUG("Created cache container " << m_CscCacheKey);
   }
  // Create the RPC cache container
  // Max should match 600 (hardcoded in RPC_RawDataProviderTool)
  if (!m_RpcCacheKey.key().empty()) {
     ATH_CHECK(createContainer(m_RpcCacheKey,    m_idHelperSvc->rpcIdHelper().module_hash_max(), ctx));
     ATH_MSG_DEBUG("Created cache container " << m_RpcCacheKey);
   }
  // Create the TGC cache container
  if (!m_TgcCacheKey.key().empty()) {
    ATH_CHECK(createContainer(m_TgcCacheKey,    m_idHelperSvc->tgcIdHelper().module_hash_max(), ctx));
    ATH_MSG_DEBUG("Created cache container " << m_TgcCacheKey);
  }
  return StatusCode::SUCCESS;
}
