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
                                           SCT_RDO_Container& rdoIDCont,
                                           IDCInDetBSErrContainer& errs) const
{
  if (vecROBFrags.empty()) return StatusCode::SUCCESS;
  ATH_MSG_DEBUG("SCTRawDataProviderTool::convert()");


  // loop over the ROB fragments
  StatusCode sc{StatusCode::SUCCESS};
  for (const ROBFragment* robFrag : vecROBFrags) {
    // get the ID of this ROB/ROD
    sc = m_decoder->fillCollection(*robFrag, rdoIDCont, errs);
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

  if (sc == StatusCode::FAILURE) {
    ATH_MSG_ERROR("There was a problem with SCT ByteStream conversion");
    return sc;
  }

  return sc;
}
