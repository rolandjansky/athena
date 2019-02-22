/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
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

#include "AthenaKernel/ClassID_traits.h"
#include "AthenaKernel/StorableConversions.h"
#include "StoreGate/StoreGateSvc.h"

// ============================================================================
// TrigT1
// ============================================================================
#include "TrigT1CaloEvent/RODHeader.h"
// ============================================================================
// xAOD
// ============================================================================
#include "xAODTrigL1Calo/RODHeader.h"
#include "xAODTrigL1Calo/RODHeaderContainer.h"
#include "xAODTrigL1Calo/RODHeaderAuxContainer.h"
// ============================================================================
// Local
// ============================================================================
#include "RodHeaderByteStreamAuxCnv.h"
#include "../RodHeaderByteStreamTool.h"
#include "../ToString.h"
// ============================================================================

namespace LVL1BS {
RodHeaderByteStreamAuxCnv::RodHeaderByteStreamAuxCnv(ISvcLocator* svcloc) :
  Converter(storageType(), classID(), svcloc),
  AthMessaging(svcloc != 0 ? msgSvc() : 0, "RodHeaderByteStreamAuxCnv"),
  m_name("RodHeaderByteStreamAuxCnv"),
  m_readTool("LVL1BS::RodHeaderByteStreamTool/RodHeaderByteStreamTool")
{
}

const CLID& RodHeaderByteStreamAuxCnv::classID() {
  return ClassID_traits<xAOD::RODHeaderAuxContainer>::ID();
}

long RodHeaderByteStreamAuxCnv::storageType()
{
  return ByteStreamAddress::storageType();
}

//  Init method gets all necessary services etc.

StatusCode RodHeaderByteStreamAuxCnv::initialize() {
  ATH_MSG_DEBUG(
    "Initializing " << m_name << " - package version " << PACKAGE_VERSION);

  CHECK(Converter::initialize());
  CHECK(m_readTool.retrieve());

  return StatusCode::SUCCESS;
}

// createObj should create the RDO from bytestream.
StatusCode RodHeaderByteStreamAuxCnv::createObj(IOpaqueAddress* pAddr,
    DataObject*& pObj) {
  ATH_MSG_DEBUG("createObj() called");
  // -------------------------------------------------------------------------
  ByteStreamAddress *pBS_Addr = dynamic_cast<ByteStreamAddress *>(pAddr);
  CHECK(pBS_Addr != nullptr);
  // -------------------------------------------------------------------------
  const std::string nm = *(pBS_Addr->par());
  ATH_MSG_DEBUG("Creating Objects " << nm);

  auto aux = new xAOD::RODHeaderAuxContainer;
  xAOD::RODHeaderContainer container;
  container.setStore(aux);
  // -------------------------------------------------------------------------
  DataVector<LVL1::RODHeader> vec;
  StatusCode sc = m_readTool->convert(nm, &vec);
  if (sc.isFailure()) {
    ATH_MSG_ERROR("Failed to create objects");
    delete aux;
    return sc;
  }

  for (auto source : vec) {
    xAOD::RODHeader* item = new xAOD::RODHeader();
    container.push_back(item);
    item->initialize(source->version(), source->sourceID(), source->run(),
      source->l1ID(), source->bunchCrossing(), source->l1TriggerType(),
      source->detEventType(), source->statusWords(), source->payloadSize());
  }

  // -------------------------------------------------------------------------
  //ATH_MSG_VERBOSE(ToString(container));
  ATH_MSG_DEBUG("Number of readed RODHeaders: " << aux->size());
  // -------------------------------------------------------------------------
  pObj = SG::asStorable(aux);
  return StatusCode::SUCCESS;
}

// createRep should create the bytestream from RDOs.
StatusCode RodHeaderByteStreamAuxCnv::createRep(DataObject* /*pObj*/,
    IOpaqueAddress*& /*pAddr*/) {
  return StatusCode::FAILURE;
}

} // end namespace
