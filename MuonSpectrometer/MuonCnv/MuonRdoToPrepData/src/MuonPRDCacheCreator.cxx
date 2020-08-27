/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonPRDCacheCreator.h"

#include "AthViews/View.h"

/// Constructor
MuonPRDCacheCreator::MuonPRDCacheCreator(const std::string &name,ISvcLocator *pSvcLocator):
  IDCCacheCreatorBase(name,pSvcLocator),
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

StatusCode MuonPRDCacheCreator::initialize() {
  ATH_CHECK( m_CscCacheKey.initialize( SG::AllowEmpty ));
  ATH_CHECK( m_CscStripCacheKey.initialize( SG::AllowEmpty ));
  ATH_CHECK( m_MdtCacheKey.initialize( SG::AllowEmpty ));
  ATH_CHECK( m_RpcCacheKey.initialize( SG::AllowEmpty ));
  ATH_CHECK( m_TgcCacheKey.initialize( SG::AllowEmpty ));
  ATH_CHECK( m_sTgcCacheKey.initialize( SG::AllowEmpty ));
  ATH_CHECK( m_MmCacheKey.initialize( SG::AllowEmpty ));
  ATH_CHECK( m_RpcCoinCacheKey.initialize( SG::AllowEmpty ));
  ATH_CHECK( m_TgcCoinCacheKey.initialize( SG::AllowEmpty ));

  ATH_CHECK(m_idHelperSvc.retrieve());

  // Check we have the tools to allow us to setup cache
  if( !m_idHelperSvc->hasCSC() && !m_CscCacheKey.key().empty() ){
    ATH_MSG_WARNING("CSC ID Helper is not available and CSC PRD cache was requested - This will not be created");
  } 
  if( !m_idHelperSvc->hasMDT() && !m_MdtCacheKey.key().empty() ){
    ATH_MSG_WARNING("MDT ID Helper is not available and MDT PRD cache was requested - This will not be created");
  }
  if( !m_idHelperSvc->hasRPC() && !m_RpcCacheKey.key().empty() ){
    ATH_MSG_WARNING("RPC ID Helper is not available and RPC PRD cache was requested - This will not be created");
  }
  if( !m_idHelperSvc->hasTGC() && !m_TgcCacheKey.key().empty() ){
    ATH_MSG_WARNING("TGC ID Helper is not available and TGC PRD cache was requested - This will not be created");
  }
  if( !m_idHelperSvc->hasSTgc() && !m_sTgcCacheKey.key().empty() ){
    ATH_MSG_WARNING("STGC ID Helper is not available and STGC PRD cache was requested - This will not be created");
  }
  if( !m_idHelperSvc->hasMM() && !m_MmCacheKey.key().empty() ){
    ATH_MSG_WARNING("MM ID Helper is not available and MM PRD cache was requested - This will not be created");
  }
  if(m_disableWarning) m_disableWarningCheck.store(true, std::memory_order_relaxed);
  return StatusCode::SUCCESS;
}

StatusCode MuonPRDCacheCreator::execute (const EventContext& ctx) const {

  ATH_CHECK(checkInsideViewOnce(ctx));

  // Create all the cache containers (if the tools are available)
  // CSC
  if( m_idHelperSvc->hasCSC() ){
    ATH_CHECK(createContainer(m_CscCacheKey, m_idHelperSvc->cscIdHelper().module_hash_max(), ctx));
    ATH_CHECK(createContainer(m_CscStripCacheKey, m_idHelperSvc->cscIdHelper().module_hash_max(), ctx));
  }

  // MDT
  if( m_idHelperSvc->hasMDT() ){
    auto maxHashMDTs = m_idHelperSvc->mdtIdHelper().stationNameIndex("BME") != -1 ? m_idHelperSvc->mdtIdHelper().detectorElement_hash_max() : m_idHelperSvc->mdtIdHelper().module_hash_max();
    ATH_CHECK(createContainer(m_MdtCacheKey, maxHashMDTs, ctx));
  }

  // RPC
  if( m_idHelperSvc->hasRPC() ){
    ATH_CHECK(createContainer(m_RpcCacheKey, m_idHelperSvc->rpcIdHelper().module_hash_max(), ctx));
    ATH_CHECK(createContainer(m_RpcCoinCacheKey, m_idHelperSvc->rpcIdHelper().module_hash_max(), ctx));
  }

  // TGC
  if( m_idHelperSvc->hasTGC() ){
    ATH_CHECK(createContainer(m_TgcCacheKey, m_idHelperSvc->tgcIdHelper().module_hash_max(), ctx));
    ATH_CHECK(createContainer(m_TgcCoinCacheKey, m_idHelperSvc->tgcIdHelper().module_hash_max(), ctx));
  }

  // NSW STGC
  if( m_idHelperSvc->hasSTgc() ){
    ATH_CHECK(createContainer(m_sTgcCacheKey, m_idHelperSvc->stgcIdHelper().module_hash_max(), ctx));
  }

  // NSW MM
  if( m_idHelperSvc->hasMM() ){
    ATH_CHECK(createContainer(m_MmCacheKey, m_idHelperSvc->mmIdHelper().module_hash_max(), ctx));
  }

  return StatusCode::SUCCESS;
}
