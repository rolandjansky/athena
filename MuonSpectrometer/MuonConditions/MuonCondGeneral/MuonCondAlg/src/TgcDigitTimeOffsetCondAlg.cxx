/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonCondAlg/TgcDigitTimeOffsetCondAlg.h"
#include "MuonCondSvc/MdtStringUtils.h"
#include "StoreGate/ReadCondHandle.h"
#include "StoreGate/WriteCondHandle.h"
#include "CoralBase/Blob.h"
#include <string_view>

TgcDigitTimeOffsetCondAlg::TgcDigitTimeOffsetCondAlg(const std::string& name, ISvcLocator* pSvcLocator)
: AthReentrantAlgorithm(name, pSvcLocator) {
}

StatusCode TgcDigitTimeOffsetCondAlg::initialize() {
  ATH_MSG_DEBUG("initialize " << name());
  ATH_CHECK(m_condSvc.retrieve());
  ATH_CHECK(m_readKey.initialize());
  ATH_CHECK(m_writeKey.initialize());
  if (m_condSvc->regHandle(this, m_writeKey).isFailure()) {
    ATH_MSG_FATAL("unable to register WriteCondHandle " << m_writeKey.fullKey() << " with CondSvc");
    return StatusCode::FAILURE;
  }
  return StatusCode::SUCCESS;
}

StatusCode TgcDigitTimeOffsetCondAlg::execute(const EventContext& ctx) const {
  SG::WriteCondHandle<TgcDigitTimeOffsetData> writeHandle{m_writeKey, ctx};
  if (writeHandle.isValid()) {
    ATH_MSG_DEBUG("CondHandle " << writeHandle.fullKey() << " is already valid."
		  << ". In theory this should not be called, but may happen"
		  << " if multiple concurrent events are being processed out of order.");
    return StatusCode::SUCCESS;
  }
  SG::ReadCondHandle<CondAttrListCollection> readHandle_TOffset{m_readKey, ctx};
  if (readHandle_TOffset.cptr() == nullptr) {
    ATH_MSG_ERROR("Null pointer to the read conditions object");
    return StatusCode::FAILURE;
  }
  ATH_MSG_DEBUG("Size of CondAttrListCollection" << readHandle_TOffset.fullKey() << " = " << readHandle_TOffset->size());
  EventIDRange rangeW_TOffset;
  if (!readHandle_TOffset.range(rangeW_TOffset)) {
    ATH_MSG_ERROR("Failed to retrieve validity range for " << readHandle_TOffset.key());
    return StatusCode::FAILURE;
  }
  ATH_MSG_DEBUG("Range of input is " << rangeW_TOffset);

  // write condition object
  EventIDRange rangeIntersection = EventIDRange::intersect(rangeW_TOffset);
  if(rangeIntersection.start()>rangeIntersection.stop()) {
    ATH_MSG_ERROR("Invalid intersection range: " << rangeIntersection);
    return StatusCode::FAILURE;
  }
  using namespace MuonCalib;
  // Fill
  auto outputCdo = std::make_unique<TgcDigitTimeOffsetData>();
  char delimiter{';'};
  for (const auto &[channel, attribute] : *readHandle_TOffset.cptr()) {
    const coral::Blob& blob_strip = attribute["bTimeOffset_strip"].data<coral::Blob>();
    const char* charstrip = reinterpret_cast<const char*>(blob_strip.startingAddress());
    std::string_view strstrip(charstrip);

    std::vector<std::string_view> tokens = MdtStringUtils::tokenize(strstrip, delimiter);
    auto it = std::begin(tokens);
    uint16_t station_number = MdtStringUtils::stoi(*it);
    ++it;
    uint16_t station_eta = MdtStringUtils::stoi(*it);
    ++it;
    float offset_strip = MdtStringUtils::stof(*it);
    ++it;
    uint16_t chamberId = (station_number << 3) + station_eta;
    outputCdo->stripOffset.emplace(chamberId, offset_strip);

    const coral::Blob& blob_wire = attribute["bTimeOffset_wire"].data<coral::Blob>();
    const char* charwire = reinterpret_cast<const char*>(blob_wire.startingAddress());
    std::string_view strwire(charwire);

    tokens.clear();
    tokens = MuonCalib::MdtStringUtils::tokenize(strwire, delimiter);
    it = std::begin(tokens);
    station_number = MdtStringUtils::stoi(*it);
    ++it;
    station_eta = MdtStringUtils::stoi(*it);
    ++it;
    float offset_wire = MdtStringUtils::stof(*it);
    ++it;
    chamberId = (station_number << 3) + station_eta;
    outputCdo->wireOffset.emplace(chamberId, offset_wire);
  }  // end of for(attrmap)

  // Record
  if (writeHandle.record(rangeIntersection, std::move(outputCdo)).isFailure()) {
    ATH_MSG_FATAL("Could not record TgcDigitTimeOffsetData " << writeHandle.key()
		  << " with EventRange " << rangeIntersection
		  << " into Conditions Store");
    return StatusCode::FAILURE;
  }
  ATH_MSG_DEBUG("recorded new " << writeHandle.key() << " with range " << rangeIntersection << " into Conditions Store");

  return StatusCode::SUCCESS;
}

