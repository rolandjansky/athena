/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "SCTRawDataProviderTool.h"

#include "SCT_RawDataByteStreamCnv/ISCT_RodDecoder.h"
#include "StoreGate/ReadHandle.h"

using OFFLINE_FRAGMENTS_NAMESPACE::ROBFragment;

// Constructor
SCTRawDataProviderTool::SCTRawDataProviderTool(const std::string& type, const std::string& name, 
                                               const IInterface* parent) : 
  base_class(type, name, parent)
{
}

// Initialize
StatusCode SCTRawDataProviderTool::initialize()
{
  ATH_CHECK(m_decoder.retrieve());

  return StatusCode::SUCCESS;
}

// Convert method
StatusCode SCTRawDataProviderTool::convert(std::vector<const ROBFragment*>& vecROBFrags,
                                           ISCT_RDO_Container& rdoIDCont,
                                           InDetBSErrContainer* errs,
                                           SCT_ByteStreamFractionContainer* bsFracCont) const
{
  const EventContext& ctx{Gaudi::Hive::currentContext()};
  return convert(vecROBFrags, rdoIDCont, errs, bsFracCont, ctx);
}

// Convert method
StatusCode SCTRawDataProviderTool::convert(std::vector<const ROBFragment*>& vecROBFrags,
                                           ISCT_RDO_Container& rdoIDCont,
                                           InDetBSErrContainer* errs,
                                           SCT_ByteStreamFractionContainer* bsFracCont,
                                           const EventContext& ctx) const
{
  if (vecROBFrags.empty()) return StatusCode::SUCCESS;
  ATH_MSG_DEBUG("SCTRawDataProviderTool::convert()");
  
  // Retrieve or prepare the already decoded ROBIDs in this thread.
  std::lock_guard<std::mutex> lock{m_mutex};
  CacheEntry* ent{m_cache.get(ctx)};
  if (ent->m_evt != ctx.evt()) {
    ent->m_evt = ctx.evt();
    ent->m_robIDSet.clear();
  }

  // loop over the ROB fragments
  StatusCode sc{StatusCode::SUCCESS};
  std::unordered_set<uint32_t> tmpROBIDSet;
  for (const ROBFragment* robFrag : vecROBFrags) {
    // get the ID of this ROB/ROD
    uint32_t robid{(robFrag)->rod_source_id()};
    // check if this ROBFragment was already decoded (EF case in ROIs)
    if (ent->m_robIDSet.count(robid) or tmpROBIDSet.count(robid)) {
      ATH_MSG_DEBUG(" ROB Fragment with ID "
                    << std::hex<<robid << std::dec
                    << " already decoded, skip");
      continue;
    }

    // Insert the new ROBID to the set.
    tmpROBIDSet.insert(robid);

    sc = m_decoder->fillCollection(*robFrag, rdoIDCont, errs, bsFracCont);
    if (sc == StatusCode::FAILURE) {
      if (m_decodeErrCount <= 100) {
        if (100 == m_decodeErrCount) {
          ATH_MSG_ERROR("Too many Problem with SCT Decoding messages, turning message off.");
        }
        else {
          ATH_MSG_ERROR("Problem with SCT ByteStream Decoding!");
        }
        m_decodeErrCount++;
      }
    }
  }

  // Insert the ROBIDs decoded in this process to the already decoded ROBIDs.
  ent->m_robIDSet.insert(tmpROBIDSet.begin(), tmpROBIDSet.end());

  if (sc == StatusCode::FAILURE) {
    ATH_MSG_ERROR("There was a problem with SCT ByteStream conversion");
    return sc;
  }

  return sc;
}
