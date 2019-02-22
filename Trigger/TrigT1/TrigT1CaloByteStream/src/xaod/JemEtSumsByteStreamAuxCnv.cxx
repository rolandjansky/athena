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
#include "TrigT1CaloEvent/JEMEtSums.h"
// ============================================================================
// xAOD
// ============================================================================
#include "xAODTrigL1Calo/JEMEtSums.h"
#include "xAODTrigL1Calo/JEMEtSumsContainer.h"
#include "xAODTrigL1Calo/JEMEtSumsAuxContainer.h"
// ============================================================================
// Local
// ============================================================================
#include "JemEtSumsByteStreamAuxCnv.h"
#include "../JepByteStreamV2Tool.h"
#include "../ToString.h"
// ============================================================================

namespace LVL1BS {
JemEtSumsByteStreamAuxCnv::JemEtSumsByteStreamAuxCnv(ISvcLocator* svcloc) :
  Converter(storageType(), classID(), svcloc),
  AthMessaging(svcloc != 0 ? msgSvc() : 0, "JemEtSumsByteStreamAuxCnv"),
  m_name("JemEtSumsByteStreamAuxCnv"),
  m_readTool("LVL1BS::JepByteStreamV2Tool/JepByteStreamV2Tool")
{
}

const CLID& JemEtSumsByteStreamAuxCnv::classID() {
  return ClassID_traits<xAOD::JEMEtSumsAuxContainer>::ID();
}

long JemEtSumsByteStreamAuxCnv::storageType()
{
  return ByteStreamAddress::storageType();
}

//  Init method gets all necessary services etc.

#ifndef PACKAGE_VERSION
#define PACKAGE_VERSION "unknown"
#endif
StatusCode JemEtSumsByteStreamAuxCnv::initialize() {
  ATH_MSG_DEBUG(
    "Initializing " << m_name << " - package version " << PACKAGE_VERSION);

  CHECK(Converter::initialize());
  CHECK(m_readTool.retrieve());

  return StatusCode::SUCCESS;
}

// createObj should create the RDO from bytestream.
StatusCode JemEtSumsByteStreamAuxCnv::createObj(IOpaqueAddress* pAddr,
    DataObject*& pObj) {
  ATH_MSG_DEBUG("createObj() called");
  // -------------------------------------------------------------------------
  ByteStreamAddress *pBS_Addr = dynamic_cast<ByteStreamAddress *>(pAddr);
  CHECK(pBS_Addr != nullptr);
  // -------------------------------------------------------------------------
  const std::string nm = *(pBS_Addr->par());
  ATH_MSG_DEBUG("Creating Objects " << nm);

  auto aux = new xAOD::JEMEtSumsAuxContainer;
  xAOD::JEMEtSumsContainer container;
  container.setStore(aux);
  // -------------------------------------------------------------------------
  DataVector<LVL1::JEMEtSums> vec;
  StatusCode sc = m_readTool->convert(nm, &vec);
  if (sc.isFailure()) {
    ATH_MSG_ERROR("Failed to create objects");
    delete aux;
    return sc;
  }

  for (auto etSums : vec) {
    xAOD::JEMEtSums* item = new xAOD::JEMEtSums();
    container.push_back(item);

    std::vector<uint16_t> etVec(etSums->EtVec().begin(), etSums->EtVec().end());
    std::vector<uint16_t> exVec(etSums->ExVec().begin(), etSums->ExVec().end());
    std::vector<uint16_t> eyVec(etSums->EyVec().begin(), etSums->EyVec().end());

    item->initialize(etSums->crate(), etSums->module(),
                     etVec, exVec, eyVec,
                     etSums->peak());
  }

  // -------------------------------------------------------------------------
  //ATH_MSG_VERBOSE(ToString(jetCollection));
  ATH_MSG_DEBUG("Number of readed JEMEtSums: " << aux->size());
  // -------------------------------------------------------------------------
  pObj = SG::asStorable(aux);
  return StatusCode::SUCCESS;
}

// createRep should create the bytestream from RDOs.
StatusCode JemEtSumsByteStreamAuxCnv::createRep(DataObject* /*pObj*/,
    IOpaqueAddress*& /*pAddr*/) {
  return StatusCode::FAILURE;
}

} // end namespace
