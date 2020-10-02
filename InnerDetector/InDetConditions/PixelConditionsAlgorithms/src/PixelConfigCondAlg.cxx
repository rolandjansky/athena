/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "PixelConfigCondAlg.h"
#include "Identifier/IdentifierHash.h"
#include "GaudiKernel/EventIDRange.h"
#include <memory>
#include <sstream>

PixelConfigCondAlg::PixelConfigCondAlg(const std::string& name, ISvcLocator* pSvcLocator):
  ::AthReentrantAlgorithm(name, pSvcLocator)
{
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
  ATH_CHECK(m_readDeadMapKey.initialize(SG::AllowEmpty));
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
  if (!m_readDeadMapKey.empty()) {
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
  writeCdo -> setBarrelThermalNoise(m_BarrelThermalNoise);
  writeCdo -> setEndcapThermalNoise(m_EndcapThermalNoise);
  writeCdo -> setDBMThermalNoise(m_DBMThermalNoise);
  writeCdo -> setFEI4BarrelHitDiscConfig(m_FEI4BarrelHitDiscConfig);
  writeCdo -> setFEI4EndcapHitDiscConfig(m_FEI4EndcapHitDiscConfig);

  // Charge calibration parameters
  writeCdo -> setDefaultQ2TotA(m_CalibrationParameterA);
  writeCdo -> setDefaultQ2TotE(m_CalibrationParameterE);
  writeCdo -> setDefaultQ2TotC(m_CalibrationParameterC);

  // DCS parameters
  writeCdo -> setDefaultBiasVoltage(m_biasVoltage);
  writeCdo -> setDefaultTemperature(m_temperature);

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

  // Cabling parameters
  writeCdo -> setCablingMapToFile(m_cablingMapToFile);
  writeCdo -> setCablingMapFileName(m_cablingMapFileName);

  int currentRunNumber = ctx.eventID().run_number();
  if (currentRunNumber<222222) {
    writeCdo -> setBarrelToTThreshold(m_BarrelToTThresholdRUN1);
    writeCdo -> setFEI3BarrelLatency(m_FEI3BarrelLatencyRUN1);
    writeCdo -> setFEI3BarrelHitDuplication(m_FEI3BarrelHitDuplicationRUN1);
    writeCdo -> setFEI3BarrelSmallHitToT(m_FEI3BarrelSmallHitToTRUN1);
    writeCdo -> setFEI3BarrelTimingSimTune(m_FEI3BarrelTimingSimTuneRUN1);
    writeCdo -> setBarrelCrossTalk(m_BarrelCrossTalkRUN1);
    writeCdo -> setBarrelNoiseOccupancy(m_BarrelNoiseOccupancyRUN1);
    writeCdo -> setBarrelDisableProbability(m_BarrelDisableProbabilityRUN1);

    writeCdo -> setEndcapToTThreshold(m_EndcapToTThresholdRUN1);
    writeCdo -> setFEI3EndcapLatency(m_FEI3EndcapLatencyRUN1);
    writeCdo -> setFEI3EndcapHitDuplication(m_FEI3EndcapHitDuplicationRUN1);
    writeCdo -> setFEI3EndcapSmallHitToT(m_FEI3EndcapSmallHitToTRUN1);
    writeCdo -> setFEI3EndcapTimingSimTune(m_FEI3EndcapTimingSimTuneRUN1);
    writeCdo -> setEndcapCrossTalk(m_EndcapCrossTalkRUN1);
    writeCdo -> setEndcapNoiseOccupancy(m_EndcapNoiseOccupancyRUN1);
    writeCdo -> setEndcapDisableProbability(m_EndcapDisableProbabilityRUN1);

    // This is ad-hoc solution.
    for (size_t i=0; i<m_BLayerNoiseShapeRUN1.size(); i++) { writeCdo->setBarrelNoiseShape(0,m_BLayerNoiseShapeRUN1[i]); }
    for (size_t i=0; i<m_PixelNoiseShapeRUN1.size(); i++)  {
      for (size_t layer:{1,2}) { writeCdo->setBarrelNoiseShape(layer,m_PixelNoiseShapeRUN1[i]); }
    }

    for (size_t i=0; i<m_PixelNoiseShapeRUN1.size(); i++)  {
      for (size_t layer:{0,1,2}) { writeCdo->setEndcapNoiseShape(layer,m_PixelNoiseShapeRUN1[i]); }
    }
  }
  else if (currentRunNumber<240000) {  // RUN2
    writeCdo -> setBarrelToTThreshold(m_BarrelToTThreshold2016);
    writeCdo -> setFEI3BarrelLatency(m_FEI3BarrelLatency2016);
    writeCdo -> setFEI3BarrelHitDuplication(m_FEI3BarrelHitDuplication2016);
    writeCdo -> setFEI3BarrelSmallHitToT(m_FEI3BarrelSmallHitToT2016);
    writeCdo -> setFEI3BarrelTimingSimTune(m_FEI3BarrelTimingSimTune2016);
    writeCdo -> setBarrelCrossTalk(m_BarrelCrossTalk2016);
    writeCdo -> setBarrelNoiseOccupancy(m_BarrelNoiseOccupancy2016);
    writeCdo -> setBarrelDisableProbability(m_BarrelDisableProbability2016);

    writeCdo -> setEndcapToTThreshold(m_EndcapToTThreshold2016);
    writeCdo -> setFEI3EndcapLatency(m_FEI3EndcapLatency2016);
    writeCdo -> setFEI3EndcapHitDuplication(m_FEI3EndcapHitDuplication2016);
    writeCdo -> setFEI3EndcapSmallHitToT(m_FEI3EndcapSmallHitToT2016);
    writeCdo -> setFEI3EndcapTimingSimTune(m_FEI3EndcapTimingSimTune2016);
    writeCdo -> setEndcapCrossTalk(m_EndcapCrossTalk2016);
    writeCdo -> setEndcapNoiseOccupancy(m_EndcapNoiseOccupancy2016);
    writeCdo -> setEndcapDisableProbability(m_EndcapDisableProbability2016);

    writeCdo -> setDBMToTThreshold(m_DBMToTThreshold2016);
    writeCdo -> setDBMCrossTalk(m_DBMCrossTalk2016);
    writeCdo -> setDBMNoiseOccupancy(m_DBMNoiseOccupancy2016);
    writeCdo -> setDBMDisableProbability(m_DBMDisableProbability2016);

    /* 
       So far, Gaudi::Property does not support 2D vector.
       for (size_t i=0; i<m_BarrelNoiseShape.size(); i++) {
       for (size_t j=0; j<m_BarrelNoiseShape[i].size(); j++) {
       writeCdo -> setBarrelNoiseShape(i,m_BarrelNoiseShape[i][j]);
       }
       }
     */

    // This is ad-hoc solution.
    for (size_t i=0; i<m_IBLNoiseShape2016.size(); i++)    { writeCdo->setBarrelNoiseShape(0,m_IBLNoiseShape2016[i]); }
    for (size_t i=0; i<m_BLayerNoiseShape2016.size(); i++) { writeCdo->setBarrelNoiseShape(1,m_BLayerNoiseShape2016[i]); }
    for (size_t i=0; i<m_PixelNoiseShape2016.size(); i++)  {
      for (size_t layer:{2,3}) { writeCdo->setBarrelNoiseShape(layer,m_PixelNoiseShape2016[i]); }
    }

    for (size_t i=0; i<m_PixelNoiseShape2016.size(); i++)  {
      for (size_t layer:{0,1,2}) { writeCdo->setEndcapNoiseShape(layer,m_PixelNoiseShape2016[i]); }
    }

    for (size_t i=0; i<m_IBLNoiseShape2016.size(); i++)    {
      for (size_t layer:{0,1,2}) { writeCdo->setDBMNoiseShape(layer,m_IBLNoiseShape2016[i]); }
    }
  }
  else if (currentRunNumber<250000) {  // RUN4
    writeCdo -> setBarrelToTThreshold(m_BarrelToTThresholdITK);
    writeCdo -> setBarrelCrossTalk(m_BarrelCrossTalkITK);
    writeCdo -> setBarrelNoiseOccupancy(m_BarrelNoiseOccupancyITK);
    writeCdo -> setBarrelDisableProbability(m_BarrelDisableProbabilityITK);

    writeCdo -> setEndcapToTThreshold(m_EndcapToTThresholdITK);
    writeCdo -> setEndcapCrossTalk(m_EndcapCrossTalkITK);
    writeCdo -> setEndcapNoiseOccupancy(m_EndcapNoiseOccupancyITK);
    writeCdo -> setEndcapDisableProbability(m_EndcapDisableProbabilityITK);

    // This is ad-hoc solution.
    for (size_t i=0; i<m_InnermostNoiseShapeITK.size(); i++)     { writeCdo->setBarrelNoiseShape(0,m_InnermostNoiseShapeITK[i]); }
    for (size_t i=0; i<m_NextInnermostNoiseShapeITK.size(); i++) { writeCdo->setBarrelNoiseShape(1,m_NextInnermostNoiseShapeITK[i]); }
    for (size_t i=0; i<m_PixelNoiseShapeITK.size(); i++)  {
      for (size_t layer:{2,3,4}) { writeCdo->setBarrelNoiseShape(layer,m_PixelNoiseShapeITK[i]); }
    }

    for (size_t i=0; i<m_EndcapToTThresholdITK.size(); i++) {
      for (size_t j=0; j<m_PixelNoiseShapeITK.size(); j++)  { writeCdo->setEndcapNoiseShape(i,m_PixelNoiseShapeITK[j]); }
    }
  }
  else if (currentRunNumber<300000) {  // RUN2 2015/2016
    writeCdo -> setBarrelToTThreshold(m_BarrelToTThreshold2016);
    writeCdo -> setFEI3BarrelLatency(m_FEI3BarrelLatency2016);
    writeCdo -> setFEI3BarrelHitDuplication(m_FEI3BarrelHitDuplication2016);
    writeCdo -> setFEI3BarrelSmallHitToT(m_FEI3BarrelSmallHitToT2016);
    writeCdo -> setFEI3BarrelTimingSimTune(m_FEI3BarrelTimingSimTune2016);
    writeCdo -> setBarrelCrossTalk(m_BarrelCrossTalk2016);
    writeCdo -> setBarrelNoiseOccupancy(m_BarrelNoiseOccupancy2016);
    writeCdo -> setBarrelDisableProbability(m_BarrelDisableProbability2016);

    writeCdo -> setEndcapToTThreshold(m_EndcapToTThreshold2016);
    writeCdo -> setFEI3EndcapLatency(m_FEI3EndcapLatency2016);
    writeCdo -> setFEI3EndcapHitDuplication(m_FEI3EndcapHitDuplication2016);
    writeCdo -> setFEI3EndcapSmallHitToT(m_FEI3EndcapSmallHitToT2016);
    writeCdo -> setFEI3EndcapTimingSimTune(m_FEI3EndcapTimingSimTune2016);
    writeCdo -> setEndcapCrossTalk(m_EndcapCrossTalk2016);
    writeCdo -> setEndcapNoiseOccupancy(m_EndcapNoiseOccupancy2016);
    writeCdo -> setEndcapDisableProbability(m_EndcapDisableProbability2016);

    writeCdo -> setDBMToTThreshold(m_DBMToTThreshold2016);
    writeCdo -> setDBMCrossTalk(m_DBMCrossTalk2016);
    writeCdo -> setDBMNoiseOccupancy(m_DBMNoiseOccupancy2016);
    writeCdo -> setDBMDisableProbability(m_DBMDisableProbability2016);

    // This is ad-hoc solution.
    for (size_t i=0; i<m_IBLNoiseShape2016.size(); i++)    { writeCdo->setBarrelNoiseShape(0,m_IBLNoiseShape2016[i]); }
    for (size_t i=0; i<m_BLayerNoiseShape2016.size(); i++) { writeCdo->setBarrelNoiseShape(1,m_BLayerNoiseShape2016[i]); }
    for (size_t i=0; i<m_PixelNoiseShape2016.size(); i++)  {
      for (size_t layer:{2,3}) { writeCdo->setBarrelNoiseShape(layer,m_PixelNoiseShape2016[i]); }
    }

    for (size_t i=0; i<m_PixelNoiseShape2016.size(); i++)  {
      for (size_t layer:{0,1,2}) { writeCdo->setEndcapNoiseShape(layer,m_PixelNoiseShape2016[i]); }
    }

    for (size_t i=0; i<m_IBLNoiseShape2016.size(); i++)    {
      for (size_t layer:{0,1,2}) { writeCdo->setDBMNoiseShape(layer,m_IBLNoiseShape2016[i]); }
    }
  }
  else if (currentRunNumber<310000) {  // RUN2 2017
    writeCdo -> setBarrelToTThreshold(m_BarrelToTThreshold2017);
    writeCdo -> setFEI3BarrelLatency(m_FEI3BarrelLatency2017);
    writeCdo -> setFEI3BarrelHitDuplication(m_FEI3BarrelHitDuplication2017);
    writeCdo -> setFEI3BarrelSmallHitToT(m_FEI3BarrelSmallHitToT2017);
    writeCdo -> setFEI3BarrelTimingSimTune(m_FEI3BarrelTimingSimTune2017);
    writeCdo -> setBarrelCrossTalk(m_BarrelCrossTalk2017);
    writeCdo -> setBarrelNoiseOccupancy(m_BarrelNoiseOccupancy2017);
    writeCdo -> setBarrelDisableProbability(m_BarrelDisableProbability2017);

    writeCdo -> setEndcapToTThreshold(m_EndcapToTThreshold2017);
    writeCdo -> setFEI3EndcapLatency(m_FEI3EndcapLatency2017);
    writeCdo -> setFEI3EndcapHitDuplication(m_FEI3EndcapHitDuplication2017);
    writeCdo -> setFEI3EndcapSmallHitToT(m_FEI3EndcapSmallHitToT2017);
    writeCdo -> setFEI3EndcapTimingSimTune(m_FEI3EndcapTimingSimTune2017);
    writeCdo -> setEndcapCrossTalk(m_EndcapCrossTalk2017);
    writeCdo -> setEndcapNoiseOccupancy(m_EndcapNoiseOccupancy2017);
    writeCdo -> setEndcapDisableProbability(m_EndcapDisableProbability2017);

    writeCdo -> setDBMToTThreshold(m_DBMToTThreshold2017);
    writeCdo -> setDBMCrossTalk(m_DBMCrossTalk2017);
    writeCdo -> setDBMNoiseOccupancy(m_DBMNoiseOccupancy2017);
    writeCdo -> setDBMDisableProbability(m_DBMDisableProbability2017);

    // This is ad-hoc solution.
    for (size_t i=0; i<m_IBLNoiseShape2017.size(); i++)    { writeCdo->setBarrelNoiseShape(0,m_IBLNoiseShape2017[i]); }
    for (size_t i=0; i<m_BLayerNoiseShape2017.size(); i++) { writeCdo->setBarrelNoiseShape(1,m_BLayerNoiseShape2017[i]); }
    for (size_t i=0; i<m_PixelNoiseShape2017.size(); i++)  {
      for (size_t layer:{2,3}) { writeCdo->setBarrelNoiseShape(layer,m_PixelNoiseShape2017[i]); }
    }

    for (size_t i=0; i<m_PixelNoiseShape2017.size(); i++)  {
      for (size_t layer:{0,1,2}) { writeCdo->setEndcapNoiseShape(layer,m_PixelNoiseShape2017[i]); }
    }

    for (size_t i=0; i<m_IBLNoiseShape2017.size(); i++)    {
      for (size_t layer:{0,1,2}) { writeCdo->setDBMNoiseShape(layer,m_IBLNoiseShape2017[i]); }
    }
  }
  else {  // RUN2 2018
    writeCdo -> setBarrelToTThreshold(m_BarrelToTThreshold2018);
    writeCdo -> setFEI3BarrelLatency(m_FEI3BarrelLatency2018);
    writeCdo -> setFEI3BarrelHitDuplication(m_FEI3BarrelHitDuplication2018);
    writeCdo -> setFEI3BarrelSmallHitToT(m_FEI3BarrelSmallHitToT2018);
    writeCdo -> setFEI3BarrelTimingSimTune(m_FEI3BarrelTimingSimTune2018);
    writeCdo -> setBarrelCrossTalk(m_BarrelCrossTalk2018);
    writeCdo -> setBarrelNoiseOccupancy(m_BarrelNoiseOccupancy2018);
    writeCdo -> setBarrelDisableProbability(m_BarrelDisableProbability2018);

    writeCdo -> setEndcapToTThreshold(m_EndcapToTThreshold2018);
    writeCdo -> setFEI3EndcapLatency(m_FEI3EndcapLatency2018);
    writeCdo -> setFEI3EndcapHitDuplication(m_FEI3EndcapHitDuplication2018);
    writeCdo -> setFEI3EndcapSmallHitToT(m_FEI3EndcapSmallHitToT2018);
    writeCdo -> setFEI3EndcapTimingSimTune(m_FEI3EndcapTimingSimTune2018);
    writeCdo -> setEndcapCrossTalk(m_EndcapCrossTalk2018);
    writeCdo -> setEndcapNoiseOccupancy(m_EndcapNoiseOccupancy2018);
    writeCdo -> setEndcapDisableProbability(m_EndcapDisableProbability2018);

    writeCdo -> setDBMToTThreshold(m_DBMToTThreshold2018);
    writeCdo -> setDBMCrossTalk(m_DBMCrossTalk2018);
    writeCdo -> setDBMNoiseOccupancy(m_DBMNoiseOccupancy2018);
    writeCdo -> setDBMDisableProbability(m_DBMDisableProbability2018);

    // This is ad-hoc solution.
    for (size_t i=0; i<m_IBLNoiseShape2018.size(); i++)    { writeCdo->setBarrelNoiseShape(0,m_IBLNoiseShape2018[i]); }
    for (size_t i=0; i<m_BLayerNoiseShape2018.size(); i++) { writeCdo->setBarrelNoiseShape(1,m_BLayerNoiseShape2018[i]); }
    for (size_t i=0; i<m_PixelNoiseShape2018.size(); i++)  {
      for (size_t layer:{2,3}) { writeCdo->setBarrelNoiseShape(layer,m_PixelNoiseShape2018[i]); }
    }

    for (size_t i=0; i<m_PixelNoiseShape2018.size(); i++)  {
      for (size_t layer:{0,1,2}) { writeCdo->setEndcapNoiseShape(layer,m_PixelNoiseShape2018[i]); }
    }

    for (size_t i=0; i<m_IBLNoiseShape2018.size(); i++)    {
      for (size_t layer:{0,1,2}) { writeCdo->setDBMNoiseShape(layer,m_IBLNoiseShape2018[i]); }
    }
  }

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

