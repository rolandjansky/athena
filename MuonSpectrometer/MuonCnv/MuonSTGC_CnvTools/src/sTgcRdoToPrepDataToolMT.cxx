/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
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
  AthAlgTool(t,n,p),
  sTgcRdoToPrepDataToolCore(t,n,p)
{
}

Muon::sTgcRdoToPrepDataToolMT::~sTgcRdoToPrepDataToolMT()
{
}

StatusCode Muon::sTgcRdoToPrepDataToolMT::initialize()
{  
  ATH_MSG_VERBOSE("Starting init");
  ATH_CHECK( sTgcRdoToPrepDataToolCore::initialize() );
  ATH_MSG_DEBUG("initialize() successful in " << name());
  return StatusCode::SUCCESS;
}

StatusCode Muon::sTgcRdoToPrepDataToolMT::finalize()
{
  return sTgcRdoToPrepDataToolCore::finalize();
}

Muon::sTgcRdoToPrepDataToolCore::SetupSTGC_PrepDataContainerStatus Muon::sTgcRdoToPrepDataToolMT::setupSTGC_PrepDataContainer() 
{
  m_fullEventDone = false;

  SG::WriteHandle< Muon::sTgcPrepDataContainer > handle(m_stgcPrepDataContainerKey);
  
  // Caching of PRD container
  const bool externalCachePRD = !m_prdContainerCacheKey.key().empty();
  if (!externalCachePRD) {
    // without the cache we just record the container
    StatusCode status = handle.record(std::make_unique<Muon::sTgcPrepDataContainer>(m_muonIdHelperTool->stgcIdHelper().module_hash_max()));
    if (status.isFailure() || !handle.isValid() )   {
      ATH_MSG_FATAL("Could not record container of sTGC PrepData Container at " << m_stgcPrepDataContainerKey.key()); 
      return FAILED;
    }
    ATH_MSG_DEBUG("Created container " << m_stgcPrepDataContainerKey.key());
  } 
  else {
    // use the cache to get the container
    SG::UpdateHandle<sTgcPrepDataCollection_Cache> update(m_prdContainerCacheKey);
    if (!update.isValid()){
      ATH_MSG_FATAL("Invalid UpdateHandle " << m_prdContainerCacheKey.key());
      return FAILED;
    }
    StatusCode status = handle.record(std::make_unique<Muon::sTgcPrepDataContainer>(update.ptr()));
    if (status.isFailure() || !handle.isValid() )   {
      ATH_MSG_FATAL("Could not record container of sTGC PrepData Container using cache " 
        << m_prdContainerCacheKey.key() << " - " <<m_stgcPrepDataContainerKey.key()); 
      return FAILED;
    }
    ATH_MSG_DEBUG("Created container using cache for " << m_stgcPrepDataContainerKey.key());
  }
  // Pass the container from the handle
  m_stgcPrepDataContainer = handle.ptr();

  return ADDED;
    
}
