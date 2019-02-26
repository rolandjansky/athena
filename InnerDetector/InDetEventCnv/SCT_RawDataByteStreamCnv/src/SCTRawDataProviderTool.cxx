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
  base_class(type, name, parent),
  m_robIDSet{},
  m_decodeErrCount{0},
  m_mutex{}
{
}

// Initialize

StatusCode SCTRawDataProviderTool::initialize()
{
  StatusCode sc{AlgTool::initialize()};
  if (sc.isFailure()) {
    ATH_MSG_FATAL("Failed to init baseclass");
    return StatusCode::FAILURE;
  }
   
  ATH_CHECK(m_decoder.retrieve());

  return StatusCode::SUCCESS;
}

// Convert method

StatusCode SCTRawDataProviderTool::convert(std::vector<const ROBFragment*>& vecROBFrags,
                                           ISCT_RDO_Container& rdoIDCont,
                                           InDetBSErrContainer* errs,
                                           SCT_ByteStreamFractionContainer* bsFracCont) const
{
  if (vecROBFrags.empty()) return StatusCode::SUCCESS;
  ATH_MSG_DEBUG("SCTRawDataProviderTool::convert()");
  
  StatusCode sc{StatusCode::SUCCESS};

  std::lock_guard<std::mutex> lock(m_mutex);

  // loop over the ROB fragments

  std::set<uint32_t> tmpROBIDSet;

  for (const ROBFragment* robFrag : vecROBFrags) {
    // get the ID of this ROB/ROD
    uint32_t robid{(robFrag)->rod_source_id()};
    // check if this ROBFragment was already decoded (EF case in ROIs)
    if (m_robIDSet.count(robid) or tmpROBIDSet.count(robid)) {
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

  m_robIDSet.insert(tmpROBIDSet.begin(), tmpROBIDSet.end());

  if (sc == StatusCode::FAILURE) {
    ATH_MSG_ERROR("There was a problem with SCT ByteStream conversion");
    return sc;
  }

  return sc;
}

// beginNewEvent method

void SCTRawDataProviderTool::beginNewEvent() const 
{
  // reset list of known robIDs
  std::lock_guard<std::mutex> lock(m_mutex);
  m_robIDSet.clear();
}
