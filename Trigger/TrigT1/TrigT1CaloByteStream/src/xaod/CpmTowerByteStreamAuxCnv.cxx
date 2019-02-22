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
#include "TrigT1CaloEvent/CMXCPHits.h"
#include "TrigT1CaloEvent/CMXCPTob.h"
#include "TrigT1CaloEvent/CPMTower.h"
// ============================================================================
// xAOD
// ============================================================================
#include "xAODTrigL1Calo/CPMTower.h"
#include "xAODTrigL1Calo/CPMTowerContainer.h"
#include "xAODTrigL1Calo/CPMTowerAuxContainer.h"
// ============================================================================
// Local
// ============================================================================
#include "CpmTowerByteStreamAuxCnv.h"
#include "../CpByteStreamV2Tool.h"
#include "../ToString.h"
// ============================================================================

namespace LVL1BS {
CpmTowerByteStreamAuxCnv::CpmTowerByteStreamAuxCnv(ISvcLocator* svcloc) :
  Converter(storageType(), classID(), svcloc),
  AthMessaging(svcloc != 0 ? msgSvc() : 0, "CpmTowerByteStreamAuxCnv"),
  m_name("CpmTowerByteStreamAuxCnv"),
  m_cpmReadTool("LVL1BS::CpByteStreamV2Tool/CpByteStreamV2Tool")
{
}

const CLID& CpmTowerByteStreamAuxCnv::classID() {
  return ClassID_traits<xAOD::CPMTowerAuxContainer>::ID();
}

long CpmTowerByteStreamAuxCnv::storageType()
{
  return ByteStreamAddress::storageType();
}

//  Init method gets all necessary services etc.

#ifndef PACKAGE_VERSION
#define PACKAGE_VERSION "unknown"
#endif
StatusCode CpmTowerByteStreamAuxCnv::initialize() {
  ATH_MSG_DEBUG(
    "Initializing " << m_name << " - package version " << PACKAGE_VERSION);

  CHECK(Converter::initialize());
  CHECK(m_cpmReadTool.retrieve());

  return StatusCode::SUCCESS;
}

// createObj should create the RDO from bytestream.
StatusCode CpmTowerByteStreamAuxCnv::createObj(IOpaqueAddress* pAddr,
    DataObject*& pObj) {
  ATH_MSG_DEBUG("createObj() called");
  // -------------------------------------------------------------------------
  ByteStreamAddress *pBS_Addr = dynamic_cast<ByteStreamAddress *>(pAddr);
  CHECK(pBS_Addr != nullptr);
  // -------------------------------------------------------------------------
  const std::string nm = *(pBS_Addr->par());
  ATH_MSG_DEBUG("Creating Objects " << nm);

  auto aux = new xAOD::CPMTowerAuxContainer;
  xAOD::CPMTowerContainer cpmCollection;
  cpmCollection.setStore(aux);
  // -------------------------------------------------------------------------
  DataVector<LVL1::CPMTower> cpmTowerVector;
  StatusCode sc = m_cpmReadTool->convert(nm, &cpmTowerVector);
  if (sc.isFailure()) {
    ATH_MSG_ERROR("Failed to create objects");
    delete aux;
    return sc;
  }

  for (auto ct : cpmTowerVector) {
    xAOD::CPMTower* item = new xAOD::CPMTower();
    cpmCollection.push_back(item);
    std::vector<uint8_t> emEnergyVec(ct->emEnergyVec().begin(), ct->emEnergyVec().end());
    std::vector<uint8_t> hadEnergyVec(ct->hadEnergyVec().begin(), ct->hadEnergyVec().end());
    std::vector<uint32_t> emErrorVec(ct->emErrorVec().begin(), ct->emErrorVec().end());
    std::vector<uint32_t> hadErrorVec(ct->hadErrorVec().begin(), ct->hadErrorVec().end());

    item->initialize(ct->eta(), ct->phi(),
                     emEnergyVec,
                     hadEnergyVec,
                     emErrorVec,
                     hadErrorVec,
                     uint8_t(ct->peak()));
  }

  // -------------------------------------------------------------------------
  //ATH_MSG_VERBOSE(ToString(cpmCollection));
  ATH_MSG_DEBUG("Number of readed CPM towers: " << aux->size());
  // -------------------------------------------------------------------------
  pObj = SG::asStorable(aux);
  return StatusCode::SUCCESS;
}

// createRep should create the bytestream from RDOs.
StatusCode CpmTowerByteStreamAuxCnv::createRep(DataObject* /*pObj*/,
    IOpaqueAddress*& /*pAddr*/) {
  return StatusCode::FAILURE;
}

} // end namespace
