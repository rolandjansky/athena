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
  m_bunchSpace(25.0),
  m_UseComTime(false),
  m_ComTime(0.0),
  m_BarrelNumberOfBCID({1,1,1,1}),
  m_EndcapNumberOfBCID({1,1,1}),
  m_DBMNumberOfBCID({1,1,1}),
  m_BarrelTimeOffset({5.0,5.0,5.0,5.0}),
  m_EndcapTimeOffset({5.0,5.0,5.0}),
  m_DBMTimeOffset({5.0,5.0,5.0}),
  m_BarrelTimeJitter({0.0,0.0,0.0,0.0}),
  m_EndcapTimeJitter({0.0,0.0,0.0}),
  m_DBMTimeJitter({0.0,0.0,0.0}),
  m_useCalibConditions(true),
  m_BarrelAnalogThreshold({2000,4300,3500,3500}),
  m_EndcapAnalogThreshold({3500,3500,3500}),
  m_DBMAnalogThreshold({2000,2000,2000}),
  m_BarrelAnalogThresholdSigma({45,35,30,30}),
  m_EndcapAnalogThresholdSigma({30,30,30}),
  m_DBMAnalogThresholdSigma({70,70,70}),
  m_BarrelAnalogThresholdNoise({130,150,160,160}),
  m_EndcapAnalogThresholdNoise({150,150,150}),
  m_DBMAnalogThresholdNoise({190,190,190}),
  m_BarrelInTimeThreshold({2000,5000,5000,5000}),
  m_EndcapInTimeThreshold({5000,5000,5000}),
  m_DBMInTimeThreshold({1200,1200,1200}),
  m_CalibrationParameterA(70.2),
  m_CalibrationParameterE(-3561.25),
  m_CalibrationParameterC(26000),
  m_BarrelToTThreshold({-1, 3, 5, 5}),
  m_EndcapToTThreshold({ 5, 5, 5}),
  m_DBMToTThreshold({-1,-1,-1}),
  m_BarrelCrossTalk({0.06,0.06,0.06,0.06}),
  m_EndcapCrossTalk({0.06,0.06,0.06}),
  m_DBMCrossTalk({0.06,0.06,0.06}),
  m_BarrelThermalNoise({160.0,160.0,160.0,160.0}),
  m_EndcapThermalNoise({160.0,160.0,160.0}),
  m_DBMThermalNoise({160.0,160.0,160.0}),
  m_BarrelNoiseOccupancy({5e-8,5e-8,5e-8,5e-8}),
  m_EndcapNoiseOccupancy({5e-8,5e-8,5e-8}),
  m_DBMNoiseOccupancy({5e-8,5e-8,5e-8}),
  m_BarrelDisableProbability({9e-3,9e-3,9e-3,9e-3}),
  m_EndcapDisableProbability({9e-3,9e-3,9e-3}),
  m_DBMDisableProbability({9e-3,9e-3,9e-3}),
  m_BarrelNoiseShape({{0.0,1.0},{0.0,1.0},{0.0,1.0},{0.0,1.0}}),
  m_EndcapNoiseShape({{0.0,1.0},{0.0,1.0},{0.0,1.0}}),
  m_DBMNoiseShape({{0.0,1.0},{0.0,1.0},{0.0,1.0}}),
  m_IBLNoiseShape({0.0,1.0}),
  m_BLayerNoiseShape({0.0,1.0}),
  m_PixelNoiseShape({0.0,1.0}),
  m_FEI3BarrelLatency({  0,151,256,256}),
  m_FEI3EndcapLatency({256,256,256}),
  m_FEI3BarrelHitDuplication({false,false,false,false}),
  m_FEI3EndcapHitDuplication({false,false,false}),
  m_FEI3BarrelSmallHitToT({-1,-1,-1,-1}),
  m_FEI3EndcapSmallHitToT({-1,-1,-1}),
  m_FEI3BarrelTimingSimTune({2015,2015,2015,2015}),
  m_FEI3EndcapTimingSimTune({2015,2015,2015}),
  m_FEI4BarrelHitDiscConfig({2}),
  m_FEI4EndcapHitDiscConfig({2}),
  m_useDeadMap(true),
  m_condSvc("CondSvc", name)
{
  declareProperty("BunchSpace",         m_bunchSpace);
  declareProperty("UseComTime",         m_UseComTime);
  declareProperty("BarrelNumberOfBCID", m_BarrelNumberOfBCID);
  declareProperty("EndcapNumberOfBCID", m_EndcapNumberOfBCID);
  declareProperty("DBMNumberOfBCID",    m_DBMNumberOfBCID);
  declareProperty("BarrelTimeOffset",   m_BarrelTimeOffset);
  declareProperty("EndcapTimeOffset",   m_EndcapTimeOffset);
  declareProperty("DBMTimeOffset",      m_DBMTimeOffset);
  declareProperty("BarrelTimeJitter",   m_BarrelTimeJitter);
  declareProperty("EndcapTimeJitter",   m_EndcapTimeJitter);
  declareProperty("DBMTimeJitter",      m_DBMTimeJitter);
  declareProperty("UseCalibConditions", m_useCalibConditions);
  declareProperty("DefaultBarrelAnalogThreshold", m_BarrelAnalogThreshold);
  declareProperty("DefaultEndcapAnalogThreshold", m_EndcapAnalogThreshold);
  declareProperty("DefaultDBMAnalogThreshold",    m_DBMAnalogThreshold);
  declareProperty("DefaultBarrelAnalogThresholdSigma", m_BarrelAnalogThresholdSigma);
  declareProperty("DefaultEndcapAnalogThresholdSigma", m_EndcapAnalogThresholdSigma);
  declareProperty("DefaultDBMAnalogThresholdSigma",    m_DBMAnalogThresholdSigma);
  declareProperty("DefaultBarrelAnalogThresholdNoise", m_BarrelAnalogThresholdNoise);
  declareProperty("DefaultEndcapAnalogThresholdNoise", m_EndcapAnalogThresholdNoise);
  declareProperty("DefaultDBMAnalogThresholdNoise",    m_DBMAnalogThresholdNoise);
  declareProperty("DefaultBarrelInTimeThreshold",      m_BarrelInTimeThreshold);
  declareProperty("DefaultEndcapInTimeThreshold",      m_EndcapInTimeThreshold);
  declareProperty("DefaultDBMInTimeThreshold",         m_DBMInTimeThreshold);
  declareProperty("DefaultCalibrationParameterA",      m_CalibrationParameterA);
  declareProperty("DefaultCalibrationParameterE",      m_CalibrationParameterE);
  declareProperty("DefaultCalibrationParameterC",      m_CalibrationParameterC);
  declareProperty("BarrelToTThreshold",    m_BarrelToTThreshold);
  declareProperty("EndcapToTThreshold",    m_EndcapToTThreshold);
  declareProperty("DBMToTThreshold",       m_DBMToTThreshold);
  declareProperty("BarrelCrossTalk",       m_BarrelCrossTalk);
  declareProperty("EndcapCrossTalk",       m_EndcapCrossTalk);
  declareProperty("DBMCrossTalk",          m_DBMCrossTalk);
  declareProperty("BarrelThermalNoise",    m_BarrelThermalNoise);
  declareProperty("EndcapThermalNoise",    m_EndcapThermalNoise);
  declareProperty("DBMThermalNoise",       m_DBMThermalNoise);
//  declareProperty("BarrelNoiseShape",      m_BarrelNoiseShape);  // So far deaclareProperty does not accept 2D vector.
//  declareProperty("EndcapNoiseShape",      m_EndcapNoiseShape);
//  declareProperty("DBMNoiseShape",         m_DBMNoiseShape);
  declareProperty("IBLNoiseShape",         m_IBLNoiseShape);       // This is ad-hoc solution.
  declareProperty("BLayerNoiseShape",      m_BLayerNoiseShape);
  declareProperty("PixelNoiseShape",       m_PixelNoiseShape);
  declareProperty("FEI3BarrelLatency",        m_FEI3BarrelLatency);
  declareProperty("FEI3EndcapLatency",        m_FEI3EndcapLatency);
  declareProperty("FEI3BarrelHitDuplication", m_FEI3BarrelHitDuplication);
  declareProperty("FEI3EndcapHitDuplication", m_FEI3EndcapHitDuplication);
  declareProperty("FEI3BarrelSmallHitToT",    m_FEI3BarrelSmallHitToT);
  declareProperty("FEI3EndcapSmallHitToT",    m_FEI3EndcapSmallHitToT);
  declareProperty("FEI3BarrelTimingSimTune",  m_FEI3BarrelTimingSimTune);
  declareProperty("FEI3EndcapTimingSimTune",  m_FEI3EndcapTimingSimTune);
  declareProperty("FEI4BarrelHitDiscConfig",  m_FEI4BarrelHitDiscConfig);
  declareProperty("FEI4EndcapHitDiscConfig",  m_FEI4EndcapHitDiscConfig);
  declareProperty("UseDeadMap",            m_useDeadMap, "Switch for usage of dead map");
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

  if (m_useDeadMap) { ATH_CHECK(m_readDeadMapKey.initialize()); }

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

  const EventIDBase start{EventIDBase::UNDEFNUM, EventIDBase::UNDEFEVT, 0,                       0,                       EventIDBase::UNDEFNUM, EventIDBase::UNDEFNUM};
  const EventIDBase stop {EventIDBase::UNDEFNUM, EventIDBase::UNDEFEVT, EventIDBase::UNDEFNUM-1, EventIDBase::UNDEFNUM-1, EventIDBase::UNDEFNUM, EventIDBase::UNDEFNUM};

  //==============
  // Set dead map
  //==============
  EventIDRange rangeDeadMap{start, stop};
  if (m_useDeadMap) {
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

      // RUN-3 format
      if (payload.exists("data_array") and not payload["data_array"].isNull()) {
        const std::string &stringStatus = payload["data_array"].data<std::string>();

        std::stringstream ss(stringStatus);
        std::vector<std::string> component;
        std::string buffer;
        while (std::getline(ss,buffer,',')) { component.push_back(buffer); }

        for (int i=0; i<(int)component.size(); i++) {
          std::stringstream checkModule(component[i]);
          std::vector<std::string> moduleString;
          while (std::getline(checkModule,buffer,'"')) { moduleString.push_back(buffer); }

          if (moduleString.size()<4) {
            ATH_MSG_FATAL("Not enough moduleString data " << moduleString.size() << " < 4 for " << component[i] 
                << " channel " <<  channelNumber << " read from " << readHandle.fullKey());
            return StatusCode::FAILURE;
          }

          int moduleHash = std::atoi(moduleString[1].c_str());
          if (moduleHash!=i) { ATH_MSG_ERROR("Mismatch hash ID in DB array."); }

          std::stringstream checkStatus(moduleString[3]);
          std::vector<std::string> moduleStringStatus;
          while (std::getline(checkStatus,buffer,' ')) { moduleStringStatus.push_back(buffer); }

          if (moduleStringStatus.size()<2) {
            ATH_MSG_FATAL("Not enough moduleStringStatus data " << moduleStringStatus.size() << " < 2 for " << component[i] 
                << " channel " <<  channelNumber << " read from " << readHandle.fullKey());
            return StatusCode::FAILURE;
          }

          int moduleStatus = std::atoi(moduleStringStatus[0].c_str());
          int chipStatus   = std::atoi(moduleStringStatus[1].c_str());

          if (moduleStatus>0) { writeCdo->setModuleStatus(moduleHash, moduleStatus); }
          if (chipStatus>0)   { writeCdo->setChipStatus(moduleHash, chipStatus); }
        }
      }
      // RUN-1, RUN-2 format
      else if (payload.exists("moduleID") and not payload["moduleID"].isNull() && payload.exists("ModuleSpecialPixelMap_Clob") and not payload["ModuleSpecialPixelMap_Clob"].isNull()) {

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

  //===============
  // Set threshold
  //===============
  EventIDRange rangeThreshold{start, stop};
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
  writeCdo -> setUseCalibConditions(m_useCalibConditions);
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
  writeCdo -> setDefaultQ2TotA(m_CalibrationParameterA);
  writeCdo -> setDefaultQ2TotE(m_CalibrationParameterE);
  writeCdo -> setDefaultQ2TotC(m_CalibrationParameterC);
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

  //=======================
  // Combine time interval
  //=======================
  EventIDRange rangeW{start, stop};
//  rangeW = EventIDRange::intersect(rangeDeadMap, rangeThreshold);
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

StatusCode PixelConfigCondAlg::finalize() {
  ATH_MSG_DEBUG("PixelConfigCondAlg::finalize()");
  return StatusCode::SUCCESS;
}

