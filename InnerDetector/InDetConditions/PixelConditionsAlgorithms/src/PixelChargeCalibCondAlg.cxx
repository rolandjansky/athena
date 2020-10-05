/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "PixelChargeCalibCondAlg.h"
#include "Identifier/IdentifierHash.h"
#include "InDetReadoutGeometry/SiDetectorElement.h"
#include "PixelReadoutGeometry/PixelModuleDesign.h"
#include "GaudiKernel/EventIDRange.h"
#include <memory>
#include <sstream>

PixelChargeCalibCondAlg::PixelChargeCalibCondAlg(const std::string& name, ISvcLocator* pSvcLocator):
  ::AthReentrantAlgorithm(name, pSvcLocator)
{
}

StatusCode PixelChargeCalibCondAlg::initialize() {
  ATH_MSG_DEBUG("PixelChargeCalibCondAlg::initialize()");

  ATH_CHECK(detStore()->retrieve(m_pixelID,"PixelID"));
  ATH_CHECK(m_pixelDetEleCollKey.initialize());
  ATH_CHECK(m_condSvc.retrieve());
  ATH_CHECK(m_configKey.initialize());
  ATH_CHECK(m_readKey.initialize(SG::AllowEmpty));
  ATH_CHECK(m_writeKey.initialize());
  if (m_condSvc->regHandle(this,m_writeKey).isFailure()) {
    ATH_MSG_FATAL("unable to register WriteCondHandle " << m_writeKey.fullKey() << " with CondSvc");
    return StatusCode::FAILURE;
  }
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

  SG::ReadCondHandle<PixelModuleData> configData(m_configKey, ctx);

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
        while (std::getline(ss,buffer,'\n')) { component.push_back(buffer); }

        for (int i=1; i<(int)component.size(); i++) {
          std::stringstream checkFE(component[i]);
          std::vector<std::string> FEString;
          while (std::getline(checkFE,buffer,' ')) { FEString.push_back(buffer); }

          if (FEString.size()<21) {
            ATH_MSG_INFO("size of FEString is " << FEString.size() << " and is less than expected, 21.");
            ATH_MSG_INFO("This is the problem in the contents in conditions DB. This should rather be fixed in DB-side.");
            continue;
          }

          // Normal pixel
          writeCdo -> setAnalogThreshold((int)channelNumber, std::atoi(FEString[1].c_str()));
          writeCdo -> setAnalogThresholdSigma((int)channelNumber, std::atoi(FEString[2].c_str()));
          writeCdo -> setAnalogThresholdNoise((int)channelNumber, std::atoi(FEString[3].c_str()));
          writeCdo -> setInTimeThreshold((int)channelNumber, std::atoi(FEString[4].c_str()));

          writeCdo -> setQ2TotA((int)channelNumber, std::atof(FEString[13].c_str()));
          writeCdo -> setQ2TotE((int)channelNumber, std::atof(FEString[14].c_str()));
          writeCdo -> setQ2TotC((int)channelNumber, std::atof(FEString[15].c_str()));

          writeCdo -> setTotRes1((int)channelNumber, std::atof(FEString[19].c_str()));
          writeCdo -> setTotRes2((int)channelNumber, std::atof(FEString[20].c_str()));

          // Long pixel
          writeCdo -> setAnalogThresholdLong((int)channelNumber, std::atoi(FEString[5].c_str()));
          writeCdo -> setAnalogThresholdSigmaLong((int)channelNumber, std::atoi(FEString[6].c_str()));
          writeCdo -> setAnalogThresholdNoiseLong((int)channelNumber, std::atoi(FEString[7].c_str()));
          writeCdo -> setInTimeThresholdLong((int)channelNumber, std::atoi(FEString[8].c_str()));

          writeCdo -> setQ2TotALong((int)channelNumber, std::atof(FEString[16].c_str()));
          writeCdo -> setQ2TotELong((int)channelNumber, std::atof(FEString[17].c_str()));
          writeCdo -> setQ2TotCLong((int)channelNumber, std::atof(FEString[18].c_str()));

          // Ganged pixel
          writeCdo -> setAnalogThresholdGanged((int)channelNumber, std::atoi(FEString[9].c_str()));
          writeCdo -> setAnalogThresholdSigmaGanged((int)channelNumber, std::atoi(FEString[10].c_str()));
          writeCdo -> setAnalogThresholdNoiseGanged((int)channelNumber, std::atoi(FEString[11].c_str()));
          writeCdo -> setInTimeThresholdGanged((int)channelNumber, std::atoi(FEString[12].c_str()));
        }
      }
      else {
        ATH_MSG_WARNING("payload[\"data\"] does not exist for ChanNum " << channelNumber);
        IdentifierHash wafer_hash = IdentifierHash(channelNumber);
        Identifier wafer_id = m_pixelID->wafer_id(wafer_hash);
        int bec   = m_pixelID->barrel_ec(wafer_id);
        int layer = m_pixelID->layer_disk(wafer_id);
        const InDetDD::SiDetectorElement *element = elements->getDetectorElement(wafer_hash);
        const InDetDD::PixelModuleDesign *p_design = static_cast<const InDetDD::PixelModuleDesign*>(&element->design());
        int numFE = p_design->numberOfCircuits() < 8 ? p_design->numberOfCircuits() : 16;
        for (int j=0; j<numFE; j++) {
          writeCdo -> setAnalogThreshold((int)channelNumber,     configData->getDefaultAnalogThreshold(bec,layer));
          writeCdo -> setAnalogThresholdSigma((int)channelNumber,configData->getDefaultAnalogThresholdSigma(bec,layer));
          writeCdo -> setAnalogThresholdNoise((int)channelNumber,configData->getDefaultAnalogThresholdNoise(bec,layer));
          writeCdo -> setInTimeThreshold((int)channelNumber,     configData->getDefaultInTimeThreshold(bec,layer));
          writeCdo -> setQ2TotA((int)channelNumber, configData->getDefaultQ2TotA());
          writeCdo -> setQ2TotE((int)channelNumber, configData->getDefaultQ2TotE());
          writeCdo -> setQ2TotC((int)channelNumber, configData->getDefaultQ2TotC());
          writeCdo -> setTotRes1((int)channelNumber, 0.0);
          writeCdo -> setTotRes2((int)channelNumber, 0.0);

          // Long pixel
          writeCdo -> setAnalogThresholdLong((int)channelNumber,     configData->getDefaultAnalogThreshold(bec,layer));
          writeCdo -> setAnalogThresholdSigmaLong((int)channelNumber,configData->getDefaultAnalogThresholdSigma(bec,layer));
          writeCdo -> setAnalogThresholdNoiseLong((int)channelNumber,configData->getDefaultAnalogThresholdNoise(bec,layer));
          writeCdo -> setInTimeThresholdLong((int)channelNumber,     configData->getDefaultInTimeThreshold(bec,layer));

          writeCdo -> setQ2TotALong((int)channelNumber, configData->getDefaultQ2TotA());
          writeCdo -> setQ2TotELong((int)channelNumber, configData->getDefaultQ2TotE());
          writeCdo -> setQ2TotCLong((int)channelNumber, configData->getDefaultQ2TotC());

          // Ganged pixel
          writeCdo -> setAnalogThresholdGanged((int)channelNumber,     configData->getDefaultAnalogThreshold(bec,layer));
          writeCdo -> setAnalogThresholdSigmaGanged((int)channelNumber,configData->getDefaultAnalogThresholdSigma(bec,layer));
          writeCdo -> setAnalogThresholdNoiseGanged((int)channelNumber,configData->getDefaultAnalogThresholdNoise(bec,layer));
          writeCdo -> setInTimeThresholdGanged((int)channelNumber,     configData->getDefaultInTimeThreshold(bec,layer));
        }
      }
    }
  }
  else {
    for (int i=0; i<(int)m_pixelID->wafer_hash_max(); i++) {
      IdentifierHash wafer_hash = IdentifierHash(i);
      Identifier wafer_id = m_pixelID->wafer_id(wafer_hash);
      int bec   = m_pixelID->barrel_ec(wafer_id);
      int layer = m_pixelID->layer_disk(wafer_id);
      const InDetDD::SiDetectorElement *element = elements->getDetectorElement(wafer_hash);
      const InDetDD::PixelModuleDesign *p_design = static_cast<const InDetDD::PixelModuleDesign*>(&element->design());
      int numFE = p_design->numberOfCircuits() < 8 ? p_design->numberOfCircuits() : 16;
      for (int j=0; j<numFE; j++) {
        writeCdo -> setAnalogThreshold(i,     configData->getDefaultAnalogThreshold(bec,layer));
        writeCdo -> setAnalogThresholdSigma(i,configData->getDefaultAnalogThresholdSigma(bec,layer));
        writeCdo -> setAnalogThresholdNoise(i,configData->getDefaultAnalogThresholdNoise(bec,layer));
        writeCdo -> setInTimeThreshold(i,     configData->getDefaultInTimeThreshold(bec,layer));
        writeCdo -> setQ2TotA(i, configData->getDefaultQ2TotA());
        writeCdo -> setQ2TotE(i, configData->getDefaultQ2TotE());
        writeCdo -> setQ2TotC(i, configData->getDefaultQ2TotC());
        writeCdo -> setTotRes1(i, 0.0);
        writeCdo -> setTotRes2(i, 0.0);

        // Long pixel
        writeCdo -> setAnalogThresholdLong(i,     configData->getDefaultAnalogThreshold(bec,layer));
        writeCdo -> setAnalogThresholdSigmaLong(i,configData->getDefaultAnalogThresholdSigma(bec,layer));
        writeCdo -> setAnalogThresholdNoiseLong(i,configData->getDefaultAnalogThresholdNoise(bec,layer));
        writeCdo -> setInTimeThresholdLong(i,     configData->getDefaultInTimeThreshold(bec,layer));

        writeCdo -> setQ2TotALong(i, configData->getDefaultQ2TotA());
        writeCdo -> setQ2TotELong(i, configData->getDefaultQ2TotE());
        writeCdo -> setQ2TotCLong(i, configData->getDefaultQ2TotC());

        // Ganged pixel
        writeCdo -> setAnalogThresholdGanged(i,     configData->getDefaultAnalogThreshold(bec,layer));
        writeCdo -> setAnalogThresholdSigmaGanged(i,configData->getDefaultAnalogThresholdSigma(bec,layer));
        writeCdo -> setAnalogThresholdNoiseGanged(i,configData->getDefaultAnalogThresholdNoise(bec,layer));
        writeCdo -> setInTimeThresholdGanged(i,     configData->getDefaultInTimeThreshold(bec,layer));
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

