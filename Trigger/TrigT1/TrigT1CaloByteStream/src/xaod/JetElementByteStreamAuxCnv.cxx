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
#include "TrigT1CaloEvent/JetElement.h"
// ============================================================================
// xAOD
// ============================================================================
#include "xAODTrigL1Calo/JetElement.h"
#include "xAODTrigL1Calo/JetElementContainer.h"
#include "xAODTrigL1Calo/JetElementAuxContainer.h"
// ============================================================================
// Local
// ============================================================================
#include "JetElementByteStreamAuxCnv.h"
#include "../JepByteStreamV2Tool.h"
#include "../ToString.h"
// ============================================================================

namespace LVL1BS {
JetElementByteStreamAuxCnv::JetElementByteStreamAuxCnv(ISvcLocator* svcloc) :
  Converter(storageType(), classID(), svcloc),
  AthMessaging(svcloc != 0 ? msgSvc() : 0, "JetElementByteStreamAuxCnv"),
  m_name("JetElementByteStreamAuxCnv"),
  m_readTool("LVL1BS::JepByteStreamV2Tool/JepByteStreamV2Tool")
{
}

const CLID& JetElementByteStreamAuxCnv::classID() {
  return ClassID_traits<xAOD::JetElementAuxContainer>::ID();
}

long JetElementByteStreamAuxCnv::storageType()
{
  return ByteStreamAddress::storageType();
}

//  Init method gets all necessary services etc.

#ifndef PACKAGE_VERSION
#define PACKAGE_VERSION "unknown"
#endif
StatusCode JetElementByteStreamAuxCnv::initialize() {
  ATH_MSG_DEBUG(
    "Initializing " << m_name << " - package version " << PACKAGE_VERSION);

  CHECK(Converter::initialize());
  CHECK(m_readTool.retrieve());

  return StatusCode::SUCCESS;
}

// createObj should create the RDO from bytestream.
StatusCode JetElementByteStreamAuxCnv::createObj(IOpaqueAddress* pAddr,
    DataObject*& pObj) {
  ATH_MSG_DEBUG("createObj() called");
  // -------------------------------------------------------------------------
  ByteStreamAddress *pBS_Addr = dynamic_cast<ByteStreamAddress *>(pAddr);
  CHECK(pBS_Addr != nullptr);
  // -------------------------------------------------------------------------
  const std::string nm = *(pBS_Addr->par());
  ATH_MSG_DEBUG("Creating Objects " << nm);

  auto aux = new xAOD::JetElementAuxContainer;
  xAOD::JetElementContainer container;
  container.setStore(aux);
  // -------------------------------------------------------------------------
  DataVector<LVL1::JetElement> vec;
  StatusCode sc = m_readTool->convert(nm, &vec);
  if (sc.isFailure()) {
    ATH_MSG_ERROR("Failed to create objects");
    delete aux;
    return sc;
  }

  for (auto source : vec) {
    xAOD::JetElement* item = new xAOD::JetElement();
    container.push_back(item);
    
    std::vector<uint16_t> emJetElementETVec(
      source->emEnergyVec().begin(),
      source->emEnergyVec().end()
    );

    std::vector<uint16_t> hadJetElementETVec(
      source->hadEnergyVec().begin(),
      source->hadEnergyVec().end()
    );

    std::vector<uint32_t> emJetElementError(
      source->emErrorVec().begin(),
      source->emErrorVec().end()
    );

    std::vector<uint32_t> hadJetElementError(
      source->hadErrorVec().begin(),
      source->hadErrorVec().end()
    );

    std::vector<uint32_t> linkErrorVec(
      source->linkErrorVec().begin(),
      source->linkErrorVec().end()
    );

    item->initialize(
      source->eta(),
      source->phi(),
      source->key(),
      emJetElementETVec,
      hadJetElementETVec,
      emJetElementError,
      hadJetElementError,
      linkErrorVec,
      source->peak()
    );
  }

  // -------------------------------------------------------------------------
  //ATH_MSG_VERBOSE(ToString(container));
  ATH_MSG_DEBUG("Number of readed JetElement: " << aux->size());
  // -------------------------------------------------------------------------
  pObj = SG::asStorable(aux);
  return StatusCode::SUCCESS;
}

// createRep should create the bytestream from RDOs.
StatusCode JetElementByteStreamAuxCnv::createRep(DataObject* /*pObj*/,
    IOpaqueAddress*& /*pAddr*/) {
  return StatusCode::FAILURE;
}

} // end namespace
