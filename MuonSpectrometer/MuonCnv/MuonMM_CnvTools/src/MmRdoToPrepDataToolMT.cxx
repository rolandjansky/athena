/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// MMRdoToPrepDataTool.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#include "MmRdoToPrepDataToolMT.h"
#include "MuonPrepRawData/MMPrepDataContainer.h"

Muon::MmRdoToPrepDataToolMT::MmRdoToPrepDataToolMT(const std::string& t,
					       const std::string& n,
					       const IInterface*  p )
  :
  AthAlgTool(t,n,p),
  MmRdoToPrepDataToolCore(t,n,p)
{
  declareProperty("MmPrdContainterCacheKey", m_prdContainerCacheKey, "Optional external cache for the MM PRD container");
}


Muon::MmRdoToPrepDataToolMT::~MmRdoToPrepDataToolMT()
{
}

StatusCode Muon::MmRdoToPrepDataToolMT::initialize()
{  
  ATH_MSG_VERBOSE("Starting init");
  ATH_CHECK( MmRdoToPrepDataToolCore::initialize() );
  ATH_CHECK( m_prdContainerCacheKey.initialize( !m_prdContainerCacheKey.key().empty() ) );
  ATH_MSG_DEBUG("initialize() successful in " << name());
  return StatusCode::SUCCESS;
}

StatusCode Muon::MmRdoToPrepDataToolMT::finalize()
{
  return MmRdoToPrepDataToolCore::finalize();
}

Muon::MmRdoToPrepDataToolCore::SetupMM_PrepDataContainerStatus Muon::MmRdoToPrepDataToolMT::setupMM_PrepDataContainer() 
{
  // MT version of this method always adds container. Caching will be added later.
  SG::WriteHandle< Muon::MMPrepDataContainer > handle(m_mmPrepDataContainerKey);

  // Caching of PRD container
  const bool externalCachePRD = !m_prdContainerCacheKey.key().empty();
  if (!externalCachePRD) {
    // without the cache we just record the container
    StatusCode status = handle.record(std::make_unique<Muon::MMPrepDataContainer>(m_muonIdHelperTool->mmIdHelper().module_hash_max()));
    if (status.isFailure() || !handle.isValid() )   {
      ATH_MSG_FATAL("Could not record container of MicroMega PrepData Container at " << m_mmPrepDataContainerKey.key()); 
      return FAILED;
    }
    ATH_MSG_DEBUG("Created container " << m_mmPrepDataContainerKey.key());
  } 
  else {
    // use the cache to get the container
    SG::UpdateHandle<MMPrepDataCollection_Cache> update(m_prdContainerCacheKey);
    if (!update.isValid()){
      ATH_MSG_FATAL("Invalid UpdateHandle " << m_prdContainerCacheKey.key());
      return FAILED;
    }
    StatusCode status = handle.record(std::make_unique<Muon::MMPrepDataContainer>(update.ptr()));
    if (status.isFailure() || !handle.isValid() )   {
      ATH_MSG_FATAL("Could not record container of MicroMega PrepData Container using cache " 
        << m_prdContainerCacheKey.key() << " - " <<m_mmPrepDataContainerKey.key()); 
      return FAILED;
    }
    ATH_MSG_DEBUG("Created container using cache for " << m_prdContainerCacheKey.key());
  }
  // Pass the container from the handle
  m_mmPrepDataContainer = handle.ptr();

  return ADDED;
}
