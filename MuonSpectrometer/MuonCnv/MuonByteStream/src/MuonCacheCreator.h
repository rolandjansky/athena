/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#pragma once
 
#include "ViewAlgs/IDCCacheCreatorBase.h"
#include "GaudiKernel/ServiceHandle.h"
#include "MuonRDO/MdtCsm_Cache.h"
#include "MuonRDO/CscRawDataCollection_Cache.h"
#include "MuonRDO/RpcPad_Cache.h"
#include "MuonRDO/TgcRdo_Cache.h"
#include "MuonIdHelpers/IMuonIdHelperSvc.h"

class MuonCacheCreator : public IDCCacheCreatorBase {
 public:

  /// Constructor
  MuonCacheCreator(const std::string &name,ISvcLocator *pSvcLocator);
  /// Destructor
  virtual ~MuonCacheCreator()=default;

  /// Initialize the algorithm
  virtual StatusCode initialize () override;

  /// Execture the algorithm
  virtual StatusCode execute (const EventContext& ctx) const override;

protected:

  /// Write handle key for the MDT CSM cache container
  SG::WriteHandleKey<MdtCsm_Cache> m_MdtCsmCacheKey;
  SG::WriteHandleKey<CscRawDataCollection_Cache> m_CscCacheKey;
  SG::WriteHandleKey<RpcPad_Cache> m_RpcCacheKey;
  SG::WriteHandleKey<TgcRdo_Cache> m_TgcCacheKey;
  ServiceHandle<Muon::IMuonIdHelperSvc> m_idHelperSvc {this, "MuonIdHelperSvc", "Muon::MuonIdHelperSvc/MuonIdHelperSvc"};
  BooleanProperty m_disableWarning{this,"DisableViewWarning",false};

};//class MuonCacheCreator


