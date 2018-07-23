/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// SCTRawDataProviderTool.cxx
//   Implementation file for class SCTRawDataProviderTool
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#include "SCTRawDataProviderTool.h"
#include "StoreGate/ReadHandle.h"

using OFFLINE_FRAGMENTS_NAMESPACE::ROBFragment;

// -------------------------------------------------------
// Constructor

SCTRawDataProviderTool::SCTRawDataProviderTool
(const std::string& type, const std::string& name, const IInterface* parent)
  : base_class(type, name, parent),
    m_robIdSet()
{
}

// -------------------------------------------------------
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

// -------------------------------------------------------
// Convert method

StatusCode SCTRawDataProviderTool::convert(std::vector<const ROBFragment*>& vecRobs,
                                           ISCT_RDO_Container& rdoIdc,
                                           InDetBSErrContainer* errs,
                                           SCT_ByteStreamFractionContainer* bsFracCont)
{
  if(vecRobs.empty()) return StatusCode::SUCCESS;
  ATH_MSG_DEBUG("SCTRawDataProviderTool::convert()");
  
  m_decodeErrCount = 0;

  StatusCode sc{StatusCode::SUCCESS};

  // loop over the ROB fragments
  for(const ROBFragment* rob_it : vecRobs) {
    // get the ID of this ROB/ROD
    uint32_t robid{(rob_it)->rod_source_id()};
    // check if this ROBFragment was already decoded (EF case in ROIs)
    if (!m_robIdSet.insert(robid).second) {
      ATH_MSG_DEBUG(" ROB Fragment with ID  "
                    << std::hex<<robid << std::dec
                    << " already decoded, skip"); 
    } 
    else {
      sc = m_decoder->fillCollection(*rob_it, rdoIdc, errs, bsFracCont);
      if (sc==StatusCode::FAILURE) {
        if (m_decodeErrCount < 100) {
          ATH_MSG_ERROR("Problem with SCT ByteStream Decoding!");
          m_decodeErrCount++;
        } 
        else if (100 == m_decodeErrCount) {
          ATH_MSG_ERROR("Too many Problem with SCT Decoding messages, turning message off.  ");
          m_decodeErrCount++;
        }
      }
    }
  }

  if (sc==StatusCode::FAILURE) {
    ATH_MSG_ERROR("There was a problem with SCT ByteStream conversion");
    return sc;
  }

  return sc;
}

// -------------------------------------------------------
// beginNewEvent method

void SCTRawDataProviderTool::beginNewEvent() {
   // reset list of known robIds
   m_robIdSet.clear(); 
}
