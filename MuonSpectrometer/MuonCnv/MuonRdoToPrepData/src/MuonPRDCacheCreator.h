/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#pragma once
 
#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "GaudiKernel/ServiceHandle.h"

#include "MuonPrepRawData/MuonPrepDataCollection_Cache.h"
#include "MuonTrigCoinData/MuonTrigCoinData_Cache.h"
#include "MuonIdHelpers/IMuonIdHelperSvc.h"


// Class for setting up PRD cache containers
class MuonPRDCacheCreator : public AthReentrantAlgorithm {
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

  template<typename T>
  StatusCode createContainer(const SG::WriteHandleKey<T>& , long unsigned int size, const EventContext& ) const;
  
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

  mutable bool m_disableWarning = false;
  bool isInsideView(const EventContext&) const;

};//class MuonPRDCacheCreator

// copied from http://acode-browser1.usatlas.bnl.gov/lxr/source/athena/InnerDetector/InDetRecAlgs/InDetPrepRawDataFormation/src/CacheCreator.h#0062
// maybe should figure out if this code can be shared
template<typename T>
StatusCode MuonPRDCacheCreator::createContainer(const SG::WriteHandleKey<T>& containerKey, long unsigned int size, const EventContext& ctx) const{
  if(containerKey.key().empty()){
    ATH_MSG_DEBUG( "Creation of container (PRD) "<< containerKey.key() << " is disabled (no name specified)");
    return StatusCode::SUCCESS;
  }
  SG::WriteHandle<T> ContainerCacheKey(containerKey, ctx);
  ATH_CHECK( ContainerCacheKey.recordNonConst ( std::make_unique<T>(size, nullptr ) ));
  ATH_MSG_DEBUG( "Container (PRD) "<< containerKey.key() << " created to hold size " << size);
  return StatusCode::SUCCESS;
}
