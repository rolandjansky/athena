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
#include "GaudiKernel/DataObject.h"
#include "GaudiKernel/IOpaqueAddress.h"
#include "GaudiKernel/IRegistry.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/StatusCode.h"

#include "AthenaKernel/ClassID_traits.h"
#include "AthenaKernel/StorableConversions.h"
#include "StoreGate/StoreGateSvc.h"

#include "xAODTrigL1Calo/TriggerTower.h"
#include "xAODTrigL1Calo/TriggerTowerContainer.h"
#include "xAODTrigL1Calo/TriggerTowerAuxContainer.h"

#include "ZdcByteStream/ZdcByteStreamReadV1V2Tool.h"

#include "PpmByteStreamAuxCnv.h"
#include "../ToString.h"
#include "PpmByteStreamReadV1V2Tool.h"

namespace LVL1BS {
PpmByteStreamAuxCnv::PpmByteStreamAuxCnv(ISvcLocator* svcloc) :
    Converter(storageType(), classID(), svcloc),
    AthMessaging(svcloc != 0 ? msgSvc() : 0, "PpmByteStreamAuxCnv"),
    m_name("PpmByteStreamAuxCnv"),
    m_readTool("LVL1BS::PpmByteStreamReadV1V2Tool/PpmByteStreamReadV1V2Tool"),
    m_zdcTool("ZdcByteStreamReadV1V2Tool/ZdcByteStreamReadV1V2Tool") {

}

const CLID& PpmByteStreamAuxCnv::classID() {
  return ClassID_traits<xAOD::TriggerTowerAuxContainer>::ID();
}

long PpmByteStreamAuxCnv::storageType()
{
  return ByteStreamAddress::storageType();
}

//  Init method gets all necessary services etc.

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

  auto aux = new xAOD::TriggerTowerAuxContainer;
  xAOD::TriggerTowerContainer ttCollection;
  ttCollection.setStore(aux);
  // -------------------------------------------------------------------------
  StatusCode sc;
  if (nm.substr(0,3) != "Zdc")
    {
      ATH_MSG_DEBUG("converting for L1Calo: " << nm );
      sc = m_readTool->convert(nm, &ttCollection);
    }
  else
    { 
      ATH_MSG_DEBUG("converting for ZDC: " << nm );
      sc = m_zdcTool->convert(nm, &ttCollection);      
    }
  if (sc.isFailure()) {
    ATH_MSG_ERROR("Failed to create Objects" << nm);
    delete aux;
    return sc;
  }
  // -------------------------------------------------------------------------
  ATH_MSG_VERBOSE(ToString(ttCollection));
  ATH_MSG_DEBUG("Number of readed objects: " << aux->size());
  // -------------------------------------------------------------------------
  pObj = SG::asStorable(aux);
  return StatusCode::SUCCESS;
}

// createRep should create the bytestream from RDOs.
StatusCode PpmByteStreamAuxCnv::createRep(DataObject* /*pObj*/,
    IOpaqueAddress*& /*pAddr*/) {
  return StatusCode::FAILURE;
}

} // end namespace
