/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonPRDCacheCreator.h"

#include "MuonIdHelpers/MdtIdHelper.h"
#include "MuonIdHelpers/CscIdHelper.h"
#include "MuonIdHelpers/RpcIdHelper.h"
#include "MuonIdHelpers/TgcIdHelper.h"
#include "MuonIdHelpers/sTgcIdHelper.h"
#include "MuonIdHelpers/MmIdHelper.h"

#include "AthViews/View.h"

/// Constructor
MuonPRDCacheCreator::MuonPRDCacheCreator(const std::string &name,ISvcLocator *pSvcLocator):
  AthReentrantAlgorithm(name,pSvcLocator),
   m_CscCacheKey(""),
   m_CscStripCacheKey(""),
   m_MdtCacheKey(""),
   m_RpcCacheKey(""),
   m_TgcCacheKey(""),
   m_sTgcCacheKey(""),
   m_MmCacheKey(""),
   m_RpcCoinCacheKey(""),
   m_TgcCoinCacheKey("")
{
  declareProperty("CscCacheKey",        m_CscCacheKey);
  declareProperty("CscStripCacheKey",   m_CscStripCacheKey);
  declareProperty("MdtCacheKey",        m_MdtCacheKey);
  declareProperty("RpcCacheKey",        m_RpcCacheKey);
  declareProperty("TgcCacheKey",        m_TgcCacheKey);
  declareProperty("sTgcCacheKey",       m_sTgcCacheKey);
  declareProperty("MmCacheKey",         m_MmCacheKey);
  declareProperty("RpcCoinCacheKey",    m_RpcCoinCacheKey);
  declareProperty("TgcCoinCacheKey",    m_TgcCoinCacheKey);
  declareProperty("DisableViewWarning", m_disableWarning);
}

MuonPRDCacheCreator::~MuonPRDCacheCreator() {

}

StatusCode MuonPRDCacheCreator::initialize() {
  ATH_CHECK( m_CscCacheKey.initialize( !m_CscCacheKey.key().empty() ));
  ATH_CHECK( m_CscStripCacheKey.initialize( !m_CscStripCacheKey.key().empty() ));
  ATH_CHECK( m_MdtCacheKey.initialize( !m_MdtCacheKey.key().empty() ));
  ATH_CHECK( m_RpcCacheKey.initialize( !m_RpcCacheKey.key().empty() ));
  ATH_CHECK( m_TgcCacheKey.initialize( !m_TgcCacheKey.key().empty() ));
  ATH_CHECK( m_sTgcCacheKey.initialize( !m_sTgcCacheKey.key().empty() ));
  ATH_CHECK( m_MmCacheKey.initialize( !m_MmCacheKey.key().empty() ));
  ATH_CHECK( m_RpcCoinCacheKey.initialize( !m_RpcCoinCacheKey.key().empty() ));
  ATH_CHECK( m_TgcCoinCacheKey.initialize( !m_TgcCoinCacheKey.key().empty() ));

  // Retrieve ID tools
  ATH_CHECK( m_muonIdHelperTool.retrieve() );

  return StatusCode::SUCCESS;
}

bool MuonPRDCacheCreator::isInsideView(const EventContext& context) const
{
   const IProxyDict* proxy = context.getExtension<Atlas::ExtendedEventContext>().proxy();
   const SG::View* view = dynamic_cast<const SG::View*>(proxy);
   return view != nullptr;
}

StatusCode MuonPRDCacheCreator::execute (const EventContext& ctx) const {

  if(!m_disableWarning){
     if(isInsideView(ctx)){
        ATH_MSG_ERROR("CacheCreator is running inside a view, this is probably a misconfiguration");
        return StatusCode::FAILURE;
     }
     m_disableWarning = true; //only check once
  }

  // Create all the cache containers (if the tools are available)
  // CSC
  if( m_muonIdHelperTool->hasCscIdHelper() ){
    ATH_CHECK(createContainer(m_CscCacheKey, m_muonIdHelperTool->cscIdHelper().module_hash_max(), ctx));
    ATH_CHECK(createContainer(m_CscStripCacheKey, m_muonIdHelperTool->cscIdHelper().module_hash_max(), ctx));
  }
  if( !m_muonIdHelperTool->hasCscIdHelper() && !m_CscCacheKey.key().empty() ){
    ATH_MSG_WARNING("CSC ID Helper is not available and CSC PRD cache was requested. This will not be created");
  }

  // MDT
  if( m_muonIdHelperTool->hasMdtIdHelper() ){
    auto maxHashMDTs = m_muonIdHelperTool->mdtIdHelper().stationNameIndex("BME") != -1 ? m_muonIdHelperTool->mdtIdHelper().detectorElement_hash_max() : m_muonIdHelperTool->mdtIdHelper().module_hash_max();
    ATH_CHECK(createContainer(m_MdtCacheKey, maxHashMDTs, ctx));
  }
  if( !m_muonIdHelperTool->hasMdtIdHelper() && !m_MdtCacheKey.key().empty() ){
    ATH_MSG_WARNING("MDT ID Helper is not available and MDT PRD cache was requested. This will not be created");
  }

  // RPC
  if( m_muonIdHelperTool->hasRpcIdHelper() ){
    ATH_CHECK(createContainer(m_RpcCacheKey, m_muonIdHelperTool->rpcIdHelper().module_hash_max(), ctx));
    ATH_CHECK(createContainer(m_RpcCoinCacheKey, m_muonIdHelperTool->rpcIdHelper().module_hash_max(), ctx));
  }
  if( !m_muonIdHelperTool->hasRpcIdHelper() && !m_RpcCacheKey.key().empty() ){
    ATH_MSG_WARNING("RPC ID Helper is not available and RPC PRD cache was requested. This will not be created");
  }

  // TGC
  if( m_muonIdHelperTool->hasTgcIdHelper() ){
    ATH_CHECK(createContainer(m_TgcCacheKey, m_muonIdHelperTool->tgcIdHelper().module_hash_max(), ctx));
    ATH_CHECK(createContainer(m_TgcCoinCacheKey, m_muonIdHelperTool->tgcIdHelper().module_hash_max(), ctx));
  }
  if( !m_muonIdHelperTool->hasTgcIdHelper() && !m_TgcCacheKey.key().empty() ){
    ATH_MSG_WARNING("TGC ID Helper is not available and TGC PRD cache was requested. This will not be created");
  }

  // NSW STGC
  if( m_muonIdHelperTool->hasSTgcIdHelper() ){
    ATH_CHECK(createContainer(m_sTgcCacheKey, m_muonIdHelperTool->stgcIdHelper().module_hash_max(), ctx));
  }
  if( !m_muonIdHelperTool->hasSTgcIdHelper() && !m_sTgcCacheKey.key().empty() ){
    ATH_MSG_WARNING("STGC ID Helper is not available and STGC PRD cache was requested. This will not be created");
  }

  // NSW MM
  if( m_muonIdHelperTool->hasMmIdHelper() ){
    ATH_CHECK(createContainer(m_MmCacheKey, m_muonIdHelperTool->mmIdHelper().module_hash_max(), ctx));
  }
  if( !m_muonIdHelperTool->hasMmIdHelper() && !m_MmCacheKey.key().empty() ){
    ATH_MSG_WARNING("MM ID Helper is not available and MM PRD cache was requested. This will not be created");
  }

  return StatusCode::SUCCESS;
}
