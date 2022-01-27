/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

// Trigger includes
#include "L1TriggerResultByteStreamCnv.h"
#include "xAODTrigger/TrigCompositeContainer.h"

// Athena includes
#include "AthenaBaseComps/AthCheckMacros.h"
#include "AthenaKernel/ClassID_traits.h"
#include "AthenaKernel/StorableConversions.h"
#include "ByteStreamCnvSvcBase/ByteStreamAddress.h"

// Gaudi includes
#include "GaudiKernel/IRegistry.h"
#include "GaudiKernel/ThreadLocalContext.h"

// TDAQ includes
#include "eformat/Issue.h"
#include "eformat/SourceIdentifier.h"

using WROBF = OFFLINE_FRAGMENTS_NAMESPACE_WRITE::ROBFragment;

// =============================================================================
// Standard constructor
// =============================================================================
L1TriggerResultByteStreamCnv::L1TriggerResultByteStreamCnv(ISvcLocator* svcLoc) :
  Converter(storageType(), classID(), svcLoc),
  AthMessaging(msgSvc(), "L1TriggerResultByteStreamCnv") {}

// =============================================================================
// Standard destructor
// =============================================================================
L1TriggerResultByteStreamCnv::~L1TriggerResultByteStreamCnv() {}

// =============================================================================
// Implementation of Converter::initialize
// =============================================================================
StatusCode L1TriggerResultByteStreamCnv::initialize() {
  ATH_MSG_VERBOSE("start of " << __FUNCTION__);
  ATH_CHECK(m_ByteStreamEventAccess.retrieve());

  // Check one property of the encoder tool to determine if the tool is configured in the job
  const bool doMuon = not serviceLocator()->getOptsSvc().get("ToolSvc.L1MuonBSEncoderTool.ROBIDs").empty();
  ATH_MSG_DEBUG("MUCTPI BS encoding is " << (doMuon ? "enabled" : "disabled"));
  ATH_CHECK(m_muonEncoderTool.retrieve(EnableTool(doMuon)));

  const bool doMuonDaq = not serviceLocator()->getOptsSvc().get("ToolSvc.L1MuonBSEncoderToolDAQ.ROBIDs").empty();
  ATH_MSG_DEBUG("MUCTPI DAQ ROB encoding is " << (doMuonDaq ? "enabled" : "disabled"));
  ATH_CHECK(m_muonEncoderToolDaq.retrieve(EnableTool(doMuonDaq)));

  ATH_MSG_VERBOSE("end of " << __FUNCTION__);
  return StatusCode::SUCCESS;
}

// =============================================================================
// Implementation of Converter::finalize
// =============================================================================
StatusCode L1TriggerResultByteStreamCnv::finalize() {
  ATH_MSG_VERBOSE("start of " << __FUNCTION__);
  if (m_ByteStreamEventAccess.release().isFailure())
    ATH_MSG_WARNING("Failed to release service " << m_ByteStreamEventAccess.typeAndName());
  if (m_muonEncoderTool.isEnabled() && m_muonEncoderTool.release().isFailure())
    ATH_MSG_WARNING("Failed to release tool " << m_muonEncoderTool.typeAndName());
  if (m_muonEncoderToolDaq.isEnabled() && m_muonEncoderToolDaq.release().isFailure())
    ATH_MSG_WARNING("Failed to release tool " << m_muonEncoderToolDaq.typeAndName());
  ATH_MSG_VERBOSE("end of " << __FUNCTION__);
  return StatusCode::SUCCESS;
}

// =============================================================================
// Implementation of Converter::createObj
// =============================================================================
StatusCode L1TriggerResultByteStreamCnv::createObj(IOpaqueAddress* /*pAddr*/, DataObject*& /*pObj*/) {
  ATH_MSG_ERROR("L1TriggerResult cannot be created directly from ByteStream!"
                << " Use the L1TriggerResultMaker algorithm instead");
  return StatusCode::FAILURE;
}

