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
#include "TrigT1CaloEvent/CMXRoI.h"
// ============================================================================
// xAOD
// ============================================================================
#include "xAODTrigL1Calo/CMXRoI.h"
#include "xAODTrigL1Calo/CMXRoIContainer.h"
#include "xAODTrigL1Calo/CMXRoIAuxContainer.h"
// ============================================================================
// Local
// ============================================================================
#include "CmxRoIByteStreamAuxCnv.h"
#include "../JepRoiByteStreamV2Tool.h"
#include "../ToString.h"
// ============================================================================

namespace LVL1BS {
CmxRoIByteStreamAuxCnv::CmxRoIByteStreamAuxCnv(ISvcLocator* svcloc) :
  Converter(storageType(), classID(), svcloc),
  AthMessaging(svcloc != 0 ? msgSvc() : 0, "CmxRoIByteStreamAuxCnv"),
  m_name("CmxRoIByteStreamAuxCnv"),
  m_readTool("LVL1BS::JepRoiByteStreamV2Tool/JepRoiByteStreamV2Tool")
{
}

const CLID& CmxRoIByteStreamAuxCnv::classID() {
  return ClassID_traits<xAOD::CMXRoIAuxContainer>::ID();
}

long CmxRoIByteStreamAuxCnv::storageType()
{
  return ByteStreamAddress::storageType();
}

//  Init method gets all necessary services etc.

StatusCode CmxRoIByteStreamAuxCnv::initialize() {
  ATH_MSG_DEBUG(
    "Initializing " << m_name << " - package version " << PACKAGE_VERSION);

  CHECK(Converter::initialize());
  CHECK(m_readTool.retrieve());

  return StatusCode::SUCCESS;
}

// createObj should create the RDO from bytestream.
StatusCode CmxRoIByteStreamAuxCnv::createObj(IOpaqueAddress* pAddr,
    DataObject*& pObj) {
  ATH_MSG_DEBUG("createObj() called");
  // -------------------------------------------------------------------------
  ByteStreamAddress *pBS_Addr = dynamic_cast<ByteStreamAddress *>(pAddr);
  CHECK(pBS_Addr != nullptr);
  // -------------------------------------------------------------------------
  const std::string nm = *(pBS_Addr->par());
  ATH_MSG_DEBUG("Creating Objects " << nm);

  auto aux = new xAOD::CMXRoIAuxContainer;
  xAOD::CMXRoIContainer container;
  container.setStore(aux);
  // -------------------------------------------------------------------------
  LVL1::CMXRoI source;
  StatusCode sc = m_readTool->convert(nm, &source);
  if (sc.isFailure()) {
    ATH_MSG_ERROR("Failed to create objects");
    delete aux;
    return sc;
  }

  xAOD::CMXRoI* item = new xAOD::CMXRoI();
  container.push_back(item);

  item->initialize(source.ex(), source.ey(), source.et(),
    source.exError(), source.eyError(), source.etError(),
    //source.sumEtHits(), source.missingEtHits(),
    // amazurov: values for EtHits and missingEtHits are swaзув in source 
    source.missingEtHits(), source.sumEtHits(),
    source.missingEtSigHits(),
    source.ex(LVL1::CMXRoI::SumType::MASKED),
    source.ey(LVL1::CMXRoI::SumType::MASKED),
    source.et(LVL1::CMXRoI::SumType::MASKED),
    source.exError(LVL1::CMXRoI::SumType::MASKED),
    source.eyError(LVL1::CMXRoI::SumType::MASKED),
    source.etError(LVL1::CMXRoI::SumType::MASKED),
    // source.sumEtHits(LVL1::CMXRoI::SumType::MASKED), source.missingEtHits(LVL1::CMXRoI::SumType::MASKED)
    // amazurov: values for EtHits and missingEtHits are swaзув in source  
    source.missingEtHits(LVL1::CMXRoI::SumType::MASKED), source.sumEtHits(LVL1::CMXRoI::SumType::MASKED)
  );

  // -------------------------------------------------------------------------
  //ATH_MSG_VERBOSE(ToString(container));
  ATH_MSG_DEBUG("Number of readed CMXRoI: " << aux->size());
  // -------------------------------------------------------------------------
  pObj = SG::asStorable(aux);
  return StatusCode::SUCCESS;
}

// createRep should create the bytestream from RDOs.
StatusCode CmxRoIByteStreamAuxCnv::createRep(DataObject* /*pObj*/,
    IOpaqueAddress*& /*pAddr*/) {
  return StatusCode::FAILURE;
}

} // end namespace
