/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "SCTRawDataProviderTool.h"
#include "GaudiKernel/IToolSvc.h"
#include "InDetRawData/SCT_RDO_Container.h"
#include "ByteStreamData/RawEvent.h" 
#include "SCT_RawDataByteStreamCnv/ISCT_RodDecoder.h"
#include "StoreGate/ReadHandle.h"

using OFFLINE_FRAGMENTS_NAMESPACE::ROBFragment;

/// -------------------------------------------------------
/// contructor

SCTRawDataProviderTool::SCTRawDataProviderTool
(const std::string& type, const std::string& name, const IInterface* parent)
  :  AthAlgTool(type, name, parent),
     m_robIdSet()
     {
       declareInterface<ISCTRawDataProviderTool>(this);   
     }

/// -------------------------------------------------------
/// initialize

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

/// -------------------------------------------------------
/// convert method

void SCTRawDataProviderTool::BeginNewEvent() {
   /** reset list of known robIds */
   m_robIdSet.clear(); 
}

StatusCode SCTRawDataProviderTool::convert(std::vector<const ROBFragment*>& vecRobs,
                                           ISCT_RDO_Container& rdoIdc,
                                           InDetBSErrContainer* errs,
                                           SCT_ByteStreamFractionContainer* bsFracCont)
{
  if(vecRobs.empty()) return StatusCode::SUCCESS;
  ATH_MSG_DEBUG("SCTRawDataProviderTool::convert()");
  static int DecodeErrCount{0};

  /**  are we working on a new event ? */
  std::vector<const ROBFragment*>::const_iterator rob_it{vecRobs.begin()};
  
  StatusCode sc{StatusCode::SUCCESS};

  /** loop over the ROB fragments */

  for(; rob_it!=vecRobs.end(); ++rob_it) {
    /** get the ID of this ROB/ROD */
    uint32_t robid{(*rob_it)->rod_source_id()};
    /** check if this ROBFragment was already decoded (EF case in ROIs) */
    if (!m_robIdSet.insert(robid).second) {
      ATH_MSG_DEBUG(" ROB Fragment with ID  "
                    << std::hex<<robid << std::dec
                    << " already decoded, skip"); 
    } else {
      sc = m_decoder->fillCollection(**rob_it, rdoIdc, errs, bsFracCont);
      if (sc==StatusCode::FAILURE) {
        if (DecodeErrCount < 100) {
          ATH_MSG_ERROR("Problem with SCT ByteStream Decoding!");
          DecodeErrCount++;
        } else if (100 == DecodeErrCount) {
          ATH_MSG_ERROR("Too many Problem with SCT Decoding messages, turning message off.  ");
          DecodeErrCount++;
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