// =============================================================================
// Implementation of Converter::createRep
// =============================================================================
StatusCode L1TriggerResultByteStreamCnv::createRep(DataObject* pObj, IOpaqueAddress*& pAddr) {
  ATH_MSG_VERBOSE("start of " << __FUNCTION__);

  // Cast the DataObject to L1TriggerResult
  xAOD::TrigCompositeContainer* l1TriggerResult = nullptr;
  bool castSuccessful = SG::fromStorable(pObj, l1TriggerResult);
  if (!castSuccessful || !l1TriggerResult) {
    ATH_MSG_ERROR("Failed to convert DataObject to xAOD::TrigCompositeContainer for L1TriggerResult");
    return StatusCode::FAILURE;
  }

  // Obtain the RawEventWrite (aka eformat::write::FullEventFragment) pointer
  RawEventWrite* re = m_ByteStreamEventAccess->getRawEvent();
  if (!re) {
    ATH_MSG_ERROR("Failed to obtain a pointer to RawEventWrite");
    return StatusCode::FAILURE;
  }
  ATH_MSG_VERBOSE("Obtained RawEventWrite pointer = " << re);

  // CTP encoding should come here when implemented and update full event header (L1 trigger bits)
  // in addition to encoding the corresponding ROBFragment.
  // The xAOD CTP result object holding the bits will be obtained here via ElementLink from l1TriggerResult

  // MuonRoI encoding
  for (ToolHandle<IL1TriggerByteStreamTool>& tool : {std::reference_wrapper(m_muonEncoderTool), std::reference_wrapper(m_muonEncoderToolDaq)}) {
    if (not tool.isEnabled()) {continue;}
    std::vector<WROBF*> muon_robs;
    ATH_CHECK(tool->convertToBS(muon_robs, Gaudi::Hive::currentContext())); // TODO: find a way to avoid ThreadLocalContext
    ATH_MSG_DEBUG(tool.name() << " created " << muon_robs.size() << " L1Muon ROB Fragments");
    for (WROBF* rob : muon_robs) {
      printRob(*rob);
      // Set LVL1 Trigger Type from the full event
      rob->rod_lvl1_type(re->lvl1_trigger_type());
      // Set LVL1 ID from the full event
      rob->rod_lvl1_id(re->lvl1_id());
      // Add the ROBFragment to the full event
      re->append(rob);
      ATH_MSG_DEBUG("Added ROB fragment 0x" << MSG::hex << rob->source_id() << MSG::dec << " to the output raw event");
    }
  }

  // Placeholder for other systems: L1Topo, L1Calo

  // Create a ByteStreamAddress for L1TriggerResult
  ByteStreamAddress* bsAddr = new ByteStreamAddress(classID(), pObj->registry()->name(), "");
  pAddr = static_cast<IOpaqueAddress*>(bsAddr);

  ATH_MSG_VERBOSE("end of " << __FUNCTION__);
  return StatusCode::SUCCESS;
}

// =============================================================================
// Debug print helper
// =============================================================================
void L1TriggerResultByteStreamCnv::printRob(const OFFLINE_FRAGMENTS_NAMESPACE_WRITE::ROBFragment& rob) const {
  if (not msgLvl(MSG::DEBUG)) {return;}
  const uint32_t ndata = rob.rod_ndata();
  const uint32_t* data = rob.rod_data();
  ATH_MSG_DEBUG("This ROB has " << ndata << " data words");
  for (uint32_t i=0; i<ndata; ++i, ++data) {
    ATH_MSG_DEBUG("--- 0x" << MSG::hex << *data << MSG::dec);
  }
}

// =============================================================================
// CLID / storageType
// =============================================================================
const CLID& L1TriggerResultByteStreamCnv::classID() {
  return ClassID_traits<xAOD::TrigCompositeContainer>::ID();
}

long L1TriggerResultByteStreamCnv::storageType() {
  return ByteStreamAddress::storageType();
}
