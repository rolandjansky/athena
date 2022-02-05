/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonByteStream/sTgcRawDataProvider.h"

//=====================================================================
Muon::sTgcRawDataProvider::sTgcRawDataProvider(const std::string& name, ISvcLocator* pSvcLocator) 
: AthReentrantAlgorithm(name, pSvcLocator)
{ }


//=====================================================================
StatusCode Muon::sTgcRawDataProvider::initialize()
{
  ATH_MSG_INFO("sTgcRawDataProvider::initialize");
  ATH_MSG_INFO(m_seededDecoding);

  ATH_CHECK(m_rawDataTool.retrieve());
  ATH_CHECK(m_roiCollectionKey.initialize(m_seededDecoding)); // mark the RoI-collection flag as used or not used

  if(m_seededDecoding) {
    if (m_regsel_stgc.retrieve().isFailure()) { // in RoI - seeded mode, retrieve the region selector
      ATH_MSG_FATAL("Unable to retrieve RegionSelector Tool");
      return StatusCode::FAILURE;
    } 
  } else {
     m_regsel_stgc.disable();
  }
    
  return StatusCode::SUCCESS;
}


//=====================================================================
StatusCode Muon::sTgcRawDataProvider::execute(const EventContext& ctx) const
{
  // This method is substantially different than the one for TGCs because the hash IDs 
  // corresponding to each ROI (in seeded mode) have quadruplet-level granularity (module=quadruplet).
  // Thus, ROB-level selection is not enough; we need quadruplet-level selection. Therefore,
  // we pass the list of hash IDs to the decoder to make the selection based on decoded elink info.

  ATH_MSG_VERBOSE("sTgcRawDataProvider::execute");

  if(m_seededDecoding) { // RoI - seeded mode

    ATH_MSG_DEBUG("converting sTGC BS into RDOs in ROI-seeded mode");

    // get the RoIs to process
    SG::ReadHandle<TrigRoiDescriptorCollection> muonRoI(m_roiCollectionKey, ctx);
    if(!muonRoI.isValid()){
      ATH_MSG_WARNING("Cannot retrieve muonRoI "<<m_roiCollectionKey.key());
      return StatusCode::FAILURE;
    }

    // get list of offline hash IDs (module context i.e. STGC quadruplets) from region selection
    // using an unordered_set to make sure each hash ID entry is unique
    std::vector<IdentifierHash>        stgc_hash_ids;
    std::unordered_set<IdentifierHash> stgc_hash_ids_set;
    for(auto roi : *muonRoI) {
      ATH_MSG_DEBUG("Getting ROBs for RoI " << *roi);
      m_regsel_stgc->HashIDList(*roi, stgc_hash_ids);
      for (IdentifierHash& hash : stgc_hash_ids) stgc_hash_ids_set.insert(hash);
      stgc_hash_ids.clear();
    }

    // put the IDs back in the vector and decode
    stgc_hash_ids.insert(stgc_hash_ids.end(), stgc_hash_ids_set.begin(), stgc_hash_ids_set.end());
    if (!m_rawDataTool->convert(stgc_hash_ids, ctx).isSuccess()) {
      ATH_MSG_ERROR("STGC BS conversion into RDOs failed");
      return StatusCode::FAILURE;
    }
  } else {
    ATH_MSG_DEBUG("converting sTGC BS into RDOs in unseeded mode");
    if (!m_rawDataTool->convert(ctx).isSuccess()) {
      ATH_MSG_ERROR("STGC BS conversion into RDOs failed");
      return StatusCode::FAILURE;
    }
  }

  return StatusCode::SUCCESS;
}
