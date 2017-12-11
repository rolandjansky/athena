/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "SCT_SensorsCondAlg.h"

#include "GaudiKernel/EventIDRange.h"

SCT_SensorsCondAlg::SCT_SensorsCondAlg(const std::string& name, ISvcLocator* pSvcLocator)
  : ::AthAlgorithm(name, pSvcLocator)
  , m_readKey{"/SCT/Sensors"}
  , m_writeKey{"SCT_SensorsCondData"}
  , m_condSvc{"CondSvc", name}
{
  declareProperty("ReadKey", m_readKey, "Key of input (raw) conditions folder");
  declareProperty("WriteKey", m_writeKey, "Key of output (derived) conditions folder");
}

SCT_SensorsCondAlg::~SCT_SensorsCondAlg()
{
}

StatusCode SCT_SensorsCondAlg::initialize()
{
  ATH_MSG_DEBUG("initialize " << name());

  // CondSvc
  ATH_CHECK( m_condSvc.retrieve() );

  // Read Cond Handle
  ATH_CHECK( m_readKey.initialize() );

  // Write Cond Handle
  ATH_CHECK( m_writeKey.initialize() );
  // Register write handle
  if(m_condSvc->regHandle(this, m_writeKey).isFailure()) {
    ATH_MSG_ERROR("unable to register WriteCondHandle " << m_writeKey.fullKey() << " with CondSvc");
    return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;
}

StatusCode SCT_SensorsCondAlg::execute()
{
  ATH_MSG_DEBUG("execute " << name());

  // Write Cond Handle
  SG::WriteCondHandle<SCT_SensorsCondData> writeHandle{m_writeKey};

  // Do we have a valid Write Cond Handle for current time?
  if(writeHandle.isValid()) {
    // in theory this should never be called in MT
    writeHandle.updateStore();
    ATH_MSG_DEBUG("CondHandle " << writeHandle.fullKey() << " is already valid."
                  << ". In theory this should not be called, but may happen"
                  << " if multiple concurrent events are being processed out of order."
                  << " Forcing update of Store contents");
    return StatusCode::SUCCESS; 
  }

  // Read Cond Handle 
  SG::ReadCondHandle<CondAttrListCollection> readHandle{m_readKey};
  const CondAttrListCollection* readCdo{*readHandle}; 
  if(readCdo==nullptr) {
    ATH_MSG_FATAL("Null pointer to the read conditions object");
    return StatusCode::FAILURE;
  }
  ATH_MSG_INFO("Size of CondAttrListCollection readCdo->size()= " << readCdo->size());

  // Define validity of the output cond obbject
  EventIDRange rangeW;
  if(not readHandle.range(rangeW)) {
    ATH_MSG_FATAL("Failed to retrieve validity range for " << readHandle.key());
    return StatusCode::FAILURE;
  }

  // Construct the output Cond Object and fill it in
  SCT_SensorsCondData* writeCdo{new SCT_SensorsCondData()};

  // Fill write conditions data object
  CondAttrListCollection::const_iterator attrList{readCdo->begin()};
  CondAttrListCollection::const_iterator end{readCdo->end()};
  // CondAttrListCollection doesnt support C++11 type loops, no generic 'begin'
  for(; attrList!=end; ++attrList) {
    CondAttrListCollection::ChanNum truncatedSerialNumber{attrList->first};
    SCT_SensorCondData data;
    bool isOK{false};
    isOK |= data.setTruncatedSerialNumber(truncatedSerialNumber);
    isOK |= data.setManufacturer(attrList->second[0].data<std::string>());
    isOK |= data.setDepletionVoltage(SCT_SensorCondData::SENSOR1, attrList->second[1].data<float>());
    isOK |= data.setDepletionVoltage(SCT_SensorCondData::SENSOR2, attrList->second[2].data<float>());
    isOK |= data.setDepletionVoltage(SCT_SensorCondData::SENSOR3, attrList->second[3].data<float>());
    isOK |= data.setDepletionVoltage(SCT_SensorCondData::SENSOR4, attrList->second[4].data<float>());
    isOK |= data.setCrystalOrientation(SCT_SensorCondData::SENSOR1, static_cast<int>(attrList->second[5].data<long long>()));
    isOK |= data.setCrystalOrientation(SCT_SensorCondData::SENSOR2, static_cast<int>(attrList->second[6].data<long long>()));
    isOK |= data.setCrystalOrientation(SCT_SensorCondData::SENSOR3, static_cast<int>(attrList->second[7].data<long long>()));
    isOK |= data.setCrystalOrientation(SCT_SensorCondData::SENSOR4, static_cast<int>(attrList->second[8].data<long long>()));
    if (not isOK) {
      ATH_MSG_WARNING("At least one element of SCT_SensorCondData for truncatedSerialNumber " << truncatedSerialNumber << " was not correctly stored.");
    }
    (*writeCdo)[truncatedSerialNumber] = data;
  }

  // Record write conditions data object
  if(writeHandle.record(rangeW, writeCdo).isFailure()) {
    ATH_MSG_FATAL("Could not record SCT_SensorsCondData " << writeHandle.key() 
                  << " with EventRange " << rangeW
                  << " into Conditions Store");
    return StatusCode::FAILURE;
  }
  ATH_MSG_INFO("recorded new CDO " << writeHandle.key() << " with range " << rangeW << " into Conditions Store");

  return StatusCode::SUCCESS;
}

StatusCode SCT_SensorsCondAlg::finalize()
{
  ATH_MSG_DEBUG("finalize " << name());
  return StatusCode::SUCCESS;
}
