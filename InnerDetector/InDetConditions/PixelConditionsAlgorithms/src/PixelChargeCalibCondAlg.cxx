/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "PixelChargeCalibCondAlg.h"
#include "Identifier/IdentifierHash.h"
#include "InDetReadoutGeometry/SiDetectorElement.h"
#include "PixelReadoutGeometry/PixelModuleDesign.h"
#include "GaudiKernel/EventIDRange.h"
#include <memory>
#include <sstream>

#include <iostream>
#include <fstream>
#include <iomanip>

namespace
{

constexpr int halfModuleThreshold{8};
constexpr size_t FEStringSize{21};

} // namespace


PixelChargeCalibCondAlg::PixelChargeCalibCondAlg(const std::string& name, ISvcLocator* pSvcLocator):
  ::AthReentrantAlgorithm(name, pSvcLocator)
{
}

StatusCode PixelChargeCalibCondAlg::initialize() {
  ATH_MSG_DEBUG("PixelChargeCalibCondAlg::initialize()");

  ATH_CHECK(detStore()->retrieve(m_pixelID,"PixelID"));
  ATH_CHECK(m_pixelDetEleCollKey.initialize());
  ATH_CHECK(m_configKey.initialize());
  ATH_CHECK(m_readKey.initialize(SG::AllowEmpty));
  ATH_CHECK(m_writeKey.initialize());

  return StatusCode::SUCCESS;
}

