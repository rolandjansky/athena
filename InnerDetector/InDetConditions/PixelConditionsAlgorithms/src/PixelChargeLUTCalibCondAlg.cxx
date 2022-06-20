/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "GaudiKernel/EventIDRange.h"
#include "Identifier/IdentifierHash.h"
#include "InDetReadoutGeometry/SiDetectorElement.h"
#include "PixelChargeLUTCalibCondAlg.h"
#include "PixelReadoutGeometry/PixelModuleDesign.h"

#include <nlohmann/json.hpp>

#include <memory>
#include <sstream>

namespace
{

constexpr int halfModuleThreshold{8};
constexpr size_t FEStringSize{20};

} // namespace


PixelChargeLUTCalibCondAlg::PixelChargeLUTCalibCondAlg(const std::string& name, ISvcLocator* pSvcLocator):
  ::AthReentrantAlgorithm(name, pSvcLocator)
{
}

StatusCode PixelChargeLUTCalibCondAlg::initialize() {
  ATH_MSG_DEBUG("PixelChargeLUTCalibCondAlg::initialize()");

  ATH_CHECK(detStore()->retrieve(m_pixelID,"PixelID"));
  ATH_CHECK(m_pixelDetEleCollKey.initialize());
  ATH_CHECK(m_configKey.initialize());
  ATH_CHECK(m_readKey.initialize(SG::AllowEmpty));
  ATH_CHECK(m_writeKey.initialize());

  return StatusCode::SUCCESS;
}

