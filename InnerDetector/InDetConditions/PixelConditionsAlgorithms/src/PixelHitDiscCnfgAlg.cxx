/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "PixelHitDiscCnfgAlg.h"
#include "GaudiKernel/EventIDRange.h"

#include "CoralBase/Blob.h"

#include <cstdint>
#include <istream>
#include <map>
#include <string>

PixelHitDiscCnfgAlg::PixelHitDiscCnfgAlg(const std::string& name, ISvcLocator* pSvcLocator):
  ::AthReentrantAlgorithm(name, pSvcLocator)
{
}

StatusCode PixelHitDiscCnfgAlg::initialize() {
  ATH_MSG_DEBUG("PixelHitDiscCnfgAlg::initialize()");

  ATH_CHECK(m_moduleDataKey.initialize());
  ATH_CHECK(m_readKey.initialize());
  ATH_CHECK(m_writeKey.initialize());

  return StatusCode::SUCCESS;
}

StatusCode PixelHitDiscCnfgAlg::execute(const EventContext& ctx) const {
  ATH_MSG_DEBUG("PixelHitDiscCnfgAlg::execute()");

  SG::WriteCondHandle<PixelHitDiscCnfgData> writeHandle(m_writeKey, ctx);
  if (writeHandle.isValid()) {
    ATH_MSG_DEBUG("CondHandle " << writeHandle.fullKey() << " is already valid.. In theory this should not be called, but may happen if multiple concurrent events are being processed out of order.");
    return StatusCode::SUCCESS; 
  }

  // Construct the output Cond Object and fill it in
  std::unique_ptr<PixelHitDiscCnfgData> writeCdo(std::make_unique<PixelHitDiscCnfgData>());

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
  ATH_MSG_DEBUG("Size of AthenaAttributeList " << readHandle.fullKey() << " readCdo->size()= " << readCdo->size());
  ATH_MSG_DEBUG("Range of input is " << rangeW);

  const coral::Blob& blob=(*readCdo)["HitDiscCnfgData"].data<coral::Blob>();
  const uint32_t* p = static_cast<const uint32_t*>(blob.startingAddress());

  uint32_t cooldata;
  unsigned int len = blob.size()/sizeof(uint32_t);
  ATH_MSG_DEBUG("blob.size() = " << blob.size() << ", len = " << len);

  int nhitPL = 0;
  int hitPL = 0;
  int nhit3D = 0;
  int hit3D = 0;
  for (unsigned int i = 0; i < len; ++i) {

    cooldata = *p++;

    ATH_MSG_DEBUG("Got hitdisccnfgData[" << i << "] = 0x" << std::hex << cooldata << std::dec);

    // The implementation below uses one common value, one common 3D value,
    // and an exception list of individual FEs, in order to save DB space.
    // Here we convert this into only one common value and an exception list, i.e.
    // if the 3D FEs have a different common value they are all added to the exception list
    if ((cooldata & 0x8FFFFFFF)==0x0) { // undefined.
      ATH_MSG_DEBUG("Setting common HitDiscCnfg value to " << ((cooldata&0x30000000) >> 28));
      nhitPL++;
      hitPL+=((cooldata&0x30000000) >> 28);
    }
    // Update all 3D sensors with a common value, identified by 0xZ0000000, where Z = 10hh
    else if ((cooldata & 0x8FFFFFFF)==0x80000000) {
      ATH_MSG_DEBUG("Setting common 3D HitDiscCnfg value to " << ((cooldata&0x30000000) >> 28));
      nhit3D++;
      hit3D+=((cooldata&0x30000000) >> 28);
    }
    // Update a single link, 0xHLDDRRRR
    else {
      ATH_MSG_DEBUG("Setting HitDiscCnfg value to " << ((cooldata&0x30000000) >> 28)
          << " for ROB 0x" << std::hex << (cooldata & 0xFFFFFF)
          << ", link " << std::dec << ((cooldata & 0xF000000) >> 24));
      nhitPL++;
      hitPL+=((cooldata&0x30000000) >> 28);
    }
  }
  // Take average.
  double aveHitPL = 1.0*SG::ReadCondHandle<PixelModuleData>(m_moduleDataKey,ctx)->getFEI4HitDiscConfig(0,0);
  double aveHit3D = 1.0*SG::ReadCondHandle<PixelModuleData>(m_moduleDataKey,ctx)->getFEI4HitDiscConfig(0,0);

  if (nhitPL) { aveHitPL = hitPL/(1.0*nhitPL); }

  if (nhit3D) { aveHit3D = hit3D/(1.0*nhit3D); }
  else        { aveHit3D = aveHitPL; }

  if (aveHitPL-(double)(int)aveHitPL<0.5) { aveHitPL=(double)(int)aveHitPL; }
  else                                    { aveHitPL=(double)(int)(aveHitPL+0.9); }

  if (aveHit3D-(double)(int)aveHit3D<0.5) { aveHit3D=(double)(int)aveHit3D; }
  else                                    { aveHit3D=(double)(int)(aveHit3D+0.9); }

  writeCdo -> setHitDiscCnfgPL((int)aveHitPL);
  writeCdo -> setHitDiscCnfg3D((int)aveHit3D);

  if (writeHandle.record(rangeW, std::move(writeCdo)).isFailure()) {
    ATH_MSG_FATAL("Could not record PixelHitDiscCnfgData " << writeHandle.key() << " with EventRange " << rangeW << " into Conditions Store");
    return StatusCode::FAILURE;
  }
  ATH_MSG_INFO("recorded new CDO " << writeHandle.key() << " with range " << rangeW << " into Conditions Store");

  return StatusCode::SUCCESS;
}

