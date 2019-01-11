/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonCacheCreator.h"

#include "MuonIdHelpers/MdtIdHelper.h"
#include "MuonIdHelpers/CscIdHelper.h"
#include "MuonIdHelpers/RpcIdHelper.h"
#include "MuonIdHelpers/TgcIdHelper.h"
#include "AthViews/View.h"

/// Constructor
MuonCacheCreator::MuonCacheCreator(const std::string &name,ISvcLocator *pSvcLocator):
  AthReentrantAlgorithm(name,pSvcLocator),
  m_MdtCsmCacheKey(""),
  m_CscCacheKey(""),
  m_RpcCacheKey(""),
  m_TgcCacheKey("")
{
  declareProperty("MdtCsmCacheKey", m_MdtCsmCacheKey);
  declareProperty("CscCacheKey",    m_CscCacheKey);
  declareProperty("RpcCacheKey",    m_RpcCacheKey);
  declareProperty("TgcCacheKey",    m_TgcCacheKey);
  declareProperty("DisableViewWarning", m_disableWarning);
}

MuonCacheCreator::~MuonCacheCreator() {

}

StatusCode MuonCacheCreator::initialize() {
  ATH_CHECK( m_MdtCsmCacheKey.initialize(!m_MdtCsmCacheKey.key().empty()) );
  ATH_CHECK( m_CscCacheKey.initialize(!m_CscCacheKey.key().empty()) );
  ATH_CHECK( m_RpcCacheKey.initialize(!m_RpcCacheKey.key().empty()) );
  ATH_CHECK( m_TgcCacheKey.initialize(!m_TgcCacheKey.key().empty()) );
  ATH_CHECK( detStore()->retrieve(m_mdtIdHelper,"MDTIDHELPER") );
  ATH_CHECK( detStore()->retrieve(m_cscIdHelper,"CSCIDHELPER") );
  ATH_CHECK( detStore()->retrieve(m_rpcIdHelper,"RPCIDHELPER") );
  ATH_CHECK( detStore()->retrieve(m_tgcIdHelper,"TGCIDHELPER") );
  return StatusCode::SUCCESS;
}

bool MuonCacheCreator::isInsideView(const EventContext& context) const
{
   const IProxyDict* proxy = context.getExtension<Atlas::ExtendedEventContext>().proxy();
   const SG::View* view = dynamic_cast<const SG::View*>(proxy);
   return view != nullptr;
}

StatusCode MuonCacheCreator::execute (const EventContext& ctx) const {

  if(!m_disableWarning && isInsideView(ctx)){
     ATH_MSG_WARNING("CacheCreator is running inside a view, this is probably a misconfiguration");
  }
  // Create the MDT cache container
  auto maxHashMDTs = m_mdtIdHelper->stationNameIndex("BME") != -1 ? m_mdtIdHelper->detectorElement_hash_max() : m_mdtIdHelper->module_hash_max();
  ATH_CHECK(CreateContainer(m_MdtCsmCacheKey, maxHashMDTs, ctx));
  // Create the CSC cache container
  ATH_CHECK(CreateContainer(m_CscCacheKey,    m_cscIdHelper->module_hash_max(), ctx));
  // Create the RPC cache container
  ATH_CHECK(CreateContainer(m_RpcCacheKey,    m_rpcIdHelper->module_hash_max(), ctx));
  // Create the TGC cache container
  ATH_CHECK(CreateContainer(m_TgcCacheKey,    m_tgcIdHelper->module_hash_max(), ctx));

  ATH_MSG_INFO("Created cache container " << m_MdtCsmCacheKey);
  ATH_MSG_INFO("Created cache container " << m_CscCacheKey);
  ATH_MSG_INFO("Created cache container " << m_RpcCacheKey);
  ATH_MSG_INFO("Created cache container " << m_TgcCacheKey);


  return StatusCode::SUCCESS;
}
