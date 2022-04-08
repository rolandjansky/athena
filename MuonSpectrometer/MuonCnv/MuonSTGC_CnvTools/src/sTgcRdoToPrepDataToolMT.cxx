/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// sTgcRdoToPrepDataToolMT.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#include "sTgcRdoToPrepDataToolMT.h"
#include "MuonReadoutGeometry/sTgcReadoutElement.h"


Muon::sTgcRdoToPrepDataToolMT::sTgcRdoToPrepDataToolMT(const std::string& t,
						   const std::string& n,
						   const IInterface*  p )
  :
  base_class(t,n,p)
{
  declareProperty("PrdCacheKey", m_prdContainerCacheKey, "Optional external cache for the sTGC PRD container");
}

StatusCode Muon::sTgcRdoToPrepDataToolMT::initialize()
{  
  ATH_MSG_VERBOSE("Starting init");
  ATH_CHECK( sTgcRdoToPrepDataToolCore::initialize() );
  ATH_CHECK( m_prdContainerCacheKey.initialize(!m_prdContainerCacheKey.key().empty()) );
  ATH_MSG_DEBUG("initialize() successful in " << name());
  return StatusCode::SUCCESS;
}

Muon::sTgcPrepDataContainer* Muon::sTgcRdoToPrepDataToolMT::setupSTGC_PrepDataContainer() const
{

  SG::WriteHandle< Muon::sTgcPrepDataContainer > handle(m_stgcPrepDataContainerKey);
  const bool externalCachePRD = !m_prdContainerCacheKey.key().empty();
  if(!externalCachePRD) {
    // No external cache, just record the container
    StatusCode status = handle.record(std::make_unique<Muon::sTgcPrepDataContainer>(m_idHelperSvc->stgcIdHelper().module_hash_max()));
    
    if (status.isFailure() || !handle.isValid() )   {
      ATH_MSG_FATAL("Could not record container of STGC PrepData Container at " << m_stgcPrepDataContainerKey.key()); 
      return nullptr;
    }
  } else {
    //use the cache to get the container
    SG::UpdateHandle<sTgcPrepDataCollection_Cache> update(m_prdContainerCacheKey);
    if (!update.isValid()) {
      ATH_MSG_FATAL("Invalid UpdateHandle " << m_prdContainerCacheKey.key());
      return nullptr;
    }
    StatusCode status = handle.record(std::make_unique<Muon::sTgcPrepDataContainer>(update.ptr()));
    if (status.isFailure() || !handle.isValid()) {
      ATH_MSG_FATAL("Could not record container of sTGC PrepData Container using cache " << m_prdContainerCacheKey.key() << " - " << m_stgcPrepDataContainerKey.key());
      return nullptr;
    }
    ATH_MSG_DEBUG("Created container using cache for " << m_prdContainerCacheKey.key());
  }
  
  return handle.ptr();
}
