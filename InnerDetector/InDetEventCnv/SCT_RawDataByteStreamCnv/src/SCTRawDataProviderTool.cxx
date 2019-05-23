/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
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
  
  StatusCode sc{StatusCode::SUCCESS};

  std::lock_guard<std::recursive_mutex> lock{m_mutex};

  std::set<uint32_t>& robIDSet{getRobIDSet(ctx)};

  // loop over the ROB fragments

  std::set<uint32_t> tmpROBIDSet;

  for (const ROBFragment* robFrag : vecROBFrags) {
    // get the ID of this ROB/ROD
    uint32_t robid{(robFrag)->rod_source_id()};
    // check if this ROBFragment was already decoded (EF case in ROIs)
    if (robIDSet.count(robid) or tmpROBIDSet.count(robid)) {
      ATH_MSG_DEBUG(" ROB Fragment with ID  "
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

  robIDSet.insert(tmpROBIDSet.begin(), tmpROBIDSet.end());

  if (sc == StatusCode::FAILURE) {
    ATH_MSG_ERROR("There was a problem with SCT ByteStream conversion");
    return sc;
  }

  return sc;
}

std::set<uint32_t>& SCTRawDataProviderTool::getRobIDSet(const EventContext& ctx) const {
  std::lock_guard<std::recursive_mutex> lock{m_mutex};
  EventContext::ContextID_t slot{ctx.slot()};
  EventContext::ContextEvt_t evt{ctx.evt()};

  if (slot<m_cache.size() and m_cache[slot]==evt) {
    // Cache is valid
    return m_robIDSet[slot];
  }

  // Expand cache if necessary
  static const EventContext::ContextEvt_t invalidValue{EventContext::INVALID_CONTEXT_EVT};  
  if (slot>=m_cache.size()) {
    m_cache.resize(slot+1, invalidValue);
    m_robIDSet.resize(slot+1);
  }

  // Set event number and clear cache for the new event
  m_cache[slot] = evt;
  m_robIDSet[slot].clear();

  return m_robIDSet[slot];
}
