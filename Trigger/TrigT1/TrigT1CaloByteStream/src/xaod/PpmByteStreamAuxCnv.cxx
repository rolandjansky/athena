/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <vector>
#include <cstdint>
#include <memory>

#include "ByteStreamCnvSvcBase/ByteStreamAddress.h"
#include "ByteStreamCnvSvcBase/IByteStreamEventAccess.h"
#include "ByteStreamCnvSvcBase/IROBDataProviderSvc.h"

#include "ByteStreamData/RawEvent.h"
#include "ByteStreamData/ROBData.h"

#include "AthenaKernel/errorcheck.h"
#include "GaudiKernel/CnvFactory.h"
#include "GaudiKernel/DataObject.h"
#include "GaudiKernel/IOpaqueAddress.h"
#include "GaudiKernel/IRegistry.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/StatusCode.h"

#include "SGTools/ClassID_traits.h"
#include "SGTools/StorableConversions.h"
#include "StoreGate/StoreGateSvc.h"

#include "xAODTrigL1Calo/TriggerTower.h"
#include "xAODTrigL1Calo/TriggerTowerContainer.h"
#include "xAODTrigL1Calo/TriggerTowerAuxContainer.h"

#include "PpmByteStreamAuxCnv.h"
#include "../ToString.h"
#include "L1CaloByteStreamReadTool.h"

namespace LVL1BS {
PpmByteStreamAuxCnv::PpmByteStreamAuxCnv(ISvcLocator* svcloc) :
    Converter(ByteStream_StorageType, classID(), svcloc),
    AthMessaging(svcloc != 0 ? msgSvc() : 0, "PpmByteStreamAuxCnv"),
    m_name("PpmByteStreamAuxCnv"),
    m_readTool("LVL1BS::L1CaloByteStreamReadTool/L1CaloByteStreamReadTool") {

}

const CLID& PpmByteStreamAuxCnv::classID() {
  return ClassID_traits<xAOD::TriggerTowerAuxContainer>::ID();
}

//  Init method gets all necessary services etc.

#ifndef PACKAGE_VERSION
#define PACKAGE_VERSION "unknown"
#endif
StatusCode PpmByteStreamAuxCnv::initialize() {
  ATH_MSG_DEBUG(
      "Initializing " << m_name << " - package version " << PACKAGE_VERSION);

  CHECK(Converter::initialize());
  CHECK(m_readTool.retrieve());

  return StatusCode::SUCCESS;
}

// createObj should create the RDO from bytestream.
StatusCode PpmByteStreamAuxCnv::createObj(IOpaqueAddress* pAddr,
    DataObject*& pObj) {
  ATH_MSG_DEBUG("createObj() called");
  // -------------------------------------------------------------------------
  ByteStreamAddress *pBS_Addr = dynamic_cast<ByteStreamAddress *>(pAddr);
  CHECK(pBS_Addr != nullptr);
  // -------------------------------------------------------------------------
  const std::string nm = *(pBS_Addr->par());
  ATH_MSG_DEBUG("Creating Objects " << nm);

  auto aux = new xAOD::TriggerTowerAuxContainer();
  xAOD::TriggerTowerContainer ttCollection;
  ttCollection.setStore(aux);

  StatusCode sc = m_readTool->convert(nm, &ttCollection);
  if (sc.isFailure()) {
      ATH_MSG_ERROR("Failed to create Objects");
      delete aux;
      return sc;
    }
  ATH_MSG_VERBOSE(ToString(ttCollection));
  ATH_MSG_DEBUG("Number of readed objects: " << aux->size());
  pObj = SG::asStorable(aux);
  
  return StatusCode::SUCCESS;
}

// createRep should create the bytestream from RDOs.
StatusCode PpmByteStreamAuxCnv::createRep(DataObject* /*pObj*/,
    IOpaqueAddress*& /*pAddr*/) {
  return StatusCode::FAILURE;
}

} // end namespace