StatusCode PixelChargeLUTCalibCondAlg::execute(const EventContext& ctx) const {
  ATH_MSG_DEBUG("PixelChargeLUTCalibCondAlg::execute()");

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

  SG::ReadCondHandle<PixelModuleData> configDataHandle(m_configKey, ctx);
  const PixelModuleData *configData = *configDataHandle;

  const std::array<InDetDD::PixelDiodeType, 4> diodeTypes
    = {InDetDD::PixelDiodeType::NORMAL, InDetDD::PixelDiodeType::LONG, InDetDD::PixelDiodeType::GANGED, InDetDD::PixelDiodeType::LARGE};

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
      const CondAttrListCollection::AttributeList &payload = attrList->second;

      // RUN-3 format
      if (payload.exists("data_array") and not payload["data_array"].isNull()) {
        const nlohmann::json jsonData = nlohmann::json::parse(payload["data_array"].data<std::string>());

        for (const auto &[hash, data] : jsonData.items()) {
          const unsigned int moduleHash = std::stoul(hash);
          IdentifierHash wafer_hash = IdentifierHash(moduleHash);
          const InDetDD::SiDetectorElement *element = elements->getDetectorElement(wafer_hash);
          const InDetDD::PixelModuleDesign *p_design = static_cast<const InDetDD::PixelModuleDesign*>(&element->design());
          // in some cases numberOfCircuits returns FEs per half-module
          unsigned int numFE = p_design->numberOfCircuits() < halfModuleThreshold ? p_design->numberOfCircuits() : 2 * p_design->numberOfCircuits();

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

          PixelChargeCalibCondData::IBLModule tot2Charges;

          analogThreshold.reserve(numFE);
          analogThresholdSigma.reserve(numFE);
          analogThresholdNoise.reserve(numFE);
          inTimeThreshold.reserve(numFE);

          analogThresholdLong.reserve(numFE);
          analogThresholdSigmaLong.reserve(numFE);
          analogThresholdNoiseLong.reserve(numFE);
          inTimeThresholdLong.reserve(numFE);

          analogThresholdGanged.reserve(numFE);
          analogThresholdSigmaGanged.reserve(numFE);
          analogThresholdNoiseGanged.reserve(numFE);
          inTimeThresholdGanged.reserve(numFE);

          totA.reserve(numFE);
          totE.reserve(numFE);
          totC.reserve(numFE);

          totAGanged.reserve(numFE);
          totEGanged.reserve(numFE);
          totCGanged.reserve(numFE);

          totRes1.reserve(numFE);
          totRes2.reserve(numFE);

          tot2Charges.reserve(numFE);

          for (unsigned int j{}; j < numFE; j++) {
            const auto &calibArray = data.at(j);
            if (!calibArray.empty()) {
              // new charge calibration for RUN-3
              if (p_design->getReadoutTechnology() == InDetDD::PixelReadoutTechnology::FEI4 && !(element->isDBM())) {
                if (calibArray.size() != FEStringSize) {
                  ATH_MSG_FATAL("Parameter size is not consistent(" << FEStringSize << ") " << calibArray.size() << " at (i,j)=(" <<  moduleHash << "," << j << ")");
                  return StatusCode::FAILURE;
                }

                auto &charges = tot2Charges.emplace_back(PixelChargeCalibCondData::IBLCalibration());
                for (size_t k{}; k < PixelChargeCalibCondData::IBLCalibrationSize; k++) {
                  charges[k] = calibArray[k + 4].get<float>();
                }
                writeCdo -> setCalibrationStrategy(moduleHash, PixelChargeCalibCondData::CalibrationStrategy::LUTFEI4);

                analogThreshold.push_back(calibArray[0].get<int>());
                analogThresholdSigma.push_back(0); // TODO: avoid this in clients
                analogThresholdNoise.push_back(calibArray[1].get<int>());

                analogThresholdLong.push_back(calibArray[2].get<int>());
                analogThresholdSigmaLong.push_back(0); // TODO: avoid this in clients
                analogThresholdNoiseLong.push_back(calibArray[3].get<int>());

                analogThresholdGanged.push_back(calibArray[2].get<int>());
                analogThresholdSigmaGanged.push_back(0); // TODO: avoid this in clients
                analogThresholdNoiseGanged.push_back(calibArray[3].get<int>());

                totA.push_back(0.0);
                totE.push_back(0.0);
                totC.push_back(0.0);

                totRes1.push_back(0.0); // TODO: avoid this in clients
                totRes2.push_back(0.0); // TODO: avoid this in clients
              }
              // conventional calibration
              else {
                if (calibArray.size() != FEStringSize) {
                  ATH_MSG_FATAL("Parameter size is not consistent(" << FEStringSize << ") " << calibArray.size() << " at (i,j)=(" << moduleHash << "," << j << ")");
                  return StatusCode::FAILURE;
                }

                analogThreshold.push_back(calibArray[0].get<int>());
                analogThresholdSigma.push_back(calibArray[1].get<int>());
                analogThresholdNoise.push_back(calibArray[2].get<int>());
                inTimeThreshold.push_back(calibArray[3].get<int>());

                analogThresholdLong.push_back(calibArray[4].get<int>());
                analogThresholdSigmaLong.push_back(calibArray[5].get<int>());
                analogThresholdNoiseLong.push_back(calibArray[6].get<int>());
                inTimeThresholdLong.push_back(calibArray[7].get<int>());

                analogThresholdGanged.push_back(calibArray[8].get<int>());
                analogThresholdSigmaGanged.push_back(calibArray[9].get<int>());
                analogThresholdNoiseGanged.push_back(calibArray[10].get<int>());
                inTimeThresholdGanged.push_back(calibArray[11].get<int>());

                totA.push_back(calibArray[12].get<float>());
                totE.push_back(calibArray[13].get<float>());
                totC.push_back(calibArray[14].get<float>());

                totAGanged.push_back(calibArray[15].get<float>());
                totEGanged.push_back(calibArray[16].get<float>());
                totCGanged.push_back(calibArray[17].get<float>());

                totRes1.push_back(calibArray[18].get<float>());
                totRes2.push_back(calibArray[19].get<float>());
              }
            }
            else {
              ATH_MSG_FATAL("Array size is zero in " << calibArray << " at (i,j)=(" << moduleHash << "," << j << ")");
              return StatusCode::FAILURE;
            }
          }

          // Special calibration
          if (!tot2Charges.empty()) {
            writeCdo -> setTot2Charges(moduleHash, tot2Charges);
          }

          // Normal pixel
          writeCdo -> setAnalogThreshold(InDetDD::PixelDiodeType::NORMAL, moduleHash, std::move(analogThreshold));
          writeCdo -> setAnalogThresholdSigma(InDetDD::PixelDiodeType::NORMAL, moduleHash, std::move(analogThresholdSigma));
          writeCdo -> setAnalogThresholdNoise(InDetDD::PixelDiodeType::NORMAL, moduleHash, std::move(analogThresholdNoise));
          writeCdo -> setInTimeThreshold(InDetDD::PixelDiodeType::NORMAL, moduleHash, std::move(inTimeThreshold));

          writeCdo -> setQ2TotA(InDetDD::PixelDiodeType::NORMAL, moduleHash, totA); // can not move as shared
          writeCdo -> setQ2TotE(InDetDD::PixelDiodeType::NORMAL, moduleHash, totE); // can not move as shared
          writeCdo -> setQ2TotC(InDetDD::PixelDiodeType::NORMAL, moduleHash, totC); // can not move as shared

          writeCdo -> setTotRes1(moduleHash, std::move(totRes1));
          writeCdo -> setTotRes2(moduleHash, std::move(totRes2));

          // Long pixel
          writeCdo -> setAnalogThreshold(InDetDD::PixelDiodeType::LONG, moduleHash, std::move(analogThresholdLong));
          writeCdo -> setAnalogThresholdSigma(InDetDD::PixelDiodeType::LONG, moduleHash, std::move(analogThresholdSigmaLong));
          writeCdo -> setAnalogThresholdNoise(InDetDD::PixelDiodeType::LONG, moduleHash, std::move(analogThresholdNoiseLong));
          writeCdo -> setInTimeThreshold(InDetDD::PixelDiodeType::LONG, moduleHash, std::move(inTimeThresholdLong));

          writeCdo -> setQ2TotA(InDetDD::PixelDiodeType::LONG, moduleHash, totA); // can not move as shared
          writeCdo -> setQ2TotE(InDetDD::PixelDiodeType::LONG, moduleHash, totE); // can not move as shared
          writeCdo -> setQ2TotC(InDetDD::PixelDiodeType::LONG, moduleHash, totC); // can not move as shared

          // Ganged/large pixel
          if (p_design->getReadoutTechnology() == InDetDD::PixelReadoutTechnology::RD53) {
            writeCdo -> setAnalogThreshold(InDetDD::PixelDiodeType::LARGE, moduleHash, std::move(analogThresholdGanged));
            writeCdo -> setAnalogThresholdSigma(InDetDD::PixelDiodeType::LARGE, moduleHash, std::move(analogThresholdSigmaGanged));
            writeCdo -> setAnalogThresholdNoise(InDetDD::PixelDiodeType::LARGE, moduleHash, std::move(analogThresholdNoiseGanged));
            writeCdo -> setInTimeThreshold(InDetDD::PixelDiodeType::LARGE, moduleHash, std::move(inTimeThresholdGanged));

            writeCdo -> setQ2TotA(InDetDD::PixelDiodeType::LARGE, moduleHash, std::move(totAGanged));
            writeCdo -> setQ2TotE(InDetDD::PixelDiodeType::LARGE, moduleHash, std::move(totEGanged));
            writeCdo -> setQ2TotC(InDetDD::PixelDiodeType::LARGE, moduleHash, std::move(totCGanged));
          } else {
            writeCdo -> setAnalogThreshold(InDetDD::PixelDiodeType::GANGED, moduleHash, std::move(analogThresholdGanged));
            writeCdo -> setAnalogThresholdSigma(InDetDD::PixelDiodeType::GANGED, moduleHash, std::move(analogThresholdSigmaGanged));
            writeCdo -> setAnalogThresholdNoise(InDetDD::PixelDiodeType::GANGED, moduleHash, std::move(analogThresholdNoiseGanged));
            writeCdo -> setInTimeThreshold(InDetDD::PixelDiodeType::GANGED, moduleHash, std::move(inTimeThresholdGanged));

            writeCdo -> setQ2TotA(InDetDD::PixelDiodeType::GANGED, moduleHash, std::move(totAGanged));
            writeCdo -> setQ2TotE(InDetDD::PixelDiodeType::GANGED, moduleHash, std::move(totEGanged));
            writeCdo -> setQ2TotC(InDetDD::PixelDiodeType::GANGED, moduleHash, std::move(totCGanged));
          }
        }
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
