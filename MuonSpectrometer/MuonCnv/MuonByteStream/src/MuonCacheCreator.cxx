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
  ATH_CHECK( m_muonIdHelperTool.retrieve() );
  return StatusCode::SUCCESS;
}

bool MuonCacheCreator::isInsideView(const EventContext& context) const
{
   const IProxyDict* proxy = context.getExtension<Atlas::ExtendedEventContext>().proxy();
   const SG::View* view = dynamic_cast<const SG::View*>(proxy);
   return view != nullptr;
}

StatusCode MuonCacheCreator::execute (const EventContext& ctx) const {

  if(!m_disableWarning){
     if(isInsideView(ctx)){
        ATH_MSG_ERROR("CacheCreator is running inside a view, this is probably a misconfiguration");
        return StatusCode::FAILURE;
     }
     m_disableWarning = true; //only check once
  }
  // Create the MDT cache container
  auto maxHashMDTs = m_muonIdHelperTool->mdtIdHelper().stationNameIndex("BME") != -1 ? m_muonIdHelperTool->mdtIdHelper().detectorElement_hash_max() : m_muonIdHelperTool->mdtIdHelper().module_hash_max();
  ATH_CHECK(createContainer(m_MdtCsmCacheKey, maxHashMDTs, ctx));
  // Create the CSC cache container
  ATH_CHECK(createContainer(m_CscCacheKey,    m_muonIdHelperTool->cscIdHelper().module_hash_max(), ctx));
  // Create the RPC cache container
  // Max should match 600 (hardcoded in RPC_RawDataProviderTool)
  ATH_CHECK(createContainer(m_RpcCacheKey,    m_muonIdHelperTool->rpcIdHelper().module_hash_max(), ctx));
  // Create the TGC cache container
  ATH_CHECK(createContainer(m_TgcCacheKey,    m_muonIdHelperTool->tgcIdHelper().module_hash_max(), ctx));

  ATH_MSG_DEBUG("Created cache container " << m_MdtCsmCacheKey);
  ATH_MSG_DEBUG("Created cache container " << m_CscCacheKey);
  ATH_MSG_DEBUG("Created cache container " << m_RpcCacheKey);
  ATH_MSG_DEBUG("Created cache container " << m_TgcCacheKey);


  return StatusCode::SUCCESS;
}
