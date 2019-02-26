/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// ============================================================================
// Includes
// ============================================================================
// STD
// ============================================================================
#include <vector>
#include <cstdint>
#include <memory>
#include <algorithm>
// ============================================================================
// Athena
// ============================================================================
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

// ============================================================================
// TrigT1
// ============================================================================
#include "L1TopoRDO/L1TopoRDOCollection.h"
// ============================================================================
// xAOD
// ============================================================================
#include "xAODTrigL1Calo/L1TopoRawData.h"
#include "xAODTrigL1Calo/L1TopoRawDataContainer.h"
#include "xAODTrigL1Calo/L1TopoRawDataAuxContainer.h"
// ============================================================================
// Local
// ============================================================================
#include "L1TopoByteStreamAuxCnv.h"
#include "L1TopoByteStreamTool.h"
// ============================================================================

namespace LVL1BS {
L1TopoByteStreamAuxCnv::L1TopoByteStreamAuxCnv(ISvcLocator* svcloc)
    : Converter(storageType(), classID(), svcloc),
      AthMessaging(svcloc != 0 ? msgSvc() : 0, "L1TopoByteStreamAuxCnv"),
      m_name("L1TopoByteStreamAuxCnv"),
      m_readTool("L1TopoByteStreamTool/L1TopoByteStreamTool") {}

const CLID& L1TopoByteStreamAuxCnv::classID() {
  return ClassID_traits<xAOD::L1TopoRawDataAuxContainer>::ID();
}

long L1TopoByteStreamAuxCnv::storageType() {
  return ByteStreamAddress::storageType();
}

//  Init method gets all necessary services etc.

StatusCode L1TopoByteStreamAuxCnv::initialize() {
  ATH_MSG_DEBUG("Initializing " << m_name << " - package version "
                                << PACKAGE_VERSION);

  CHECK(Converter::initialize());
  CHECK(m_readTool.retrieve());

  return StatusCode::SUCCESS;
}

// createObj should create the RDO from bytestream.
StatusCode L1TopoByteStreamAuxCnv::createObj(IOpaqueAddress* pAddr,
                                             DataObject*& pObj) {
  ATH_MSG_DEBUG("createObj() called");
  // -------------------------------------------------------------------------
  ByteStreamAddress* pBS_Addr = dynamic_cast<ByteStreamAddress*>(pAddr);
  CHECK(pBS_Addr != nullptr);
  // -------------------------------------------------------------------------
  const std::string nm = *(pBS_Addr->par());
  ATH_MSG_DEBUG("Creating Objects " << nm);

  auto aux = new xAOD::L1TopoRawDataAuxContainer;
  xAOD::L1TopoRawDataContainer topoContainer;
  topoContainer.setStore(aux);
  // -------------------------------------------------------------------------
  L1TopoRDOCollection topoRDOCollection;
  StatusCode sc = m_readTool->convert(nm, &topoRDOCollection);
  if (sc.isFailure()) {
    ATH_MSG_ERROR("Failed to create objects");
    delete aux;
    return sc;
  }

  for (auto topoRDO : topoRDOCollection) {
    xAOD::L1TopoRawData* item = new xAOD::L1TopoRawData();
    topoContainer.push_back(item);
    item->initialize(topoRDO->getDataWords(), topoRDO->getStatusWords(),
                     topoRDO->getError(), topoRDO->getSourceID());
  }

  // -------------------------------------------------------------------------
  // ATH_MSG_VERBOSE(ToString(cpmCollection));
  ATH_MSG_DEBUG("Number of readed Topo: " << aux->size());
  // -------------------------------------------------------------------------
  pObj = SG::asStorable(aux);
  return StatusCode::SUCCESS;
}

// createRep should create the bytestream from RDOs.
StatusCode L1TopoByteStreamAuxCnv::createRep(DataObject* /*pObj*/,
                                             IOpaqueAddress*& /*pAddr*/) {
  return StatusCode::FAILURE;
}

}  // end namespace
