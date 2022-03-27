/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "PixelReadoutSpeedAlg.h"
#include "GaudiKernel/EventIDRange.h"

#include "CoralBase/Blob.h"

#include <cstdint>
#include <istream>
#include <map>
#include <string>

PixelReadoutSpeedAlg::PixelReadoutSpeedAlg(const std::string& name, ISvcLocator* pSvcLocator):
  ::AthReentrantAlgorithm(name, pSvcLocator)
{
}

StatusCode PixelReadoutSpeedAlg::initialize() {
  ATH_MSG_DEBUG("PixelReadoutSpeedAlg::initialize()");

  ATH_CHECK(m_readKey.initialize());
  ATH_CHECK(m_writeKey.initialize());

  return StatusCode::SUCCESS;
}

StatusCode PixelReadoutSpeedAlg::execute(const EventContext& ctx) const {
  ATH_MSG_DEBUG("PixelReadoutSpeedAlg::execute()");

  SG::WriteCondHandle<PixelReadoutSpeedData> writeHandle(m_writeKey, ctx);
  if (writeHandle.isValid()) {
    ATH_MSG_DEBUG("CondHandle " << writeHandle.fullKey() << " is already valid.. In theory this should not be called, but may happen if multiple concurrent events are being processed out of order.");
    return StatusCode::SUCCESS; 
  }

  SG::ReadCondHandle<AthenaAttributeList> readHandle(m_readKey, ctx);
  const AthenaAttributeList* readCdo = *readHandle; 
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
  ATH_MSG_INFO("Size of AthenaAttributeList " << readHandle.fullKey() << " readCdo->size()= " << readCdo->size());
  ATH_MSG_INFO("Range of input is " << rangeW);

  // Construct the output Cond Object and fill it in
  std::unique_ptr<PixelReadoutSpeedData> writeCdo(std::make_unique<PixelReadoutSpeedData>());

  const coral::Blob& blob=(*readCdo)["readoutspeed_per_ROD"].data<coral::Blob>();
  const char* p = static_cast<const char*>(blob.startingAddress());
  unsigned int len = blob.size();
  std::map<uint32_t,bool> rodReadoutMap; // save readout speed for each ROD. Is set to 40 MBit (false) by default
  std::string dataString;
  dataString.resize(len);
  for (unsigned int i=0; i!=len; ++i) { dataString[i]=*p++; }

  int pos=0;
  while (dataString.find(',',pos)!=std::string::npos) {
    std::istringstream iss(dataString.substr(pos,dataString.find(',',pos)));
    uint32_t rod;
    iss >> std::hex >> rod;

    const std::string speed = dataString.substr(dataString.find(',',pos)+1,dataString.find('\n',pos)-dataString.find(',',pos)-1);

    if (speed!="SINGLE_40") { rodReadoutMap[rod]=true; }
    else                    { rodReadoutMap[rod]=false; }

    pos = dataString.find('\n',pos)+1;
  }
  writeCdo -> setReadoutMap(rodReadoutMap);

  if (writeHandle.record(rangeW, std::move(writeCdo)).isFailure()) {
    ATH_MSG_FATAL("Could not record PixelReadoutSpeedData " << writeHandle.key() << " with EventRange " << rangeW << " into Conditions Store");
    return StatusCode::FAILURE;
  }
  ATH_MSG_INFO("recorded new CDO " << writeHandle.key() << " with range " << rangeW << " into Conditions Store");

  return StatusCode::SUCCESS;
}

