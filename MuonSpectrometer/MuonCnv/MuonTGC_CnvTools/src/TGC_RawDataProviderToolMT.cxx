/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "TGC_RawDataProviderToolMT.h"

#include "MuonRDO/TgcRdoContainer.h"

//================ Constructor =================================================

Muon::TGC_RawDataProviderToolMT::TGC_RawDataProviderToolMT(
						       const std::string& t,
						       const std::string& n,
						       const IInterface*  p) :
  TGC_RawDataProviderToolCore(t, n, p)
{
  declareInterface<IMuonRawDataProviderTool>(this);
  declareProperty("TgcContainerCacheKey", m_rdoContainerCacheKey, "Optional external cache for the TGC container");
}

//================ Initialisation =================================================

StatusCode Muon::TGC_RawDataProviderToolMT::initialize()
{
  // call base class initialise
  ATH_CHECK( TGC_RawDataProviderToolCore::initialize() );

  // Initialise the container cache if available
  ATH_CHECK( m_rdoContainerCacheKey.initialize( !m_rdoContainerCacheKey.key().empty() ) );
  
  ATH_MSG_INFO( "initialize() successful in " << name() );
  return StatusCode::SUCCESS;
}

//============================================================================================

StatusCode Muon::TGC_RawDataProviderToolMT::convert(const ROBFragmentList& vecRobs) 
{    

  SG::WriteHandle<TgcRdoContainer> rdoContainerHandle(m_rdoContainerKey); 

  // Split the methods to have one where we use the cache and one where we just setup the container
  const bool externalCacheRDO = !m_rdoContainerCacheKey.key().empty();
  if(!externalCacheRDO){
    ATH_CHECK( rdoContainerHandle.record( std::make_unique<TgcRdoContainer> (m_maxhashtoUse) ) );
    ATH_MSG_DEBUG( "Created TGC container" );
  }
  else{
    SG::UpdateHandle<TgcRdo_Cache> update(m_rdoContainerCacheKey);
    ATH_CHECK(update.isValid());
    ATH_CHECK(rdoContainerHandle.record (std::make_unique<TgcRdoContainer>( update.ptr() )));
    ATH_MSG_DEBUG("Created container using cache for " << m_rdoContainerCacheKey.key());
  }

  TgcRdoContainer* rdoContainer = rdoContainerHandle.ptr();

  // this should never happen, but since we dereference the pointer, we should check
  if(!rdoContainer) {
    ATH_MSG_ERROR("TGC RDO Container is null, cannot decode TGC data");
    return StatusCode::FAILURE;
  }

  return convertIntoContainer(vecRobs, *rdoContainer);
}

StatusCode  Muon::TGC_RawDataProviderToolMT::convert(const ROBFragmentList& vecRobs,
						   const std::vector<IdentifierHash>&) 
{
  return convert(vecRobs);
}

StatusCode  Muon::TGC_RawDataProviderToolMT::convert()
{
  if(!m_cabling) {
    StatusCode sc = getCabling();
    if(sc.isFailure()) return sc;
  }

  std::vector<const OFFLINE_FRAGMENTS_NAMESPACE::ROBFragment*> vecOfRobf;
  const std::vector<uint32_t>& robIds = m_hid2re.allRobIds();

  m_robDataProvider->getROBData(robIds, vecOfRobf);

  return convert(vecOfRobf); 
}

StatusCode  Muon::TGC_RawDataProviderToolMT::convert(const std::vector<IdentifierHash>& rdoIdhVect)
{
  std::vector<const OFFLINE_FRAGMENTS_NAMESPACE::ROBFragment*> vecOfRobf = getROBData(rdoIdhVect);

  return convert(vecOfRobf, rdoIdhVect);
}

