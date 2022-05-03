/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonCondAlg/TgcDigitASDposCondAlg.h"
#include "MuonCondSvc/MdtStringUtils.h"
#include "StoreGate/ReadCondHandle.h"
#include "StoreGate/WriteCondHandle.h"
#include "CoralBase/Blob.h"

TgcDigitASDposCondAlg::TgcDigitASDposCondAlg(const std::string& name, ISvcLocator* pSvcLocator) :
  AthReentrantAlgorithm(name, pSvcLocator)
{}

StatusCode TgcDigitASDposCondAlg::initialize()
{
  ATH_MSG_DEBUG("initialize " << name());

  ATH_CHECK(m_readKey_ASDpos.initialize());
  ATH_CHECK(m_writeKey.initialize());

  return StatusCode::SUCCESS;
}

StatusCode TgcDigitASDposCondAlg::execute(const EventContext& ctx) const
{
  SG::WriteCondHandle<TgcDigitASDposData> writeHandle{m_writeKey, ctx};
  if (writeHandle.isValid()) {
    ATH_MSG_DEBUG("CondHandle " << writeHandle.fullKey() << " is already valid."
		  << ". In theory this should not be called, but may happen"
		  << " if multiple concurrent events are being processed out of order.");
    return StatusCode::SUCCESS;
  }

  SG::ReadCondHandle<CondAttrListCollection> readHandle_ASDpos{m_readKey_ASDpos, ctx};
  if (readHandle_ASDpos.cptr() == nullptr) {
    ATH_MSG_ERROR("Null pointer to the read conditions object");
    return StatusCode::FAILURE;
  }

  ATH_MSG_DEBUG("Size of CondAttrListCollection" << readHandle_ASDpos.fullKey() << " = " << readHandle_ASDpos->size());

  EventIDRange rangeW_ASDpos;
  if (!readHandle_ASDpos.range(rangeW_ASDpos)) {
    ATH_MSG_ERROR("Failed to retrieve validity range for " << readHandle_ASDpos.key());
    return StatusCode::FAILURE;
  }
  ATH_MSG_DEBUG("Range of input is " << rangeW_ASDpos);

  // write condition object
  EventIDRange rangeIntersection = EventIDRange::intersect(rangeW_ASDpos);
  if(rangeIntersection.start()>rangeIntersection.stop()) {
    ATH_MSG_ERROR("Invalid intersection range: " << rangeIntersection);
    return StatusCode::FAILURE;
  }

  // Fill
  auto outputCdo = std::make_unique<TgcDigitASDposData>();

  std::string delimiter{";"};
  for(const auto &[channel, attribute] : *readHandle_ASDpos.cptr()) {
    const coral::Blob& blob = attribute["bASDPos"].data<coral::Blob>();
    const char *blobCStr = reinterpret_cast<const char *>(blob.startingAddress());
    std::string blobline(blobCStr,blob.size()/sizeof(char));
    std::vector<std::string> tokens;
    MuonCalib::MdtStringUtils::tokenize(blobline, tokens, delimiter);
    auto it = std::begin(tokens);
    uint16_t station = static_cast<uint16_t>(stoi(*it));
    ++it;
    uint16_t eta = static_cast<uint16_t>(stoi(*it));
    ++it;
    uint16_t phi = (stoi(*it) == -99) ? 0x1f : static_cast<uint16_t>(stoi(*it));
    uint16_t chamberId = (station << 8)  + (eta << 5) + phi;

    std::vector<float> strip_pos;
    strip_pos.assign(TgcDigitASDposData::N_STRIPASDPOS, 0);
    for (int i=0; i < TgcDigitASDposData::N_STRIPASDPOS; i++) {
      ++it;
      strip_pos[i] = stof(*it);
    }
    outputCdo->stripAsdPos.insert(std::make_pair(chamberId, strip_pos));

    std::vector<float> wire_pos;
    wire_pos.assign(TgcDigitASDposData::N_WIREASDPOS, 0);
    for (int i=0; i < TgcDigitASDposData::N_WIREASDPOS; i++) {
      ++it;
      wire_pos[i] = stof(*it);
    }
    outputCdo->wireAsdPos.insert(std::make_pair(chamberId, wire_pos));
  }  // end of for(attrmap)

  // Record
  if (writeHandle.record(rangeIntersection, std::move(outputCdo)).isFailure()) {
    ATH_MSG_FATAL("Could not record TgcDigitASDposData " << writeHandle.key()
		  << " with EventRange " << rangeIntersection
		  << " into Conditions Store");
    return StatusCode::FAILURE;
  }
  ATH_MSG_DEBUG("recorded new " << writeHandle.key() << " with range " << rangeIntersection << " into Conditions Store");

  return StatusCode::SUCCESS;
}
