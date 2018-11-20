/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#include "PixelDeadMapCondAlg.h"
#include "Identifier/IdentifierHash.h"
#include "GaudiKernel/EventIDRange.h"
#include <memory>
#include <sstream>

PixelDeadMapCondAlg::PixelDeadMapCondAlg(const std::string& name, ISvcLocator* pSvcLocator):
  ::AthAlgorithm(name, pSvcLocator),
  m_condSvc("CondSvc", name)
{
}

StatusCode PixelDeadMapCondAlg::initialize() {
  ATH_MSG_INFO("PixelDeadMapCondAlg::initialize()");

  ATH_CHECK(m_condSvc.retrieve());

  ATH_CHECK(m_readKey.initialize());

  ATH_CHECK(m_writeKey.initialize());
  if (m_condSvc->regHandle(this,m_writeKey).isFailure()) {
    ATH_MSG_FATAL("unable to register WriteCondHandle " << m_writeKey.fullKey() << " with CondSvc");
    return StatusCode::FAILURE;
  }
  return StatusCode::SUCCESS;
}

StatusCode PixelDeadMapCondAlg::execute() {
  ATH_MSG_INFO("PixelDeadMapCondAlg::execute()");

  SG::WriteCondHandle<PixelModuleData> writeHandle(m_writeKey);
  if (writeHandle.isValid()) {
    ATH_MSG_DEBUG("CondHandle " << writeHandle.fullKey() << " is already valid.. In theory this should not be called, but may happen if multiple concurrent events are being processed out of order.");
    return StatusCode::SUCCESS; 
  }

  SG::ReadCondHandle<CondAttrListCollection> readHandle(m_readKey);
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

  // Read dead map info
  for (CondAttrListCollection::const_iterator attrList=readCdo->begin(); attrList!=readCdo->end(); ++attrList) {
    CondAttrListCollection::ChanNum channelNumber = attrList->first;
    CondAttrListCollection::AttributeList payload = attrList->second;

    // RUN-3 format
    if (payload.exists("data_array") and not payload["data_array"].isNull()) {
      std::string stringStatus = payload["data_array"].data<std::string>();

      std::stringstream ss(stringStatus);
      std::vector<std::string> component;
      std::string buffer;
      while (std::getline(ss,buffer,',')) { component.push_back(buffer); }

      for (int i=0; i<(int)component.size(); i++) {
        std::stringstream checkModule(component[i]);
        std::vector<std::string> moduleString;
        while (std::getline(checkModule,buffer,'"')) { moduleString.push_back(buffer); }

        int moduleHash = std::atoi(moduleString[1].c_str());
        if (moduleHash!=i) { ATH_MSG_ERROR("Mismatch hash ID in DB array."); }

        std::stringstream checkStatus(moduleString[3]);
        std::vector<std::string> moduleStringStatus;
        while (std::getline(checkStatus,buffer,' ')) { moduleStringStatus.push_back(buffer); }

        int moduleStatus = std::atoi(moduleStringStatus[0].c_str());
        int chipStatus   = std::atoi(moduleStringStatus[1].c_str());

        writeCdo -> setModuleStatus(moduleHash, moduleStatus);
        writeCdo -> setChipStatus(moduleHash, chipStatus);
      }
    }
    // RUN-1, RUN-2 format
    else if (payload.exists("moduleID") and not payload["moduleID"].isNull() && payload.exists("ModuleSpecialPixelMap_Clob") and not payload["ModuleSpecialPixelMap_Clob"].isNull()) {

      int moduleHash = payload["moduleID"].data<int>();
      std::string stringStatus = payload["ModuleSpecialPixelMap_Clob"].data<std::string>();

      std::stringstream ss(stringStatus);
      std::vector<std::string> moduleStringStatus;
      std::string buffer;
      while (std::getline(ss,buffer,' ')) { moduleStringStatus.push_back(buffer); }

      int moduleStatus = std::atoi(moduleStringStatus[0].c_str());
      int chipStatus   = std::atoi(moduleStringStatus[1].c_str());

      writeCdo -> setModuleStatus(moduleHash, moduleStatus);
      writeCdo -> setChipStatus(moduleHash, chipStatus);
    } 
    else {
      ATH_MSG_WARNING("Can not retrieve " << channelNumber);
      for (int i=0; i<2048; i++) {
        writeCdo -> setModuleStatus(i, 0);
        writeCdo -> setChipStatus(i, 0);
      }
    }
  }

  if (writeHandle.record(rangeW, std::move(writeCdo)).isFailure()) {
    ATH_MSG_FATAL("Could not record PixelModuleData " << writeHandle.key() << " with EventRange " << rangeW << " into Conditions Store");
    return StatusCode::FAILURE;
  }
  ATH_MSG_INFO("recorded new CDO " << writeHandle.key() << " with range " << rangeW << " into Conditions Store");

  return StatusCode::SUCCESS;
}

StatusCode PixelDeadMapCondAlg::finalize() {
  ATH_MSG_INFO("PixelDeadMapCondAlg::finalize()");
  return StatusCode::SUCCESS;
}