StatusCode PixelChargeCalibCondAlg::execute(const EventContext& ctx) const {
  ATH_MSG_DEBUG("PixelChargeCalibCondAlg::execute()");

  SG::WriteCondHandle<PixelChargeCalibCondData> writeHandle(m_writeKey, ctx);
  if (writeHandle.isValid()) {
    ATH_MSG_DEBUG("CondHandle " << writeHandle.fullKey() << " is already valid.. In theory this should not be called, but may happen if multiple concurrent events are being processed out of order.");
    return StatusCode::SUCCESS;
  }

  SG::ReadCondHandle<InDetDD::SiDetectorElementCollection> pixelDetEleHandle(m_pixelDetEleCollKey, ctx);
  const InDetDD::SiDetectorElementCollection* elements(*pixelDetEleHandle);
  if (not pixelDetEleHandle.isValid() or elements==nullptr) {
    ATH_MSG_FATAL(m_pixelDetEleCollKey.fullKey() << " is not available.");
    return StatusCode::FAILURE;
  }

  const std::array<InDetDD::PixelDiodeType, 4> diodeTypes
    = {InDetDD::PixelDiodeType::NORMAL, InDetDD::PixelDiodeType::LONG, InDetDD::PixelDiodeType::GANGED, InDetDD::PixelDiodeType::LARGE};

  SG::ReadCondHandle<PixelModuleData> configDataHandle(m_configKey, ctx);
  const PixelModuleData *configData = *configDataHandle;

  // Construct the output Cond Object and fill it in
  std::unique_ptr<PixelChargeCalibCondData> writeCdo(std::make_unique<PixelChargeCalibCondData>());

  const EventIDBase start{EventIDBase::UNDEFNUM, EventIDBase::UNDEFEVT, 0,                       0,                       EventIDBase::UNDEFNUM, EventIDBase::UNDEFNUM};
  const EventIDBase stop {EventIDBase::UNDEFNUM, EventIDBase::UNDEFEVT, EventIDBase::UNDEFNUM-1, EventIDBase::UNDEFNUM-1, EventIDBase::UNDEFNUM, EventIDBase::UNDEFNUM};

  EventIDRange rangeW{start, stop};
  if (!m_readKey.empty()) {
    SG::ReadCondHandle<CondAttrListCollection> readHandle(m_readKey, ctx);
    const CondAttrListCollection* readCdo = *readHandle;
    if (readCdo==nullptr) {
      ATH_MSG_FATAL("Null pointer to the read conditions object");
      return StatusCode::FAILURE;
    }
    // Get the validitiy range
    if (not readHandle.range(rangeW)) {
      ATH_MSG_FATAL("Failed to retrieve validity range for " << readHandle.key());
      return StatusCode::FAILURE;
    }
    ATH_MSG_INFO("Size of CondAttrListCollection " << readHandle.fullKey() << " readCdo->size()= " << readCdo->size());
    ATH_MSG_INFO("Range of input is " << rangeW);

    for (CondAttrListCollection::const_iterator attrList=readCdo->begin(); attrList!=readCdo->end(); ++attrList) {
      const CondAttrListCollection::ChanNum &channelNumber = attrList->first;
      const CondAttrListCollection::AttributeList &payload = attrList->second;

      // RUN-2 format
      if (payload.exists("data") and not payload["data"].isNull()) {
        std::string stringStatus = payload["data"].data<std::string>();

        std::stringstream ss(stringStatus);
        std::vector<std::string> component;
        std::string buffer;
        std::getline(ss, buffer, '\n'); // skip first line
        while (std::getline(ss, buffer, '\n')) { component.push_back(buffer); }

        std::vector<int> analogThreshold;
        std::vector<int> analogThresholdSigma;
        std::vector<int> analogThresholdNoise;
        std::vector<int> inTimeThreshold;

        std::vector<int> analogThresholdLong;
        std::vector<int> analogThresholdSigmaLong;
        std::vector<int> analogThresholdNoiseLong;
        std::vector<int> inTimeThresholdLong;

        std::vector<int> analogThresholdGanged;
        std::vector<int> analogThresholdSigmaGanged;
        std::vector<int> analogThresholdNoiseGanged;
        std::vector<int> inTimeThresholdGanged;

        std::vector<float> totA;
        std::vector<float> totE;
        std::vector<float> totC;

        std::vector<float> totAGanged;
        std::vector<float> totEGanged;
        std::vector<float> totCGanged;

        std::vector<float> totRes1;
        std::vector<float> totRes2;

        analogThreshold.reserve(component.size());
        analogThresholdSigma.reserve(component.size());
        analogThresholdNoise.reserve(component.size());
        inTimeThreshold.reserve(component.size());

        analogThresholdLong.reserve(component.size());
        analogThresholdSigmaLong.reserve(component.size());
        analogThresholdNoiseLong.reserve(component.size());
        inTimeThresholdLong.reserve(component.size());

        analogThresholdGanged.reserve(component.size());
        analogThresholdSigmaGanged.reserve(component.size());
        analogThresholdNoiseGanged.reserve(component.size());
        inTimeThresholdGanged.reserve(component.size());

        totA.reserve(component.size());
        totE.reserve(component.size());
        totC.reserve(component.size());

        totAGanged.reserve(component.size());
        totEGanged.reserve(component.size());
        totCGanged.reserve(component.size());

        totRes1.reserve(component.size());
        totRes2.reserve(component.size());

        // loop over FEs
        for (size_t i{}; i < component.size(); i++) {
          std::stringstream checkFE(component[i]);
          std::vector<std::string> FEString;
          while (std::getline(checkFE, buffer, ' ')) { FEString.push_back(buffer); }

          if (FEString.size() < FEStringSize) {
            ATH_MSG_ERROR("size of FEString is " << FEString.size() << " and is less than expected, " << FEStringSize << ".");
            ATH_MSG_ERROR("This is the problem in the contents in conditions DB. This should rather be fixed in DB-side.");
            return StatusCode::FAILURE;
          }

          analogThreshold.push_back(std::stoi(FEString[1]));
          analogThresholdSigma.push_back(std::stoi(FEString[2]));
          analogThresholdNoise.push_back(std::stoi(FEString[3]));
          inTimeThreshold.push_back(std::stoi(FEString[4]));

          analogThresholdLong.push_back(std::stoi(FEString[5]));
          analogThresholdSigmaLong.push_back(std::stoi(FEString[6]));
          analogThresholdNoiseLong.push_back(std::stoi(FEString[7]));
          inTimeThresholdLong.push_back(std::stoi(FEString[8]));

          analogThresholdGanged.push_back(std::stoi(FEString[9]));
          analogThresholdSigmaGanged.push_back(std::stoi(FEString[10]));
          analogThresholdNoiseGanged.push_back(std::stoi(FEString[11]));
          inTimeThresholdGanged.push_back(std::stoi(FEString[12]));

          totA.push_back(std::stof(FEString[13]));
          totE.push_back(std::stof(FEString[14]));
          totC.push_back(std::stof(FEString[15]));

          totAGanged.push_back(std::stof(FEString[16]));
          totEGanged.push_back(std::stof(FEString[17]));
          totCGanged.push_back(std::stof(FEString[18]));

          totRes1.push_back(std::stof(FEString[19]));
          totRes2.push_back(std::stof(FEString[20]));
        }

        // Normal pixel
        writeCdo -> setAnalogThreshold(InDetDD::PixelDiodeType::NORMAL, channelNumber, std::move(analogThreshold));
        writeCdo -> setAnalogThresholdSigma(InDetDD::PixelDiodeType::NORMAL, channelNumber, std::move(analogThresholdSigma));
        writeCdo -> setAnalogThresholdNoise(InDetDD::PixelDiodeType::NORMAL, channelNumber, std::move(analogThresholdNoise));
        writeCdo -> setInTimeThreshold(InDetDD::PixelDiodeType::NORMAL, channelNumber, std::move(inTimeThreshold));

        writeCdo -> setQ2TotA(InDetDD::PixelDiodeType::NORMAL, channelNumber, totA); // can not move as shared
        writeCdo -> setQ2TotE(InDetDD::PixelDiodeType::NORMAL, channelNumber, totE); // can not move as shared
        writeCdo -> setQ2TotC(InDetDD::PixelDiodeType::NORMAL, channelNumber, totC); // can not move as shared

        writeCdo -> setTotRes1(channelNumber, std::move(totRes1));
        writeCdo -> setTotRes2(channelNumber, std::move(totRes2));

        // Long pixel
        writeCdo -> setAnalogThreshold(InDetDD::PixelDiodeType::LONG, channelNumber, std::move(analogThresholdLong));
        writeCdo -> setAnalogThresholdSigma(InDetDD::PixelDiodeType::LONG, channelNumber, std::move(analogThresholdSigmaLong));
        writeCdo -> setAnalogThresholdNoise(InDetDD::PixelDiodeType::LONG, channelNumber, std::move(analogThresholdNoiseLong));
        writeCdo -> setInTimeThreshold(InDetDD::PixelDiodeType::LONG, channelNumber, std::move(inTimeThresholdLong));

        writeCdo -> setQ2TotA(InDetDD::PixelDiodeType::LONG, channelNumber, totA); // can not move as shared
        writeCdo -> setQ2TotE(InDetDD::PixelDiodeType::LONG, channelNumber, totE); // can not move as shared
        writeCdo -> setQ2TotC(InDetDD::PixelDiodeType::LONG, channelNumber, totC); // can not move as shared

        // Ganged pixel
        writeCdo -> setAnalogThreshold(InDetDD::PixelDiodeType::GANGED, channelNumber, std::move(analogThresholdGanged));
        writeCdo -> setAnalogThresholdSigma(InDetDD::PixelDiodeType::GANGED, channelNumber, std::move(analogThresholdSigmaGanged));
        writeCdo -> setAnalogThresholdNoise(InDetDD::PixelDiodeType::GANGED, channelNumber, std::move(analogThresholdNoiseGanged));
        writeCdo -> setInTimeThreshold(InDetDD::PixelDiodeType::GANGED, channelNumber, std::move(inTimeThresholdGanged));

        writeCdo -> setQ2TotA(InDetDD::PixelDiodeType::GANGED, channelNumber, std::move(totAGanged));
        writeCdo -> setQ2TotE(InDetDD::PixelDiodeType::GANGED, channelNumber, std::move(totEGanged));
        writeCdo -> setQ2TotC(InDetDD::PixelDiodeType::GANGED, channelNumber, std::move(totCGanged));
      } else {
        ATH_MSG_ERROR("payload[\"data\"] does not exist for ChanNum " << channelNumber);
        return StatusCode::FAILURE;
      }
    }
  }
  else {
    for (unsigned int moduleHash{}; moduleHash < m_pixelID->wafer_hash_max(); moduleHash++) {
      IdentifierHash wafer_hash = IdentifierHash(moduleHash);
      Identifier wafer_id = m_pixelID->wafer_id(wafer_hash);
      int barrel_ec = m_pixelID->barrel_ec(wafer_id);
      int layer     = m_pixelID->layer_disk(wafer_id);
      const InDetDD::SiDetectorElement *element = elements->getDetectorElement(wafer_hash);
      const InDetDD::PixelModuleDesign *p_design = static_cast<const InDetDD::PixelModuleDesign*>(&element->design());
      // in some cases numberOfCircuits returns FEs per half-module
      unsigned int numFE = p_design->numberOfCircuits() < halfModuleThreshold ? p_design->numberOfCircuits() : 2 * p_design->numberOfCircuits();

      for (InDetDD::PixelDiodeType type : diodeTypes) {
        writeCdo -> setAnalogThreshold(type, moduleHash, std::vector<int>(numFE, configData->getDefaultAnalogThreshold(barrel_ec, layer)));
        writeCdo -> setAnalogThresholdSigma(type, moduleHash, std::vector<int>(numFE, configData->getDefaultAnalogThresholdSigma(barrel_ec, layer)));
        writeCdo -> setAnalogThresholdNoise(type, moduleHash, std::vector<int>(numFE, configData->getDefaultAnalogThresholdNoise(barrel_ec, layer)));
        writeCdo -> setInTimeThreshold(type, moduleHash, std::vector<int>(numFE, configData->getDefaultInTimeThreshold(barrel_ec, layer)));

        writeCdo -> setQ2TotA(type, moduleHash, std::vector<float>(numFE, configData->getDefaultQ2TotA()));
        writeCdo -> setQ2TotE(type, moduleHash, std::vector<float>(numFE, configData->getDefaultQ2TotE()));
        writeCdo -> setQ2TotC(type, moduleHash, std::vector<float>(numFE, configData->getDefaultQ2TotC()));
      }

      writeCdo -> setTotRes1(moduleHash, std::vector<float>(numFE, 0.0));
      writeCdo -> setTotRes2(moduleHash, std::vector<float>(numFE, 0.0));
    }
  }

  // Scan over if the DB contents need to be overwritten.
  // This is useful for threshold study. So far only threshold value.
  for (unsigned int moduleHash{}; moduleHash < m_pixelID->wafer_hash_max(); moduleHash++) {
    IdentifierHash wafer_hash = IdentifierHash(moduleHash);
    Identifier wafer_id = m_pixelID->wafer_id(wafer_hash);
    int barrel_ec = m_pixelID->barrel_ec(wafer_id);
    int layer     = m_pixelID->layer_disk(wafer_id);
    const InDetDD::SiDetectorElement *element = elements->getDetectorElement(wafer_hash);
    const InDetDD::PixelModuleDesign *p_design = static_cast<const InDetDD::PixelModuleDesign*>(&element->design());
    // in some cases numberOfCircuits returns FEs per half-module
    unsigned int numFE = p_design->numberOfCircuits() < halfModuleThreshold ? p_design->numberOfCircuits() : 2 * p_design->numberOfCircuits();

    if (configData->getDefaultAnalogThreshold(barrel_ec, layer) > -0.1) {
      for (InDetDD::PixelDiodeType type : diodeTypes) {
        writeCdo -> setAnalogThreshold(type, moduleHash, std::vector<int>(numFE, configData->getDefaultAnalogThreshold(barrel_ec, layer)));
      }
    }
  }

  if (writeHandle.record(rangeW, std::move(writeCdo)).isFailure()) {
    ATH_MSG_FATAL("Could not record PixelChargeCalibCondData " << writeHandle.key() << " with EventRange " << rangeW << " into Conditions Store");
    return StatusCode::FAILURE;
  }
  ATH_MSG_INFO("recorded new CDO " << writeHandle.key() << " with range " << rangeW << " into Conditions Store");

  return StatusCode::SUCCESS;
}
