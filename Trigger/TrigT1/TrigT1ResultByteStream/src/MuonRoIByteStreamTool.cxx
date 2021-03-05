/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonRoIByteStreamTool.h"
#include "xAODTrigger/MuonRoI.h"
#include "xAODTrigger/MuonRoIAuxContainer.h"
#include "eformat/SourceIdentifier.h"
#include "eformat/Status.h"
#include "TrigT1Interfaces/ITrigT1MuonRecRoiTool.h"
#include "TrigT1Interfaces/ITrigThresholdDecisionTool.h"

using ROBF = OFFLINE_FRAGMENTS_NAMESPACE::ROBFragment;
using WROBF = OFFLINE_FRAGMENTS_NAMESPACE_WRITE::ROBFragment;

MuonRoIByteStreamTool::MuonRoIByteStreamTool(const std::string& type,
					     const std::string& name,
					     const IInterface* parent)
  : base_class(type, name, parent)
{
}

StatusCode MuonRoIByteStreamTool::initialize() {
  if (m_roiWriteKey.empty() == m_roiReadKey.empty()) {
    ATH_MSG_ERROR("Exactly one of the read / write handle keys has to be set and the other has to be empty, "
                  << "but they are \"" << m_roiReadKey.key() << "\" / \"" << m_roiWriteKey.key() << "\"");
    return StatusCode::FAILURE;
  }

  ATH_CHECK(m_roiWriteKey.initialize(!m_roiWriteKey.empty()));
  ATH_CHECK(m_roiReadKey.initialize(!m_roiReadKey.empty()));

  CHECK( m_rpcTool.retrieve() );
  CHECK( m_tgcTool.retrieve() );
  CHECK( m_thresholdTool.retrieve() );

  return StatusCode::SUCCESS;
}

// BS->xAOD conversion
StatusCode MuonRoIByteStreamTool::convertFromBS(const std::vector<const ROBF*>& vrobf,
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
    ATH_MSG_DEBUG("Muon RoI raw word: " << *data);
    // Here comes the decoding
    handle->push_back(new xAOD::MuonRoI);

    //compute the eta/phi of the RoI
    LVL1::TrigT1MuonRecRoiData roiData;
    if (m_rpcTool->getSystem(*data) == LVL1::ITrigT1MuonRecRoiTool::Barrel) roiData = m_rpcTool->roiData(*data);
    else roiData = m_tgcTool->roiData(*data);

    //get the threshold decisions to find the lowest pt threshold passed
    std::vector<std::pair<std::shared_ptr<TrigConf::L1Threshold>, bool> > decisions = m_thresholdTool->getThresholdDecisions(*data);
    std::pair<std::string, double> minThrInfo = m_thresholdTool->getMinThresholdNameAndValue(decisions, roiData.eta());

    //add an additional flag in the unused MSB to indicate if this is in RUN3 format
    uint32_t word = *data;
    if (m_useRun3Config) word |= 0x1<<31;

    //initialize the xAOD
    handle->back()->initialize(word, roiData.eta(), roiData.phi(), minThrInfo.first, minThrInfo.second);
  }

  ATH_MSG_DEBUG("Decoded " << handle->size() << " Muon RoIs");
  return StatusCode::SUCCESS;
}

/// xAOD->BS conversion
StatusCode MuonRoIByteStreamTool::convertToBS(std::vector<WROBF*>& vrobf,
					      const EventContext& eventContext) {
  // Retrieve the RoI container
  auto muonRoIs = SG::makeHandle(m_roiReadKey, eventContext);
  ATH_CHECK(muonRoIs.isValid());

  // Clear BS data cache
  clearCache(eventContext);

  // Create raw ROD data words
  ATH_MSG_DEBUG("Converting " << muonRoIs->size() << " L1 Muon RoIs to ByteStream");
  uint32_t* data = newRodData(eventContext, muonRoIs->size());
  for (size_t i=0; i<muonRoIs->size(); ++i) {
    data[i] = muonRoIs->at(i)->roiWord();
  }

  // Create ROBFragment containing the ROD words
  const eformat::helper::SourceIdentifier sid(eformat::TDAQ_MUON_CTP_INTERFACE, m_muCTPIModuleID.value());
  const EventIDBase& eid = eventContext.eventID();
  vrobf.push_back(newRobFragment(
    eventContext,
    sid.code(),
    eid.run_number(),
    eid.event_number(),
    eid.bunch_crossing_id(),
    0, // lvl1_type will be overwritten downstream from full event fragment
    0, // detev_type is system-specific
    muonRoIs->size(),
    data,
    eformat::STATUS_BACK // status_position is system-specific
  ));

  return StatusCode::SUCCESS;
}
