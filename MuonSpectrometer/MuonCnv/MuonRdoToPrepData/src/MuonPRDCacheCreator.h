/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#pragma once
 
#include "ViewAlgs/IDCCacheCreatorBase.h"
#include "GaudiKernel/ServiceHandle.h"

#include "MuonPrepRawData/MuonPrepDataCollection_Cache.h"
#include "MuonTrigCoinData/MuonTrigCoinData_Cache.h"
#include "MuonIdHelpers/IMuonIdHelperSvc.h"


// Class for setting up PRD cache containers
class MuonPRDCacheCreator : public IDCCacheCreatorBase {
 public:

  /// Constructor
  MuonPRDCacheCreator(const std::string &name,ISvcLocator *pSvcLocator);
  /// Destructor
  virtual ~MuonPRDCacheCreator()=default;

  /// Initialize the algorithm
  virtual StatusCode initialize () override;

  /// Execture the algorithm
  virtual StatusCode execute (const EventContext& ctx) const override;

protected:
  
  /// Write handle keys for the PRD caches
  SG::WriteHandleKey<CscPrepDataCollection_Cache>       m_CscCacheKey;
  SG::WriteHandleKey<CscStripPrepDataCollection_Cache>  m_CscStripCacheKey;
  SG::WriteHandleKey<MdtPrepDataCollection_Cache>       m_MdtCacheKey;
  SG::WriteHandleKey<RpcPrepDataCollection_Cache>       m_RpcCacheKey;
  SG::WriteHandleKey<TgcPrepDataCollection_Cache>       m_TgcCacheKey;
  SG::WriteHandleKey<sTgcPrepDataCollection_Cache>      m_sTgcCacheKey;
  SG::WriteHandleKey<MMPrepDataCollection_Cache>        m_MmCacheKey;
  SG::WriteHandleKey<RpcCoinDataCollection_Cache>       m_RpcCoinCacheKey;
  SG::WriteHandleKey<TgcCoinDataCollection_Cache>       m_TgcCoinCacheKey;

  ServiceHandle<Muon::IMuonIdHelperSvc> m_idHelperSvc {this, "MuonIdHelperSvc", "Muon::MuonIdHelperSvc/MuonIdHelperSvc"};

  bool m_disableWarning = false;

};//class MuonPRDCacheCreator

