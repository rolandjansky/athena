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
#include "TrigT1CaloEvent/CMXJetTob.h"
// ============================================================================
// xAOD
// ============================================================================
#include "xAODTrigL1Calo/CMXJetTob.h"
#include "xAODTrigL1Calo/CMXJetTobContainer.h"
#include "xAODTrigL1Calo/CMXJetTobAuxContainer.h"
// ============================================================================
// Local
// ============================================================================
#include "CmxJetTobByteStreamAuxCnv.h"
#include "../JepByteStreamV2Tool.h"
#include "../ToString.h"
// ============================================================================

namespace LVL1BS {
CmxJetTobByteStreamAuxCnv::CmxJetTobByteStreamAuxCnv(ISvcLocator* svcloc) :
  Converter(storageType(), classID(), svcloc),
  AthMessaging(svcloc != 0 ? msgSvc() : 0, "CmxJetTobByteStreamAuxCnv"),
  m_name("CmxJetTobByteStreamAuxCnv"),
  m_readTool("LVL1BS::JepByteStreamV2Tool/JepByteStreamV2Tool")
{
}

const CLID& CmxJetTobByteStreamAuxCnv::classID() {
  return ClassID_traits<xAOD::CMXJetTobAuxContainer>::ID();
}

long CmxJetTobByteStreamAuxCnv::storageType()
{
  return ByteStreamAddress::storageType();
}

//  Init method gets all necessary services etc.

StatusCode CmxJetTobByteStreamAuxCnv::initialize() {
  ATH_MSG_DEBUG(
    "Initializing " << m_name << " - package version " << PACKAGE_VERSION);

  CHECK(Converter::initialize());
  CHECK(m_readTool.retrieve());

  return StatusCode::SUCCESS;
}

// createObj should create the RDO from bytestream.
StatusCode CmxJetTobByteStreamAuxCnv::createObj(IOpaqueAddress* pAddr,
    DataObject*& pObj) {
  ATH_MSG_DEBUG("createObj() called");
  // -------------------------------------------------------------------------
  ByteStreamAddress *pBS_Addr = dynamic_cast<ByteStreamAddress *>(pAddr);
  CHECK(pBS_Addr != nullptr);
  // -------------------------------------------------------------------------
  const std::string nm = *(pBS_Addr->par());
  ATH_MSG_DEBUG("Creating Objects " << nm);

  auto aux = new xAOD::CMXJetTobAuxContainer;
  xAOD::CMXJetTobContainer container;
  container.setStore(aux);
  // -------------------------------------------------------------------------
  DataVector<LVL1::CMXJetTob> vec;
  StatusCode sc = m_readTool->convert(nm, &vec);
  if (sc.isFailure()) {
    ATH_MSG_ERROR("Failed to create objects");
    delete aux;
    return sc;
  }

  for (auto jet : vec) {
    xAOD::CMXJetTob* item = new xAOD::CMXJetTob();
    container.push_back(item);

    std::vector<uint16_t> energyLarge(jet->energyLgVec().begin(), jet->energyLgVec().end());
    std::vector<uint16_t> energySmall(jet->energySmVec().begin(), jet->energySmVec().end());
    std::vector<uint32_t> errorVec(jet->errorVec().begin(), jet->errorVec().end());
    std::vector<uint16_t> presenceMapVec(jet->presenceMapVec().begin(), jet->presenceMapVec().end());

    item->initialize(jet->crate(), jet->jem(),
                     jet->frame(), jet->location(),
                     energyLarge,
                     energySmall,
                     errorVec,
                     presenceMapVec,
                     jet->peak());
  }

  // -------------------------------------------------------------------------
  //ATH_MSG_VERBOSE(ToString(container));
  ATH_MSG_DEBUG("Number of readed CMXJetTob: " << aux->size());
  // -------------------------------------------------------------------------
  pObj = SG::asStorable(aux);
  return StatusCode::SUCCESS;
}

// createRep should create the bytestream from RDOs.
StatusCode CmxJetTobByteStreamAuxCnv::createRep(DataObject* /*pObj*/,
    IOpaqueAddress*& /*pAddr*/) {
  return StatusCode::FAILURE;
}

} // end namespace
