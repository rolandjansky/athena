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
#include "TrigT1CaloEvent/CMXCPTob.h"
// ============================================================================
// xAOD
// ============================================================================
#include "xAODTrigL1Calo/CMXCPTob.h"
#include "xAODTrigL1Calo/CMXCPTobContainer.h"
#include "xAODTrigL1Calo/CMXCPTobAuxContainer.h"
// ============================================================================
// Local
// ============================================================================
#include "CmxCpTobByteStreamAuxCnv.h"
#include "../CpByteStreamV2Tool.h"
#include "../ToString.h"
// ============================================================================

namespace LVL1BS {
CmxCpTobByteStreamAuxCnv::CmxCpTobByteStreamAuxCnv(ISvcLocator* svcloc) :
  Converter(storageType(), classID(), svcloc),
  AthMessaging(svcloc != 0 ? msgSvc() : 0, "CmxCpTobByteStreamAuxCnv"),
  m_name("CmxCpTobByteStreamAuxCnv"),
  m_readTool("LVL1BS::CpByteStreamV2Tool/CpByteStreamV2Tool")
{
}

const CLID& CmxCpTobByteStreamAuxCnv::classID() {
  return ClassID_traits<xAOD::CMXCPTobAuxContainer>::ID();
}

long CmxCpTobByteStreamAuxCnv::storageType()
{
  return ByteStreamAddress::storageType();
}

//  Init method gets all necessary services etc.

#ifndef PACKAGE_VERSION
#define PACKAGE_VERSION "unknown"
#endif
StatusCode CmxCpTobByteStreamAuxCnv::initialize() {
  ATH_MSG_DEBUG(
    "Initializing " << m_name << " - package version " << PACKAGE_VERSION);

  CHECK(Converter::initialize());
  CHECK(m_readTool.retrieve());

  return StatusCode::SUCCESS;
}

// createObj should create the RDO from bytestream.
StatusCode CmxCpTobByteStreamAuxCnv::createObj(IOpaqueAddress* pAddr,
    DataObject*& pObj) {
  ATH_MSG_DEBUG("createObj() called");
  // -------------------------------------------------------------------------
  ByteStreamAddress *pBS_Addr = dynamic_cast<ByteStreamAddress *>(pAddr);
  CHECK(pBS_Addr != nullptr);
  // -------------------------------------------------------------------------
  const std::string nm = *(pBS_Addr->par());
  ATH_MSG_DEBUG("Creating Objects " << nm);

  auto aux = new xAOD::CMXCPTobAuxContainer;
  xAOD::CMXCPTobContainer cpmCollection;
  cpmCollection.setStore(aux);
  // -------------------------------------------------------------------------
  DataVector<LVL1::CMXCPTob> cmxCpTobVector;
  StatusCode sc = m_readTool->convert(nm, &cmxCpTobVector);
  if (sc.isFailure()) {
    ATH_MSG_ERROR("Failed to create objects");
    delete aux;
    return sc;
  }

  for (auto ct : cmxCpTobVector) {
    xAOD::CMXCPTob* item = new xAOD::CMXCPTob();
    cpmCollection.push_back(item);
    std::vector<uint8_t> energyVec(ct->energyVec().begin(), ct->energyVec().end());
    std::vector<uint8_t> isolationVec(ct->isolationVec().begin(), ct->isolationVec().end());
    std::vector<uint32_t> errorVec(ct->errorVec().begin(), ct->errorVec().end());
    std::vector<uint16_t> presenceMapVec(ct->presenceMapVec().begin(), ct->presenceMapVec().end());

    item->initialize(ct->crate(), ct->cmx(),
                     ct->cpm(), ct->chip(), ct->location(),
                     energyVec,
                     isolationVec,
                     errorVec,
                     presenceMapVec,
                     ct->peak());
  }

  // -------------------------------------------------------------------------
  //ATH_MSG_VERBOSE(ToString(cpmCollection));
  ATH_MSG_DEBUG("Number of readed CMXCPTob: " << aux->size());
  // -------------------------------------------------------------------------
  pObj = SG::asStorable(aux);
  return StatusCode::SUCCESS;
}

// createRep should create the bytestream from RDOs.
StatusCode CmxCpTobByteStreamAuxCnv::createRep(DataObject* /*pObj*/,
    IOpaqueAddress*& /*pAddr*/) {
  return StatusCode::FAILURE;
}

} // end namespace
