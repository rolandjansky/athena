/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "PixelDeadMapCondAlg.h"
#include "GaudiKernel/EventIDRange.h"

#include <sstream>

PixelDeadMapCondAlg::PixelDeadMapCondAlg(const std::string& name, ISvcLocator* pSvcLocator):
  ::AthReentrantAlgorithm(name, pSvcLocator)
{
}

StatusCode PixelDeadMapCondAlg::initialize() {
  ATH_MSG_DEBUG("PixelDeadMapCondAlg::initialize()");

  ATH_CHECK(m_condSvc.retrieve());
  ATH_CHECK(m_readKey.initialize(SG::AllowEmpty));
  ATH_CHECK(m_writeKey.initialize());
  if (m_condSvc->regHandle(this,m_writeKey).isFailure()) {
    ATH_MSG_FATAL("unable to register WriteCondHandle " << m_writeKey.fullKey() << " with CondSvc");
    return StatusCode::FAILURE;
  }
  return StatusCode::SUCCESS;
}

StatusCode PixelDeadMapCondAlg::execute(const EventContext& ctx) const {
  ATH_MSG_DEBUG("PixelDeadMapCondAlg::execute()");

  SG::WriteCondHandle<PixelDeadMapCondData> writeHandle(m_writeKey, ctx);
  if (writeHandle.isValid()) {
    ATH_MSG_DEBUG("CondHandle " << writeHandle.fullKey() << " is already valid.. In theory this should not be called, but may happen if multiple concurrent events are being processed out of order.");
    return StatusCode::SUCCESS; 
  }

  // Construct the output Cond Object and fill it in
  std::unique_ptr<PixelDeadMapCondData> writeCdo(std::make_unique<PixelDeadMapCondData>());

  const EventIDBase start{EventIDBase::UNDEFNUM, EventIDBase::UNDEFEVT,                     0,                       
                                              0, EventIDBase::UNDEFNUM, EventIDBase::UNDEFNUM};
  const EventIDBase stop {EventIDBase::UNDEFNUM,   EventIDBase::UNDEFEVT, EventIDBase::UNDEFNUM-1, 
                          EventIDBase::UNDEFNUM-1, EventIDBase::UNDEFNUM, EventIDBase::UNDEFNUM};

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
    ATH_MSG_INFO("Size of AthenaAttributeList " << readHandle.fullKey() << " readCdo->size()= " << readCdo->size());
    ATH_MSG_INFO("Range of input is " << rangeW);

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
          while (std::getline(checkModule,buffer,':')) { moduleString.push_back(buffer); }

          if (moduleString.size()!=2) {
            ATH_MSG_FATAL("String size (moduleString) is not 2. " << moduleString.size() << " in " << component[i] << " channel " <<  channelNumber << " read from " << readHandle.fullKey());
            return StatusCode::FAILURE;
          }

          std::stringstream checkModuleHash(moduleString[0]);
          std::vector<std::string> moduleStringHash;
          while (std::getline(checkModuleHash,buffer,'"')) { moduleStringHash.push_back(buffer); }

          int moduleHash   = std::atoi(moduleStringHash[1].c_str());
          int moduleStatus = std::atoi(moduleString[1].c_str());

          if (moduleStatus<0) { writeCdo->setModuleStatus(moduleHash, 1); }
          else                { writeCdo->setChipStatus(moduleHash, moduleStatus); }
        }
      }
    }
  }

  if (rangeW.stop().isValid() and rangeW.start()>rangeW.stop()) {
    ATH_MSG_FATAL("Invalid intersection rangeW: " << rangeW);
    return StatusCode::FAILURE;
  }

  if (writeHandle.record(rangeW, std::move(writeCdo)).isFailure()) {
    ATH_MSG_FATAL("Could not record PixelDeadMapCondData " << writeHandle.key() << " with EventRange " << rangeW << " into Conditions Store");
    return StatusCode::FAILURE;
  }
  ATH_MSG_INFO("recorded new CDO " << writeHandle.key() << " with range " << rangeW << " into Conditions Store");

  return StatusCode::SUCCESS;
}

