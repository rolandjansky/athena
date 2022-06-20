/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonCondAlg/TgcDigitCrosstalkCondAlg.h"
#include "MuonCondSvc/MdtStringUtils.h"
#include "StoreGate/ReadCondHandle.h"
#include "StoreGate/WriteCondHandle.h"
#include "CoralBase/Blob.h"
#include <string_view>

TgcDigitCrosstalkCondAlg::TgcDigitCrosstalkCondAlg(const std::string& name, ISvcLocator* pSvcLocator)
: AthReentrantAlgorithm(name, pSvcLocator) {
}

StatusCode TgcDigitCrosstalkCondAlg::initialize() {
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

StatusCode TgcDigitCrosstalkCondAlg::execute(const EventContext& ctx) const {
  SG::WriteCondHandle<TgcDigitCrosstalkData> writeHandle{m_writeKey, ctx};
  if (writeHandle.isValid()) {
    ATH_MSG_DEBUG("CondHandle " << writeHandle.fullKey() << " is already valid."
		  << ". In theory this should not be called, but may happen"
		  << " if multiple concurrent events are being processed out of order.");
    return StatusCode::SUCCESS;
  }
  SG::ReadCondHandle<CondAttrListCollection> readHandle_XTalk{m_readKey, ctx};
  if (readHandle_XTalk.cptr() == nullptr) {
    ATH_MSG_ERROR("Null pointer to the read conditions object");
    return StatusCode::FAILURE;
  }
  ATH_MSG_DEBUG("Size of CondAttrListCollection" << readHandle_XTalk.fullKey() << " = " << readHandle_XTalk->size());
  EventIDRange rangeW_XTalk;
  if (!readHandle_XTalk.range(rangeW_XTalk)) {
    ATH_MSG_ERROR("Failed to retrieve validity range for " << readHandle_XTalk.key());
    return StatusCode::FAILURE;
  }
  ATH_MSG_DEBUG("Range of input is " << rangeW_XTalk);

  // write condition object
  EventIDRange rangeIntersection = EventIDRange::intersect(rangeW_XTalk);
  if(rangeIntersection.start()>rangeIntersection.stop()) {
    ATH_MSG_ERROR("Invalid intersection range: " << rangeIntersection);
    return StatusCode::FAILURE;
  }
  using namespace MuonCalib;
  // Fill
  auto outputCdo = std::make_unique<TgcDigitCrosstalkData>();
  char delimiter{';'};
  for (const auto &[channel, attribute] : *readHandle_XTalk.cptr()) {
    const coral::Blob& blob_strip = attribute["bXTalk_strip"].data<coral::Blob>();
    const char* charstrip = reinterpret_cast<const char*>(blob_strip.startingAddress());
    std::string_view strstrip(charstrip);

    std::vector<std::string_view> tokens = MdtStringUtils::tokenize(strstrip, delimiter);
    auto it = std::begin(tokens);
    uint16_t station_number = MdtStringUtils::stoi(*it);
    ++it;
    uint16_t station_eta = MdtStringUtils::stoi(*it);
    ++it;
    uint16_t layer = MdtStringUtils::stoi(*it);
    ++it;
    float prob10 = MdtStringUtils::stof(*it);
    ++it;
    float prob11 = MdtStringUtils::stof(*it);
    ++it;
    float prob20 = MdtStringUtils::stof(*it);
    ++it;
    float prob21 = MdtStringUtils::stof(*it);
    ++it;
    uint16_t layerId = (station_number << 5) + (station_eta << 2) + layer;
    std::array<float, TgcDigitCrosstalkData::N_PROB> prob_strip{prob10, prob11, prob20, prob21};
    outputCdo->setStripProbability(layerId, prob_strip);

    const coral::Blob& blob_wire = attribute["bXTalk_wire"].data<coral::Blob>();
    const char* charwire = reinterpret_cast<const char*>(blob_wire.startingAddress());
    std::string_view strwire(charwire);

    tokens.clear();
    tokens = MuonCalib::MdtStringUtils::tokenize(strwire, delimiter);
    it = std::begin(tokens);
    station_number = MdtStringUtils::stoi(*it);
    ++it;
    station_eta = MdtStringUtils::stoi(*it);
    ++it;
    layer = MdtStringUtils::stoi(*it);
    ++it;
    prob10 = MdtStringUtils::stof(*it);
    ++it;
    prob11 = MdtStringUtils::stof(*it);
    ++it;
    prob20 = MdtStringUtils::stof(*it);
    ++it;
    prob21 = MdtStringUtils::stof(*it);
    ++it;
    layerId = (station_number << 5) + (station_eta << 2) + layer;
    std::array<float, TgcDigitCrosstalkData::N_PROB> prob_wire{prob10, prob11, prob20, prob21};
    outputCdo->setWireProbability(layerId, prob_wire);
  }  // end of for(attrmap)

  // Record
  if (writeHandle.record(rangeIntersection, std::move(outputCdo)).isFailure()) {
    ATH_MSG_FATAL("Could not record TgcDigitCrosstalkData " << writeHandle.key()
		  << " with EventRange " << rangeIntersection
		  << " into Conditions Store");
    return StatusCode::FAILURE;
  }
  ATH_MSG_DEBUG("recorded new " << writeHandle.key() << " with range " << rangeIntersection << " into Conditions Store");

  return StatusCode::SUCCESS;
}

