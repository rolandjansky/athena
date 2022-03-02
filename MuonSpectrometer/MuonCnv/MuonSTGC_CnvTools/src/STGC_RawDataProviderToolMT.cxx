/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// STGC_RawDataProviderToolMT.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#include "STGC_RawDataProviderToolMT.h"
#include "MuonRDO/STGC_RawDataContainer.h"
#include "eformat/SourceIdentifier.h"
using eformat::helper::SourceIdentifier;

//==============================================================================
Muon::STGC_RawDataProviderToolMT::STGC_RawDataProviderToolMT(const std::string& t, const std::string& n, const IInterface*  p) 
: STGC_RawDataProviderToolCore(t, n, p)
{
  declareInterface<IMuonRawDataProviderTool>(this);
  declareProperty("sTgcContainerCacheKey", m_rdoContainerCacheKey, "Optional external cache for the sTGC container");
}


//==============================================================================
StatusCode Muon::STGC_RawDataProviderToolMT::initialize()
{
  // generate all the Source Identifiers to request the fragments.
  // assume 16 RODs per side (one per sector) and that ROB ID = ROD ID.
  for (uint32_t detID : {eformat::MUON_STGC_ENDCAP_A_SIDE, eformat::MUON_STGC_ENDCAP_C_SIDE}) { //0x6D, 0x6E
    for (uint16_t sectorID(0); sectorID < 16; ++sectorID) {
        SourceIdentifier sid(static_cast<eformat::SubDetector>(detID), sectorID);
        m_allRobIds.push_back(sid.simple_code());
    }
  }

  ATH_CHECK(m_rdoContainerCacheKey.initialize(!m_rdoContainerCacheKey.key().empty()));
  ATH_CHECK(STGC_RawDataProviderToolCore::initialize());
  return StatusCode::SUCCESS;
}


//==============================================================================
StatusCode Muon::STGC_RawDataProviderToolMT::initRdoContainer(const EventContext& ctx, STGC_RawDataContainer*& rdoContainer) const
{
  // Create the identifiable RdoContainer in StoreGate to be filled with decoded fragment contents.
  SG::WriteHandle<STGC_RawDataContainer> rdoContainerHandle(m_rdoContainerKey, ctx); 

  const bool externalCacheRDO = !m_rdoContainerCacheKey.key().empty();
  if(!externalCacheRDO){
    ATH_CHECK(rdoContainerHandle.record(std::make_unique<STGC_RawDataContainer>(m_maxhashtoUse)));
    ATH_MSG_DEBUG("Created STGC RDO container");
  } else {
    SG::UpdateHandle<STGC_RawDataCollection_Cache> update(m_rdoContainerCacheKey, ctx);
    ATH_CHECK(update.isValid());
    ATH_CHECK(rdoContainerHandle.record(std::make_unique<STGC_RawDataContainer>(update.ptr())));
    ATH_MSG_DEBUG("Created STGC RDO container using cache for " << m_rdoContainerCacheKey.key());
  }

  // this should never happen, but since we dereference the pointer, we should check
  if (!(rdoContainer = rdoContainerHandle.ptr())) {
    ATH_MSG_ERROR("The STGC RDO container is null, cannot decode STGC data");
    return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;
}


//==============================================================================
StatusCode Muon::STGC_RawDataProviderToolMT::convert(const std::vector<IdentifierHash>& rdoIdhVect, const EventContext& ctx) const
{
  // method for RoI-seeded mode. we don't let empty hash containers reach the decoder, 
  // since an empty container means unseeded mode (decode everything).

  STGC_RawDataContainer* rdoContainer{nullptr};
  ATH_CHECK(initRdoContainer(ctx, rdoContainer));

  if (rdoIdhVect.empty()) return StatusCode::SUCCESS;

  std::vector<const OFFLINE_FRAGMENTS_NAMESPACE::ROBFragment*> vecRobf;
  m_robDataProvider->getROBData(m_allRobIds, vecRobf);

  return convertIntoContainer(vecRobf, rdoIdhVect, *rdoContainer);
}


//==============================================================================
StatusCode  Muon::STGC_RawDataProviderToolMT::convert(const EventContext& ctx) const
{
  // method for unseeded mode. just decode everything.

  STGC_RawDataContainer* rdoContainer{nullptr};
  ATH_CHECK(initRdoContainer(ctx, rdoContainer));
  
  std::vector<const OFFLINE_FRAGMENTS_NAMESPACE::ROBFragment*> vecRobf;
  m_robDataProvider->getROBData(m_allRobIds, vecRobf);
  
  // dummy hashID vector for the decoder (empty = unseeded mode)
  const std::vector<IdentifierHash> rdoIdhVect;

  return convertIntoContainer(vecRobf, rdoIdhVect, *rdoContainer);
}


//===============================================================================
StatusCode  Muon::STGC_RawDataProviderToolMT::convert() const
{
  ATH_MSG_ERROR("STGC_RawDataProviderToolMT::convert() Not implemented.");
  return StatusCode::FAILURE;
}

StatusCode Muon::STGC_RawDataProviderToolMT::convert(const ROBFragmentList& ) const
{    
  ATH_MSG_ERROR("STGC_RawDataProviderToolMT::convert(const ROBFragmentList& vecRobs) Not implemented.");
  return StatusCode::FAILURE;
}

StatusCode  Muon::STGC_RawDataProviderToolMT::convert(const std::vector<IdentifierHash>& ) const
{
  ATH_MSG_ERROR("STGC_RawDataProviderToolMT::convert(const std::vector<IdentifierHash>& rdoIdhVect) Not implemented.");
  return StatusCode::FAILURE;
}

StatusCode  Muon::STGC_RawDataProviderToolMT::convert(const ROBFragmentList&, const std::vector<IdentifierHash>&) const
{
  ATH_MSG_ERROR("STGC_RawDataProviderToolMT::convert(const ROBFragmentList& vecRobs, const std::vector<IdentifierHash>&) Not implemented.");
  return StatusCode::FAILURE;
}
