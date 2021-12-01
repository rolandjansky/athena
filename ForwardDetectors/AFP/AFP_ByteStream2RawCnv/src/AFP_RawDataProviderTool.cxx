/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

// AFP_ByteStream2RawCnv includes
#include "AFP_ByteStream2RawCnv/AFP_RawDataProviderTool.h"

static const InterfaceID
    IID_IAFP_RawCollByteStreamTool("AFP_RawDataProviderTool", 1, 0);
const InterfaceID &AFP_RawDataProviderTool::interfaceID() {
  return IID_IAFP_RawCollByteStreamTool;
}

AFP_RawDataProviderTool::AFP_RawDataProviderTool(const std::string &type,
                                                 const std::string &name,
                                                 const IInterface *parent)
    : AthAlgTool(type, name, parent)
{
  declareInterface<AFP_RawDataProviderTool>(this);
}

AFP_RawDataProviderTool::~AFP_RawDataProviderTool() {}

StatusCode AFP_RawDataProviderTool::initialize() {
  ATH_MSG_INFO("Initializing " << name() << "...");

  StatusCode sc = AthAlgTool::initialize();
  ATH_MSG_DEBUG("INITIALIZE AFP_RawDataProviderTool");
  if (sc.isFailure()) {
    ATH_MSG_WARNING(" Failed to init baseclass");
    return StatusCode::SUCCESS;
  }

  if (m_decoder.retrieve().isFailure()) {
    ATH_MSG_WARNING("FAILED TO RETRIEVE TOOL " << m_decoder);
    return StatusCode::SUCCESS;
  } else {
    ATH_MSG_DEBUG("Retrieved decoder tool " << m_decoder);
  }
  return StatusCode::SUCCESS;
}

StatusCode
AFP_RawDataProviderTool::convert(std::vector<const OFFLINE_FRAGMENTS_NAMESPACE::ROBFragment *> &vecRobs,
                                 AFP_RawContainer *rawContainer) const
{
  std::set<uint32_t> robIdSet;

  if (!rawContainer) {
    ATH_MSG_WARNING("NULL pointer passed in rawContainer.");
    return StatusCode::SUCCESS;
  }

  if (vecRobs.empty()) {
    ATH_MSG_DEBUG("vecRobs.size() == 0");
    return StatusCode::SUCCESS;
  }

  // Loop over robs and fill rawContainer
  for (const OFFLINE_FRAGMENTS_NAMESPACE::ROBFragment* rob : vecRobs) {
    const uint32_t robId = rob->rod_source_id();

    if (robIdSet.insert(robId).second) {
      StatusCode sc = m_decoder->fillCollection(rob, rawContainer);
      if (sc.isFailure()) {
        ATH_MSG_ERROR("Failed filling collection");
        return StatusCode::FAILURE;
      }
    }

  } // end for loop

  return StatusCode::SUCCESS;
}

StatusCode AFP_RawDataProviderTool::finalize() {
  StatusCode sc = AthAlgTool::finalize();
  ATH_MSG_DEBUG("FINALIZE AFP_RawDataProviderTool");
  if (sc.isFailure()) {
    ATH_MSG_WARNING("FINALIZE failed");
    return StatusCode::SUCCESS;
  }

  return StatusCode::SUCCESS;
}
