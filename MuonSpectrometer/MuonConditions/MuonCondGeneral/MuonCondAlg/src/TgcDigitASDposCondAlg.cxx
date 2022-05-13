/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonCondAlg/TgcDigitASDposCondAlg.h"
#include "MuonCondSvc/MdtStringUtils.h"
#include "StoreGate/ReadCondHandle.h"
#include "StoreGate/WriteCondHandle.h"
#include "CoralBase/Blob.h"
#include <string_view>


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
  using namespace MuonCalib;
  char delimiter{';'};
  for(const auto &[channel, attribute] : *readHandle_ASDpos.cptr()) {
    const coral::Blob& blob = attribute["bASDPos"].data<coral::Blob>();
    const char *blobCStr = reinterpret_cast<const char *>(blob.startingAddress());
    std::string_view blobline(blobCStr, blob.size()/sizeof(char));
    std::vector<std::string_view> tokens = MuonCalib::MdtStringUtils::tokenize(blobline, delimiter);
    auto it = std::begin(tokens);
    uint16_t station = static_cast<uint16_t>(MdtStringUtils::stoi(*it));
    ++it;
    uint16_t eta = static_cast<uint16_t>(MdtStringUtils::stoi(*it));
    ++it;
    uint16_t phi = (MdtStringUtils::stoi(*it) == -99) ? 0x1f : static_cast<uint16_t>(MdtStringUtils::stoi(*it));
    uint16_t chamberId = (station << 8)  + (eta << 5) + phi;

    std::vector<float> strip_pos(TgcDigitASDposData::N_STRIPASDPOS, 0);
    //strip_pos initialized to size N_STRIPASDPOS
    for (int i=0; i < TgcDigitASDposData::N_STRIPASDPOS; i++) {
      ++it;
      strip_pos[i] = MdtStringUtils::stof(*it);
    }
    outputCdo->stripAsdPos.emplace(chamberId, std::move(strip_pos));

    std::vector<float> wire_pos(TgcDigitASDposData::N_WIREASDPOS, 0);
    //TgcDigitASDposData initialized to size N_WIREASDPOS
    for (int i=0; i < TgcDigitASDposData::N_WIREASDPOS; i++) {
      ++it;
      wire_pos[i] = MdtStringUtils::stof(*it);
    }
    outputCdo->wireAsdPos.emplace(chamberId, std::move(wire_pos));
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
