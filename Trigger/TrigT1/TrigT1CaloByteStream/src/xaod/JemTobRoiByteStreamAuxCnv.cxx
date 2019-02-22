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
#include "TrigT1CaloEvent/JEMTobRoI.h"
// ============================================================================
// xAOD
// ============================================================================
#include "xAODTrigL1Calo/JEMTobRoI.h"
#include "xAODTrigL1Calo/JEMTobRoIContainer.h"
#include "xAODTrigL1Calo/JEMTobRoIAuxContainer.h"
// ============================================================================
// Local
// ============================================================================
#include "JemTobRoiByteStreamAuxCnv.h"
#include "../JepRoiByteStreamV2Tool.h"
#include "../ToString.h"
// ============================================================================

namespace LVL1BS {
JemTobRoiByteStreamAuxCnv::JemTobRoiByteStreamAuxCnv(ISvcLocator* svcloc) :
  Converter(storageType(), classID(), svcloc),
  AthMessaging(svcloc != 0 ? msgSvc() : 0, "JemTobRoiByteStreamAuxCnv"),
  m_name("JemTobRoiByteStreamAuxCnv"),
  m_readTool("LVL1BS::JepRoiByteStreamV2Tool/JepRoiByteStreamV2Tool")
{
}

const CLID& JemTobRoiByteStreamAuxCnv::classID() {
  return ClassID_traits<xAOD::JEMTobRoIAuxContainer>::ID();
}

long JemTobRoiByteStreamAuxCnv::storageType()
{
  return ByteStreamAddress::storageType();
}

//  Init method gets all necessary services etc.

StatusCode JemTobRoiByteStreamAuxCnv::initialize() {
  ATH_MSG_DEBUG(
    "Initializing " << m_name << " - package version " << PACKAGE_VERSION);

  CHECK(Converter::initialize());
  CHECK(m_readTool.retrieve());

  return StatusCode::SUCCESS;
}

// createObj should create the RDO from bytestream.
StatusCode JemTobRoiByteStreamAuxCnv::createObj(IOpaqueAddress* pAddr,
    DataObject*& pObj) {
  ATH_MSG_DEBUG("createObj() called");
  // -------------------------------------------------------------------------
  ByteStreamAddress *pBS_Addr = dynamic_cast<ByteStreamAddress *>(pAddr);
  CHECK(pBS_Addr != nullptr);
  // -------------------------------------------------------------------------
  const std::string nm = *(pBS_Addr->par());
  ATH_MSG_DEBUG("Creating Objects " << nm);

  auto aux = new xAOD::JEMTobRoIAuxContainer;
  xAOD::JEMTobRoIContainer container;
  container.setStore(aux);
  // -------------------------------------------------------------------------
  DataVector<LVL1::JEMTobRoI> vec;
  StatusCode sc = m_readTool->convert(nm, &vec);
  if (sc.isFailure()) {
    ATH_MSG_ERROR("Failed to create objects");
    delete aux;
    return sc;
  }

  for (auto source : vec) {
    xAOD::JEMTobRoI* item = new xAOD::JEMTobRoI();
    container.push_back(item);
    item->setRoiWord(source->roiWord());
  }

  // -------------------------------------------------------------------------
  //ATH_MSG_VERBOSE(ToString(container));
  ATH_MSG_DEBUG("Number of readed JemTobRoi: " << aux->size());
  // -------------------------------------------------------------------------
  pObj = SG::asStorable(aux);
  return StatusCode::SUCCESS;
}

// createRep should create the bytestream from RDOs.
StatusCode JemTobRoiByteStreamAuxCnv::createRep(DataObject* /*pObj*/,
    IOpaqueAddress*& /*pAddr*/) {
  return StatusCode::FAILURE;
}

} // end namespace
