/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "ExampleL1TriggerByteStreamTool.h"
#include "xAODTrigger/MuonRoI.h"
#include "xAODTrigger/MuonRoIAuxContainer.h"
#include "eformat/SourceIdentifier.h"

using ROBF = OFFLINE_FRAGMENTS_NAMESPACE::ROBFragment;

ExampleL1TriggerByteStreamTool::ExampleL1TriggerByteStreamTool(const std::string& type,
                                                               const std::string& name,
                                                               const IInterface* parent)
: base_class(type, name, parent) {}

StatusCode ExampleL1TriggerByteStreamTool::initialize() {
  ATH_CHECK(m_roiWriteKey.initialize(!m_roiWriteKey.empty()));
  return StatusCode::SUCCESS;
}

// BS->xAOD conversion
StatusCode ExampleL1TriggerByteStreamTool::convert(const std::vector<const ROBF*>& vrobf,
                                                   xAOD::TrigComposite& l1TriggerResult,
                                                   const EventContext& eventContext) const {
  if (m_roiWriteKey.empty()) {
    ATH_MSG_ERROR("Conversion from BS to xAOD RoI requested but RoI WriteHandleKey is empty");
    return StatusCode::FAILURE;
  }

  // Create and record the RoI container
  auto handle = SG::makeHandle(m_roiWriteKey, eventContext);
  auto cont = std::make_unique<xAOD::MuonRoIContainer>();
  auto auxcont = std::make_unique<xAOD::MuonRoIAuxContainer>();
  cont->setStore(auxcont.get());
  ATH_CHECK(handle.record(std::move(cont), std::move(auxcont)));
  ATH_MSG_DEBUG("Recorded MuonRoIContainer with key " << m_roiWriteKey.key());

  // Link the RoI container (actually its first element) to L1TriggerResult
  ElementLink<xAOD::MuonRoIContainer> link = ElementLink<xAOD::MuonRoIContainer>(m_roiWriteKey.key(), 0, eventContext);
  l1TriggerResult.typelessSetObjectLink(m_linkName.value(),
                                        link.key(),
                                        ClassID_traits<xAOD::MuonRoIContainer>::ID(),
                                        /*index =*/ 0);

  // Find the ROB fragment to decode
  const eformat::helper::SourceIdentifier sid(eformat::TDAQ_MUON_CTP_INTERFACE, m_muCTPIModuleID.value());
  auto it = std::find_if(vrobf.begin(), vrobf.end(), [&sid](const ROBF* rob){return rob->rob_source_id() == sid.code();});
  if (it == vrobf.end()) {
    ATH_MSG_DEBUG("No MUCTPI ROB fragment with ID " << sid.code() << " was found, MuonRoIContainer will be empty");
    return StatusCode::SUCCESS;
  }

  // Iterate over ROD words and decode
  const ROBF* rob = *it;
  const uint32_t ndata = rob->rod_ndata();
  const uint32_t* data = rob->rod_data();
  ATH_MSG_DEBUG("Starting to decode " << ndata << " ROD words");
  for (uint32_t i=0; i<ndata; ++i, ++data) {
    // Here comes the decoding
    // Using some dummy values as this is not real decoding, just an example
    handle->push_back(new xAOD::MuonRoI);
    handle->back()->initialize(*data, 99, 99, "DummyThreshold", 99);
  }

  ATH_MSG_DEBUG("Decoded " << handle->size() << " Muon RoIs");
  return StatusCode::SUCCESS;
}

/// xAOD->BS conversion
StatusCode ExampleL1TriggerByteStreamTool::convert(const xAOD::TrigComposite& l1TriggerResult,
                                                   std::vector<const ROBF*>& /*vrobf*/,
                                                   const EventContext& /*eventContext*/) const {

  // Retrieve the RoI container
  if (!l1TriggerResult.hasObjectLink(m_linkName.value())) {
    ATH_MSG_ERROR("L1TriggerResult does not have a link \"" << m_linkName << "\"");
    return StatusCode::FAILURE;
  }

  // TODO: implement this part when new code requesting the xAOD->BS conversion is implemented (ATR-19542)

  return StatusCode::SUCCESS;
}
