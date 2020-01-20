/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "PixelConfigCondAlg.h"
#include "Identifier/IdentifierHash.h"
#include "GaudiKernel/EventIDRange.h"
#include <memory>
#include <sstream>

PixelConfigCondAlg::PixelConfigCondAlg(const std::string& name, ISvcLocator* pSvcLocator):
  ::AthReentrantAlgorithm(name, pSvcLocator),
  m_IBLNoiseShape({0.0,1.0}),
  m_BLayerNoiseShape({0.0,1.0}),
  m_PixelNoiseShape({0.0,1.0})
{
  declareProperty("IBLNoiseShape",         m_IBLNoiseShape);       // This is ad-hoc solution.
  declareProperty("BLayerNoiseShape",      m_BLayerNoiseShape);
  declareProperty("PixelNoiseShape",       m_PixelNoiseShape);
}

StatusCode PixelConfigCondAlg::initialize() {
  ATH_MSG_DEBUG("PixelConfigCondAlg::initialize()");

  ATH_CHECK(m_ComTimeKey.initialize(m_UseComTime));
  if (m_UseComTime) {
    SG::ReadHandle<ComTime> comTime(m_ComTimeKey);
    if (comTime.isValid()) {
      m_ComTime = comTime->getTime();
      ATH_MSG_DEBUG("Found tool for cosmic/commissioning timing: ComTime");
    } 
    else {
      ATH_MSG_WARNING("Did not find tool needed for cosmic/commissioning timing: ComTime");
    }
  }

  ATH_CHECK(m_condSvc.retrieve());

  if (m_useDeadmapConditions) { ATH_CHECK(m_readDeadMapKey.initialize()); }

  ATH_CHECK(m_writeKey.initialize());
  if (m_condSvc->regHandle(this,m_writeKey).isFailure()) {
    ATH_MSG_FATAL("unable to register WriteCondHandle " << m_writeKey.fullKey() << " with CondSvc");
    return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;
}

StatusCode PixelConfigCondAlg::execute(const EventContext& ctx) const {
  ATH_MSG_DEBUG("PixelConfigCondAlg::execute()");

  SG::WriteCondHandle<PixelModuleData> writeHandle(m_writeKey, ctx);
  if (writeHandle.isValid()) {
    ATH_MSG_DEBUG("CondHandle " << writeHandle.fullKey() << " is already valid.. In theory this should not be called, but may happen if multiple concurrent events are being processed out of order.");
    return StatusCode::SUCCESS; 
  }

  // Construct the output Cond Object and fill it in
  std::unique_ptr<PixelModuleData> writeCdo(std::make_unique<PixelModuleData>());

  const EventIDBase start{EventIDBase::UNDEFNUM, EventIDBase::UNDEFEVT,                     0,                       
                                              0, EventIDBase::UNDEFNUM, EventIDBase::UNDEFNUM};
  const EventIDBase stop {EventIDBase::UNDEFNUM,   EventIDBase::UNDEFEVT, EventIDBase::UNDEFNUM-1, 
                          EventIDBase::UNDEFNUM-1, EventIDBase::UNDEFNUM, EventIDBase::UNDEFNUM};

  //==============
  // Set dead map
  //==============
  EventIDRange rangeDeadMap{start, stop};
  if (m_useDeadmapConditions) {
    SG::ReadCondHandle<CondAttrListCollection> readHandle(m_readDeadMapKey, ctx);
    const CondAttrListCollection* readCdo = *readHandle; 
    if (readCdo==nullptr) {
      ATH_MSG_FATAL("Null pointer to the read conditions object");
      return StatusCode::FAILURE;
    }
    // Get the validitiy range
    if (not readHandle.range(rangeDeadMap)) {
      ATH_MSG_FATAL("Failed to retrieve validity range for " << readHandle.key());
      return StatusCode::FAILURE;
    }
    ATH_MSG_INFO("Size of CondAttrListCollection " << readHandle.fullKey() << " readCdo->size()= " << readCdo->size());
    ATH_MSG_INFO("Range of input is " << rangeDeadMap);

    // Read dead map info
    for (CondAttrListCollection::const_iterator attrList=readCdo->begin(); attrList!=readCdo->end(); ++attrList) {
      const CondAttrListCollection::ChanNum &channelNumber = attrList->first;
      const CondAttrListCollection::AttributeList &payload = attrList->second;

      // RUN-1, RUN-2 format
      if (payload.exists("moduleID") and not payload["moduleID"].isNull() && payload.exists("ModuleSpecialPixelMap_Clob") and not payload["ModuleSpecialPixelMap_Clob"].isNull()) {

        int moduleHash = payload["moduleID"].data<int>();
        const std::string &stringStatus = payload["ModuleSpecialPixelMap_Clob"].data<std::string>();

        std::stringstream ss(stringStatus);
        std::vector<std::string> moduleStringStatus;
        std::string buffer;
        while (std::getline(ss,buffer,' ')) { moduleStringStatus.push_back(buffer); }

        if (moduleStringStatus.size()<2) {
          ATH_MSG_FATAL("Not enough moduleStringStatus data " << moduleStringStatus.size() << " < 2 for channel " <<  channelNumber << " read from " << readHandle.fullKey());
          return StatusCode::FAILURE;
        }

        int moduleStatus = std::atoi(moduleStringStatus[0].c_str());
        int chipStatus   = std::atoi(moduleStringStatus[1].c_str());

        if (moduleStatus>0) { writeCdo->setModuleStatus(moduleHash, moduleStatus); }
        if (chipStatus>0)   { writeCdo->setChipStatus(moduleHash, chipStatus); }
      } 
      else {
        ATH_MSG_WARNING("Can not retrieve " << channelNumber);
      }
    }
  }

  // Switch parameters
  writeCdo -> setUseCalibConditions(m_useCalibConditions);
  writeCdo -> setUseDeadmapConditions(m_useDeadmapConditions);
  writeCdo -> setUseDCSStateConditions(m_useDCSStateConditions);
  writeCdo -> setUseDCSStatusConditions(m_useDCSStatusConditions);
  writeCdo -> setUseDCSHVConditions(m_useDCSHVConditions);
  writeCdo -> setUseDCSTemperatureConditions(m_useDCSTemperatureConditions);
  writeCdo -> setUseTDAQConditions(m_useTDAQConditions);
  writeCdo -> setUseCablingConditions(m_useCablingConditions);

  // Digitization parameters
  writeCdo -> setBunchSpace(m_bunchSpace);
  writeCdo -> setUseComTime(m_UseComTime);
  writeCdo -> setComTime(m_ComTime);
  writeCdo -> setBarrelNumberOfBCID(m_BarrelNumberOfBCID);
  writeCdo -> setEndcapNumberOfBCID(m_EndcapNumberOfBCID);
  writeCdo -> setDBMNumberOfBCID(m_DBMNumberOfBCID);
  writeCdo -> setBarrelTimeOffset(m_BarrelTimeOffset);
  writeCdo -> setEndcapTimeOffset(m_EndcapTimeOffset);
  writeCdo -> setDBMTimeOffset(m_DBMTimeOffset);
  writeCdo -> setBarrelTimeJitter(m_BarrelTimeJitter);
  writeCdo -> setEndcapTimeJitter(m_EndcapTimeJitter);
  writeCdo -> setDBMTimeJitter(m_DBMTimeJitter);
  writeCdo -> setDefaultBarrelAnalogThreshold(m_BarrelAnalogThreshold);
  writeCdo -> setDefaultEndcapAnalogThreshold(m_EndcapAnalogThreshold);
  writeCdo -> setDefaultDBMAnalogThreshold(m_DBMAnalogThreshold);
  writeCdo -> setDefaultBarrelAnalogThresholdSigma(m_BarrelAnalogThresholdSigma);
  writeCdo -> setDefaultEndcapAnalogThresholdSigma(m_EndcapAnalogThresholdSigma);
  writeCdo -> setDefaultDBMAnalogThresholdSigma(m_DBMAnalogThresholdSigma);
  writeCdo -> setDefaultBarrelAnalogThresholdNoise(m_BarrelAnalogThresholdNoise);
  writeCdo -> setDefaultEndcapAnalogThresholdNoise(m_EndcapAnalogThresholdNoise);
  writeCdo -> setDefaultDBMAnalogThresholdNoise(m_DBMAnalogThresholdNoise);
  writeCdo -> setDefaultBarrelInTimeThreshold(m_BarrelInTimeThreshold);
  writeCdo -> setDefaultEndcapInTimeThreshold(m_EndcapInTimeThreshold);
  writeCdo -> setDefaultDBMInTimeThreshold(m_DBMInTimeThreshold);
  writeCdo -> setBarrelToTThreshold(m_BarrelToTThreshold);
  writeCdo -> setEndcapToTThreshold(m_EndcapToTThreshold);
  writeCdo -> setDBMToTThreshold(m_DBMToTThreshold);
  writeCdo -> setBarrelCrossTalk(m_BarrelCrossTalk);
  writeCdo -> setEndcapCrossTalk(m_EndcapCrossTalk);
  writeCdo -> setDBMCrossTalk(m_DBMCrossTalk);
  writeCdo -> setBarrelThermalNoise(m_BarrelThermalNoise);
  writeCdo -> setEndcapThermalNoise(m_EndcapThermalNoise);
  writeCdo -> setDBMThermalNoise(m_DBMThermalNoise);
  writeCdo -> setBarrelNoiseOccupancy(m_BarrelNoiseOccupancy);
  writeCdo -> setEndcapNoiseOccupancy(m_EndcapNoiseOccupancy);
  writeCdo -> setDBMNoiseOccupancy(m_DBMNoiseOccupancy);
  writeCdo -> setBarrelDisableProbability(m_BarrelDisableProbability);
  writeCdo -> setEndcapDisableProbability(m_EndcapDisableProbability);
  writeCdo -> setDBMDisableProbability(m_DBMDisableProbability);

  for (size_t i=0; i<m_IBLNoiseShape.size(); i++)    { writeCdo->setBarrelNoiseShape(0,m_IBLNoiseShape.at(i)); }
  for (size_t i=0; i<m_BLayerNoiseShape.size(); i++) { writeCdo->setBarrelNoiseShape(1,m_BLayerNoiseShape.at(i)); }
  for (size_t i=0; i<m_PixelNoiseShape.size(); i++)  { writeCdo->setBarrelNoiseShape(2,m_PixelNoiseShape.at(i)); }
  for (size_t i=0; i<m_PixelNoiseShape.size(); i++)  { writeCdo->setBarrelNoiseShape(3,m_PixelNoiseShape.at(i)); }

  for (size_t i=0; i<m_PixelNoiseShape.size(); i++)  { writeCdo->setEndcapNoiseShape(0,m_PixelNoiseShape.at(i)); }
  for (size_t i=0; i<m_PixelNoiseShape.size(); i++)  { writeCdo->setEndcapNoiseShape(1,m_PixelNoiseShape.at(i)); }
  for (size_t i=0; i<m_PixelNoiseShape.size(); i++)  { writeCdo->setEndcapNoiseShape(2,m_PixelNoiseShape.at(i)); }

  for (size_t i=0; i<m_IBLNoiseShape.size(); i++)    { writeCdo->setDBMNoiseShape(0,m_IBLNoiseShape.at(i)); }
  for (size_t i=0; i<m_IBLNoiseShape.size(); i++)    { writeCdo->setDBMNoiseShape(1,m_IBLNoiseShape.at(i)); }
  for (size_t i=0; i<m_IBLNoiseShape.size(); i++)    { writeCdo->setDBMNoiseShape(2,m_IBLNoiseShape.at(i)); }

/*
  for (size_t i=0; i<m_BarrelNoiseShape.size(); i++) {
    for (size_t j=0; j<m_BarrelNoiseShape[i].size(); j++) {
      writeCdo -> setBarrelNoiseShape(i,m_BarrelNoiseShape[i][j]);
    }
  }
  for (size_t i=0; i<m_EndcapNoiseShape.size(); i++) {
    for (size_t j=0; j<m_EndcapNoiseShape[i].size(); j++) {
      writeCdo -> setEndcapNoiseShape(i,m_EndcapNoiseShape[i][j]);
    }
  }
  for (size_t i=0; i<m_DBMNoiseShape.size(); i++) {
    for (size_t j=0; j<m_DBMNoiseShape[i].size(); j++) {
      writeCdo -> setDBMNoiseShape(i,m_DBMNoiseShape[i][j]);
    }
  }
*/

  writeCdo -> setFEI3BarrelLatency(m_FEI3BarrelLatency);
  writeCdo -> setFEI3EndcapLatency(m_FEI3EndcapLatency);
  writeCdo -> setFEI3BarrelHitDuplication(m_FEI3BarrelHitDuplication);
  writeCdo -> setFEI3EndcapHitDuplication(m_FEI3EndcapHitDuplication);
  writeCdo -> setFEI3BarrelSmallHitToT(m_FEI3BarrelSmallHitToT);
  writeCdo -> setFEI3EndcapSmallHitToT(m_FEI3EndcapSmallHitToT);
  writeCdo -> setFEI3BarrelTimingSimTune(m_FEI3BarrelTimingSimTune);
  writeCdo -> setFEI3EndcapTimingSimTune(m_FEI3EndcapTimingSimTune);
  writeCdo -> setFEI4BarrelHitDiscConfig(m_FEI4BarrelHitDiscConfig);
  writeCdo -> setFEI4EndcapHitDiscConfig(m_FEI4EndcapHitDiscConfig);

  // Charge calibration parameters
  writeCdo -> setDefaultQ2TotA(m_CalibrationParameterA);
  writeCdo -> setDefaultQ2TotE(m_CalibrationParameterE);
  writeCdo -> setDefaultQ2TotC(m_CalibrationParameterC);

  // DCS parameters
  writeCdo -> setDefaultBiasVoltage(m_biasVoltage);
  writeCdo -> setDefaultTemperature(m_temperature);

  // Cabling parameters
  writeCdo -> setCablingMapToFile(m_cablingMapToFile);
  writeCdo -> setCablingMapFileName(m_cablingMapFileName);

  // Distortion parameters
  writeCdo -> setDistortionInputSource(m_distortionInputSource);
  writeCdo -> setDistortionVersion(m_distortionVersion);
  writeCdo -> setDistortionR1(m_distortionR1);
  writeCdo -> setDistortionR2(m_distortionR2);
  writeCdo -> setDistortionTwist(m_distortionTwist);
  writeCdo -> setDistortionMeanR(m_distortionMeanR);
  writeCdo -> setDistortionRMSR(m_distortionRMSR);
  writeCdo -> setDistortionMeanTwist(m_distortionMeanTwist);
  writeCdo -> setDistortionRMSTwist(m_distortionRMSTwist);
  writeCdo -> setDistortionWriteToFile(m_distortionWriteToFile);
  writeCdo -> setDistortionFileName(m_distortionFileName);

  //=======================
  // Combine time interval
  //=======================
  EventIDRange rangeW{start, stop};
  rangeW = rangeDeadMap;

  if (rangeW.stop().isValid() and rangeW.start()>rangeW.stop()) {
    ATH_MSG_FATAL("Invalid intersection rangeW: " << rangeW);
    return StatusCode::FAILURE;
  }

  if (writeHandle.record(rangeW, std::move(writeCdo)).isFailure()) {
    ATH_MSG_FATAL("Could not record PixelModuleData " << writeHandle.key() << " with EventRange " << rangeW << " into Conditions Store");
    return StatusCode::FAILURE;
  }
  ATH_MSG_INFO("recorded new CDO " << writeHandle.key() << " with range " << rangeW << " into Conditions Store");

  return StatusCode::SUCCESS;
}

