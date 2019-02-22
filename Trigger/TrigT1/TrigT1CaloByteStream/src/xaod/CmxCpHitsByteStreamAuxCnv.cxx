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
// ============================================================================
// xAOD
// ============================================================================
#include "xAODTrigL1Calo/CMXCPHits.h"
#include "xAODTrigL1Calo/CMXCPHitsContainer.h"
#include "xAODTrigL1Calo/CMXCPHitsAuxContainer.h"
// ============================================================================
// Local
// ============================================================================
#include "CmxCpHitsByteStreamAuxCnv.h"
#include "../CpByteStreamV2Tool.h"
#include "../ToString.h"
// ============================================================================

namespace LVL1BS {
CmxCpHitsByteStreamAuxCnv::CmxCpHitsByteStreamAuxCnv(ISvcLocator* svcloc) :
  Converter(storageType(), classID(), svcloc),
  AthMessaging(svcloc != 0 ? msgSvc() : 0, "CmxCpHitsByteStreamAuxCnv"),
  m_name("CmxCpHitsByteStreamAuxCnv"),
  m_readTool("LVL1BS::CpByteStreamV2Tool/CpByteStreamV2Tool")
{
}

const CLID& CmxCpHitsByteStreamAuxCnv::classID() {
  return ClassID_traits<xAOD::CMXCPHitsAuxContainer>::ID();
}

long CmxCpHitsByteStreamAuxCnv::storageType()
{
  return ByteStreamAddress::storageType();
}

//  Init method gets all necessary services etc.

StatusCode CmxCpHitsByteStreamAuxCnv::initialize() {
  ATH_MSG_DEBUG(
    "Initializing " << m_name << " - package version " << PACKAGE_VERSION);

  CHECK(Converter::initialize());
  CHECK(m_readTool.retrieve());

  return StatusCode::SUCCESS;
}

// createObj should create the RDO from bytestream.
StatusCode CmxCpHitsByteStreamAuxCnv::createObj(IOpaqueAddress* pAddr,
    DataObject*& pObj) {
  ATH_MSG_DEBUG("createObj() called");
  // -------------------------------------------------------------------------
  ByteStreamAddress *pBS_Addr = dynamic_cast<ByteStreamAddress *>(pAddr);
  CHECK(pBS_Addr != nullptr);
  // -------------------------------------------------------------------------
  const std::string nm = *(pBS_Addr->par());
  ATH_MSG_DEBUG("Creating Objects " << nm);

  auto aux = new xAOD::CMXCPHitsAuxContainer;
  xAOD::CMXCPHitsContainer cmxCpHitsContainer;
  cmxCpHitsContainer.setStore(aux);
  // -------------------------------------------------------------------------
  DataVector<LVL1::CMXCPHits> cmxCpHitsVector;
  StatusCode sc = m_readTool->convert(nm, &cmxCpHitsVector);
  if (sc.isFailure()) {
    ATH_MSG_ERROR("Failed to create objects");
    delete aux;
    return sc;
  }

  for (auto ch : cmxCpHitsVector) {
    xAOD::CMXCPHits* item = new xAOD::CMXCPHits();
    cmxCpHitsContainer.push_back(item);
    std::vector<uint32_t> hitsVec0(ch->hitsVec0().begin(), ch->hitsVec0().end());
    std::vector<uint32_t> hitsVec1(ch->hitsVec1().begin(), ch->hitsVec1().end());
    std::vector<uint32_t> errorVec0(ch->errorVec0().begin(), ch->errorVec0().end());
    std::vector<uint32_t> errorVec1(ch->errorVec1().begin(), ch->errorVec1().end());

    item->initialize(ch->crate(), ch->cmx(), ch->source(),
                     hitsVec0, hitsVec1, errorVec0, errorVec1, ch->peak());
  }

  // -------------------------------------------------------------------------
  //ATH_MSG_VERBOSE(ToString(cpmCollection));
  ATH_MSG_DEBUG("Number of readed CMXCPHits: " << aux->size());
  // -------------------------------------------------------------------------
  pObj = SG::asStorable(aux);
  return StatusCode::SUCCESS;
}

// createRep should create the bytestream from RDOs.
StatusCode CmxCpHitsByteStreamAuxCnv::createRep(DataObject* /*pObj*/,
    IOpaqueAddress*& /*pAddr*/) {
  return StatusCode::FAILURE;
}

} // end namespace
