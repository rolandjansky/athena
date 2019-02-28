/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "PixelDCSCondHVAlg.h"
#include "Identifier/IdentifierHash.h"
#include "GaudiKernel/EventIDRange.h"
#include <memory>

PixelDCSCondHVAlg::PixelDCSCondHVAlg(const std::string& name, ISvcLocator* pSvcLocator):
  ::AthReentrantAlgorithm(name, pSvcLocator),
  m_pixelID(nullptr),
  m_useConditions(true),
  m_defaultBiasVoltage(150.0),
  m_condSvc("CondSvc", name)
{
  declareProperty("UseConditions", m_useConditions); 
  declareProperty("BiasVoltage",   m_defaultBiasVoltage, "Default bias voltage in Volt." ); 
}

StatusCode PixelDCSCondHVAlg::initialize() {
  ATH_MSG_DEBUG("PixelDCSCondHVAlg::initialize()");

  ATH_CHECK(detStore()->retrieve(m_pixelID,"PixelID"));

  ATH_CHECK(m_condSvc.retrieve());

  ATH_CHECK(m_readKey.initialize());
  ATH_CHECK(m_writeKey.initialize());
  if (m_condSvc->regHandle(this,m_writeKey).isFailure()) {
    ATH_MSG_FATAL("unable to register WriteCondHandle " << m_writeKey.fullKey() << " with CondSvc");
    return StatusCode::FAILURE;
  }
  return StatusCode::SUCCESS;
}

StatusCode PixelDCSCondHVAlg::execute(const EventContext& ctx) const {
  ATH_MSG_DEBUG("PixelDCSCondHVAlg::execute()");

  SG::WriteCondHandle<PixelModuleData> writeHandle(m_writeKey, ctx);
  if (writeHandle.isValid()) {
    ATH_MSG_DEBUG("CondHandle " << writeHandle.fullKey() << " is already valid.. In theory this should not be called, but may happen if multiple concurrent events are being processed out of order.");
    return StatusCode::SUCCESS; 
  }

  SG::ReadCondHandle<CondAttrListCollection> readHandle(m_readKey, ctx);
  const CondAttrListCollection* readCdo = *readHandle; 
  if (readCdo==nullptr) {
    ATH_MSG_FATAL("Null pointer to the read conditions object");
    return StatusCode::FAILURE;
  }
  // Get the validitiy range
  EventIDRange rangeW;
  if (not readHandle.range(rangeW)) {
    ATH_MSG_FATAL("Failed to retrieve validity range for " << readHandle.key());
    return StatusCode::FAILURE;
  }
  ATH_MSG_INFO("Size of CondAttrListCollection " << readHandle.fullKey() << " readCdo->size()= " << readCdo->size());
  ATH_MSG_INFO("Range of input is " << rangeW);
  
  // Construct the output Cond Object and fill it in
  std::unique_ptr<PixelModuleData> writeCdo(std::make_unique<PixelModuleData>());

  // Read HV info
  std::string param("HV");
  if (m_useConditions) {
    for (CondAttrListCollection::const_iterator attrList=readCdo->begin(); attrList!=readCdo->end(); ++attrList) {
      const CondAttrListCollection::ChanNum &channelNumber = attrList->first;
      const CondAttrListCollection::AttributeList &payload = attrList->second;
      if (payload.exists(param) and not payload[param].isNull()) {
        float val = payload[param].data<float>();
        if (val>1000.0 || val<-1000.0) {
          writeCdo -> setBiasVoltage((int)channelNumber, m_defaultBiasVoltage);
        }
        else {
          writeCdo -> setBiasVoltage((int)channelNumber, val);
        }
      } 
      else {
        ATH_MSG_WARNING(param << " does not exist for ChanNum " << channelNumber);
        writeCdo -> setBiasVoltage((int)channelNumber, m_defaultBiasVoltage);
      }
    }
  }
  else {
    for (int i=0; i<(int)m_pixelID->wafer_hash_max(); i++) {
      writeCdo -> setBiasVoltage(i, m_defaultBiasVoltage);
    }
  }

  if (writeHandle.record(rangeW, std::move(writeCdo)).isFailure()) {
    ATH_MSG_FATAL("Could not record PixelModuleData " << writeHandle.key() << " with EventRange " << rangeW << " into Conditions Store");
    return StatusCode::FAILURE;
  }
  ATH_MSG_INFO("recorded new CDO " << writeHandle.key() << " with range " << rangeW << " into Conditions Store");

  return StatusCode::SUCCESS;
}

StatusCode PixelDCSCondHVAlg::finalize() {
  ATH_MSG_DEBUG("PixelDCSCondHVAlg::finalize()");
  return StatusCode::SUCCESS;
}

