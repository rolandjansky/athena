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
  m_BarrelAnalogThreshold({-1,-1,-1,-1,-1,-1,-1}),
  m_EndcapAnalogThreshold({-1,-1,-1,-1,-1,-1,-1}),
  m_DBMAnalogThreshold({-1,-1,-1,-1,-1,-1,-1}),
  m_BarrelToTThreshold({-1,-1,-1,-1,-1,-1,-1}),
  m_EndcapToTThreshold({-1,-1,-1,-1,-1,-1,-1}),
  m_DBMToTThreshold({-1,-1,-1,-1,-1,-1,-1}),
  m_BarrelLatency({256,256,256,256,256,256,256}),
  m_EndcapLatency({256,256,256,256,256,256,256}),
  m_DBMLatency({256,256,256,256,256,256,256}),
  m_BarrelCrossTalk({0.06,0.06,0.06,0.06,0.06,0.06,0.06}),
  m_EndcapCrossTalk({0.06,0.06,0.06,0.06,0.06,0.06,0.06}),
  m_DBMCrossTalk({0.06,0.06,0.06,0.06,0.06,0.06,0.06}),
  m_BarrelThermalNoise({160.0,160.0,160.0,160.0,160.0,160.0,160.0}),
  m_EndcapThermalNoise({160.0,160.0,160.0,160.0,160.0,160.0,160.0}),
  m_DBMThermalNoise({160.0,160.0,160.0,160.0,160.0,160.0,160.0}),
  m_BarrelHitDuplication({false,false,false,false,false,false,false}),
  m_EndcapHitDuplication({false,false,false,false,false,false,false}),
  m_DBMHitDuplication({false,false,false,false,false,false,false}),
  m_BarrelSmallHitToT({-1,-1,-1,-1,-1,-1,-1}),
  m_EndcapSmallHitToT({-1,-1,-1,-1,-1,-1,-1}),
  m_DBMSmallHitToT({-1,-1,-1,-1,-1,-1,-1}),
  m_IBLHitDisConfig(2),
  m_useDeadMap(true),
  m_condSvc("CondSvc", name)
{
  declareProperty("BarrelAnalogThreshold", m_BarrelAnalogThreshold);
  declareProperty("EndcapAnalogThreshold", m_EndcapAnalogThreshold);
  declareProperty("DBMAnalogThreshold",    m_DBMAnalogThreshold);
  declareProperty("BarrelToTThreshold",    m_BarrelToTThreshold);
  declareProperty("EndcapToTThreshold",    m_EndcapToTThreshold);
  declareProperty("DBMToTThreshold",       m_DBMToTThreshold);
  declareProperty("BarrelLatency",         m_BarrelLatency);
  declareProperty("EndcapLatency",         m_EndcapLatency);
  declareProperty("DBMLatency",            m_DBMLatency);
  declareProperty("BarrelCrossTalk",       m_BarrelCrossTalk);
  declareProperty("EndcapCrossTalk",       m_EndcapCrossTalk);
  declareProperty("DBMCrossTalk",          m_DBMCrossTalk);
  declareProperty("BarrelThermalNoise",    m_BarrelThermalNoise);
  declareProperty("EndcapThermalNoise",    m_EndcapThermalNoise);
  declareProperty("DBMThermalNoise",       m_DBMThermalNoise);
  declareProperty("BarrelHitDuplication",  m_BarrelHitDuplication);
  declareProperty("EndcapHitDuplication",  m_EndcapHitDuplication);
  declareProperty("DBMHitDuplication",     m_DBMHitDuplication);
  declareProperty("BarrelSmallHitToT",     m_BarrelSmallHitToT);
  declareProperty("EndcapSmallHitToT",     m_EndcapSmallHitToT);
  declareProperty("DBMSmallHitToT",        m_DBMSmallHitToT);
  declareProperty("FEI4HitDiscConfig",     m_IBLHitDisConfig);
  declareProperty("UseDeadMap",            m_useDeadMap, "Switch for usage of dead map");
}

StatusCode PixelConfigCondAlg::initialize() {
  ATH_MSG_DEBUG("PixelConfigCondAlg::initialize()");

  ATH_CHECK(m_condSvc.retrieve());

  if (m_useDeadMap) { ATH_CHECK(m_readDeadMapKey.initialize()); }

  ATH_CHECK(m_writeKey.initialize());
  if (m_condSvc->regHandle(this,m_writeKey).isFailure()) {
    ATH_MSG_FATAL("unable to register WriteCondHandle " << m_writeKey.fullKey() << " with CondSvc");
    return StatusCode::FAILURE;
  }

  if (m_IBLHitDisConfig<0 || m_IBLHitDisConfig>3) {
    ATH_MSG_FATAL("HitDiscConfig should be set [0-3].  (FEI4HitDiscConfig=" << m_IBLHitDisConfig << ")");
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
  writeCdo -> setBarrelAnalogThreshold(m_BarrelAnalogThreshold);
  writeCdo -> setEndcapAnalogThreshold(m_EndcapAnalogThreshold);
  writeCdo -> setDBMAnalogThreshold(m_DBMAnalogThreshold);
  writeCdo -> setBarrelToTThreshold(m_BarrelToTThreshold);
  writeCdo -> setEndcapToTThreshold(m_EndcapToTThreshold);
  writeCdo -> setDBMToTThreshold(m_DBMToTThreshold);
  writeCdo -> setBarrelLatency(m_BarrelLatency);
  writeCdo -> setEndcapLatency(m_EndcapLatency);
  writeCdo -> setDBMLatency(m_DBMLatency);
  writeCdo -> setBarrelCrossTalk(m_BarrelCrossTalk);
  writeCdo -> setEndcapCrossTalk(m_EndcapCrossTalk);
  writeCdo -> setDBMCrossTalk(m_DBMCrossTalk);
  writeCdo -> setBarrelThermalNoise(m_BarrelThermalNoise);
  writeCdo -> setEndcapThermalNoise(m_EndcapThermalNoise);
  writeCdo -> setDBMThermalNoise(m_DBMThermalNoise);
  writeCdo -> setBarrelHitDuplication(m_BarrelHitDuplication);
  writeCdo -> setEndcapHitDuplication(m_EndcapHitDuplication);
  writeCdo -> setDBMHitDuplication(m_DBMHitDuplication);
  writeCdo -> setBarrelSmallHitToT(m_BarrelSmallHitToT);
  writeCdo -> setEndcapSmallHitToT(m_EndcapSmallHitToT);
  writeCdo -> setDBMSmallHitToT(m_DBMSmallHitToT);
  writeCdo -> setIBLHitDiscConfig(m_IBLHitDisConfig);

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

