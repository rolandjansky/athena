/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include <vector>
#include <stdint.h>

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

#include "SGTools/ClassID_traits.h"
#include "SGTools/StorableConversions.h"
#include "StoreGate/StoreGateSvc.h"

#include "xAODTrigL1Calo/L1TopoRawDataContainer.h"
#include "xAODTrigL1Calo/L1TopoRawDataAuxContainer.h"

#include "L1TopoByteStreamxAODCnv.h"

namespace LVL1BS {

L1TopoByteStreamxAODCnv::L1TopoByteStreamxAODCnv(ISvcLocator* svcloc)
    : Converter(storageType(), classID(), svcloc),
      AthMessaging(svcloc != 0 ? msgSvc() : 0, "L1TopoByteStreamxAODCnv"),
      m_name("L1TopoByteStreamxAODCnv") {}

// CLID

const CLID& L1TopoByteStreamxAODCnv::classID() {
  return ClassID_traits<xAOD::L1TopoRawDataContainer>::ID();
}

long L1TopoByteStreamxAODCnv::storageType() {
  return ByteStreamAddress::storageType();
}

//  Init method gets all necessary services etc.

StatusCode L1TopoByteStreamxAODCnv::initialize() {
  ATH_MSG_DEBUG("Initializing " << m_name << " - package version "
                                << PACKAGE_VERSION);

  CHECK(Converter::initialize());
  return StatusCode::SUCCESS;
}

// createObj should create the RDO from bytestream.

StatusCode L1TopoByteStreamxAODCnv::createObj(IOpaqueAddress* pAddr,
                                              DataObject*& pObj) {
  ATH_MSG_DEBUG("createObj() called");
  // -------------------------------------------------------------------------
  ByteStreamAddress* pBS_Addr = dynamic_cast<ByteStreamAddress*>(pAddr);
  CHECK(pBS_Addr != nullptr);
  // -------------------------------------------------------------------------
  const std::string nm = *(pBS_Addr->par());
  const std::string nmAux = nm + "Aux.";
  ATH_MSG_DEBUG("Creating xAOD::L1TopoRawDataContainer interface objects '"
                << nm << "'");

  xAOD::L1TopoRawDataContainer* const topoCollection =
      new xAOD::L1TopoRawDataContainer;

  // Create link with AUX container
  DataLink<xAOD::L1TopoRawDataAuxContainer> link(nmAux);
  ATH_MSG_DEBUG("Creating store with data link to '" << nmAux);

  for (size_t i = 0; i < link->size(); ++i) {
    topoCollection->push_back(new xAOD::L1TopoRawData());
  }
  // ==========================================================================
  topoCollection->setStore(link);
  pObj = SG::asStorable(topoCollection);
  ATH_MSG_DEBUG(
      "Number of xAOD::L1TopoRawData created: " << topoCollection->size());

  return StatusCode::SUCCESS;
}

// createRep should create the bytestream from RDOs.

StatusCode L1TopoByteStreamxAODCnv::createRep(DataObject* /*pObj*/,
                                              IOpaqueAddress*& /*pAddr*/) {
  return StatusCode::FAILURE;
}

}  // end namespace
