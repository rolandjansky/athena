/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "MmRdoToPrepDataToolMT.h"

Muon::MmRdoToPrepDataToolMT::MmRdoToPrepDataToolMT(const std::string& t,
                                                   const std::string& n,
                                                   const IInterface*  p )
  :
  base_class(t,n,p)
{
  declareProperty("PrdCacheKey", m_prdContainerCacheKey, "Optional external cache for the MM PRD container");
}

StatusCode Muon::MmRdoToPrepDataToolMT::initialize()
{  
  ATH_MSG_VERBOSE("Starting init");
  ATH_CHECK( MmRdoToPrepDataToolCore::initialize() );
  ATH_CHECK( m_prdContainerCacheKey.initialize(!m_prdContainerCacheKey.key().empty()) );
  ATH_MSG_DEBUG("initialize() successful in " << name());
  return StatusCode::SUCCESS;
}

Muon::MMPrepDataContainer* Muon::MmRdoToPrepDataToolMT::setupMM_PrepDataContainer() const
{

  SG::WriteHandle< Muon::MMPrepDataContainer > handle(m_mmPrepDataContainerKey);
  const bool externalCachePRD = !m_prdContainerCacheKey.key().empty();
  if(!externalCachePRD) {
    // No external cache, just record the container
    StatusCode status = handle.record(std::make_unique<Muon::MMPrepDataContainer>(m_idHelperSvc->mmIdHelper().module_hash_max()));

    if (status.isFailure() || !handle.isValid() )   {
      ATH_MSG_FATAL("Could not record container of MicroMega PrepData Container at " << m_mmPrepDataContainerKey.key()); 
      return nullptr;
    }
  } else {
    // use the cache to get the container
    SG::UpdateHandle<MMPrepDataCollection_Cache> update(m_prdContainerCacheKey);
    if (!update.isValid()) {
      ATH_MSG_FATAL("Invalid UpdateHandle " << m_prdContainerCacheKey.key());
      return nullptr;
    }
    StatusCode status = handle.record(std::make_unique<Muon::MMPrepDataContainer>(update.ptr()));
    if (status.isFailure() || !handle.isValid()) {
      ATH_MSG_FATAL("Could not record container of MM PrepData Container using cache " << m_prdContainerCacheKey.key() << " - " << m_mmPrepDataContainerKey.key());
      return nullptr;
    }
    ATH_MSG_DEBUG("Created container using cache for " << m_prdContainerCacheKey.key());
  }
  return handle.ptr();
}
