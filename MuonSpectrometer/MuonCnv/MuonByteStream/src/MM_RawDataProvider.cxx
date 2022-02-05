/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonByteStream/MM_RawDataProvider.h"

//===============================================================================================
Muon::MM_RawDataProvider::MM_RawDataProvider(const std::string& name,ISvcLocator* pSvcLocator)
: AthReentrantAlgorithm(name, pSvcLocator)
{ }


//===============================================================================================
StatusCode Muon::MM_RawDataProvider::initialize() 
{

  ATH_MSG_INFO( "MM_RawDataProvider::initialize" );
  ATH_MSG_INFO( m_seededDecoding );

  ATH_CHECK( m_rawDataTool.retrieve() );
  ATH_CHECK( m_roiCollectionKey.initialize(m_seededDecoding) ); // mark the RoI-collection flag as used or not used

  if (m_seededDecoding) {
    if (m_regsel_mm.retrieve().isFailure()) { // in RoI - seeded mode, retrieve the region selector
      ATH_MSG_FATAL("Unable to retrieve RegionSelector Tool");
      return StatusCode::FAILURE;
    }
  } else {
    m_regsel_mm.disable();
  }

  return StatusCode::SUCCESS;
}

//===============================================================================================
StatusCode Muon::MM_RawDataProvider::execute(const EventContext& ctx) const 
{
  // The hash IDs corresponding to each ROI (in seeded mode) have module-level granularity, not sector level.
  // Therefore, we pass the list of hash IDs to the decoder to make the selection based on decoded elink info.

  ATH_MSG_VERBOSE( "MM_RawDataProvider::execute" );
  
  if(m_seededDecoding) {
    
    ATH_MSG_DEBUG("converting MM BS into RDOs in ROI-seeded mode");
    
    // read the RoIs to process
    SG::ReadHandle<TrigRoiDescriptorCollection> muonRoI(m_roiCollectionKey, ctx);
    if(!muonRoI.isValid()) {
      ATH_MSG_WARNING("Cannot retrieve muonRoI "<<m_roiCollectionKey.key());
      return StatusCode::FAILURE;
    }
   
    // get list of offline hash IDs (module context) from region selection,
    // using an unordered_set to make sure that hash ID entries are unique
    std::vector<IdentifierHash>        mm_hash_ids;
    std::unordered_set<IdentifierHash> mm_hash_ids_set;
    for(auto roi : *muonRoI) {
      ATH_MSG_DEBUG("Getting ROBs for RoI " << *roi);
      m_regsel_mm->HashIDList(*roi, mm_hash_ids);
      for (IdentifierHash& hash : mm_hash_ids) mm_hash_ids_set.insert(hash);
      mm_hash_ids.clear();
    }

    // put the IDs back in the vector and decode
    mm_hash_ids.insert(mm_hash_ids.end(), mm_hash_ids_set.begin(), mm_hash_ids_set.end());    
    if (!m_rawDataTool->convert(mm_hash_ids, ctx).isSuccess()) {
      ATH_MSG_ERROR("MM BS conversion into RDOs failed");
      return StatusCode::FAILURE;
    }

  } else {
    ATH_MSG_DEBUG("converting MM BS into RDOs in unseeded mode");
    if (!m_rawDataTool->convert(ctx).isSuccess()) {
      ATH_MSG_ERROR("MM BS conversion into RDOs failed");
      return StatusCode::FAILURE;
    }
  }

  return StatusCode::SUCCESS;
}
