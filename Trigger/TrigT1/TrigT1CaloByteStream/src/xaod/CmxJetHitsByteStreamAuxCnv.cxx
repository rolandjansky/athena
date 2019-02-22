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
#include "TrigT1CaloEvent/CMXJetHits.h"
// ============================================================================
// xAOD
// ============================================================================
#include "xAODTrigL1Calo/CMXJetHits.h"
#include "xAODTrigL1Calo/CMXJetHitsContainer.h"
#include "xAODTrigL1Calo/CMXJetHitsAuxContainer.h"
// ============================================================================
// Local
// ============================================================================
#include "CmxJetHitsByteStreamAuxCnv.h"
#include "../JepByteStreamV2Tool.h"
#include "../ToString.h"
// ============================================================================

namespace LVL1BS {
CmxJetHitsByteStreamAuxCnv::CmxJetHitsByteStreamAuxCnv(ISvcLocator* svcloc) :
  Converter(storageType(), classID(), svcloc),
  AthMessaging(svcloc != 0 ? msgSvc() : 0, "CmxJetHitsByteStreamAuxCnv"),
  m_name("CmxJetHitsByteStreamAuxCnv"),
  m_readTool("LVL1BS::JepByteStreamV2Tool/JepByteStreamV2Tool")
{
}

const CLID& CmxJetHitsByteStreamAuxCnv::classID() {
  return ClassID_traits<xAOD::CMXJetHitsAuxContainer>::ID();
}

long CmxJetHitsByteStreamAuxCnv::storageType()
{
  return ByteStreamAddress::storageType();
}

//  Init method gets all necessary services etc.

#ifndef PACKAGE_VERSION
#define PACKAGE_VERSION "unknown"
#endif
StatusCode CmxJetHitsByteStreamAuxCnv::initialize() {
  ATH_MSG_DEBUG(
    "Initializing " << m_name << " - package version " << PACKAGE_VERSION);

  CHECK(Converter::initialize());
  CHECK(m_readTool.retrieve());

  return StatusCode::SUCCESS;
}

// createObj should create the RDO from bytestream.
StatusCode CmxJetHitsByteStreamAuxCnv::createObj(IOpaqueAddress* pAddr,
    DataObject*& pObj) {
  ATH_MSG_DEBUG("createObj() called");
  // -------------------------------------------------------------------------
  ByteStreamAddress *pBS_Addr = dynamic_cast<ByteStreamAddress *>(pAddr);
  CHECK(pBS_Addr != nullptr);
  // -------------------------------------------------------------------------
  const std::string nm = *(pBS_Addr->par());
  ATH_MSG_DEBUG("Creating Objects " << nm);

  auto aux = new xAOD::CMXJetHitsAuxContainer;
  xAOD::CMXJetHitsContainer jetCollection;
  jetCollection.setStore(aux);
  // -------------------------------------------------------------------------
  DataVector<LVL1::CMXJetHits> cmxJetHitsVector;
  StatusCode sc = m_readTool->convert(nm, &cmxJetHitsVector);
  if (sc.isFailure()) {
    ATH_MSG_ERROR("Failed to create objects");
    delete aux;
    return sc;
  }

  for (auto jet : cmxJetHitsVector) {
    xAOD::CMXJetHits* item = new xAOD::CMXJetHits();
    jetCollection.push_back(item);

    std::vector<uint32_t> hitsVec0(jet->hitsVec0().begin(), jet->hitsVec0().end());
    std::vector<uint32_t> hitsVec1(jet->hitsVec1().begin(), jet->hitsVec1().end());
    std::vector<uint32_t> errorVec0(jet->errorVec0().begin(), jet->errorVec0().end());
    std::vector<uint32_t> errorVec1(jet->errorVec1().begin(), jet->errorVec1().end());

    item->initialize(jet->crate(), jet->source(),
                     hitsVec0, hitsVec1, errorVec0, errorVec1, jet->peak());
  }

  // -------------------------------------------------------------------------
  //ATH_MSG_VERBOSE(ToString(jetCollection));
  ATH_MSG_DEBUG("Number of readed CMXJetHits: " << aux->size());
  // -------------------------------------------------------------------------
  pObj = SG::asStorable(aux);
  return StatusCode::SUCCESS;
}

// createRep should create the bytestream from RDOs.
StatusCode CmxJetHitsByteStreamAuxCnv::createRep(DataObject* /*pObj*/,
    IOpaqueAddress*& /*pAddr*/) {
  return StatusCode::FAILURE;
}

} // end namespace
