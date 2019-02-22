/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <vector>
#include <stdint.h>

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

#include "xAODTrigL1Calo/CMXJetTob.h"
#include "xAODTrigL1Calo/CMXJetTobContainer.h"
#include "xAODTrigL1Calo/CMXJetTobAuxContainer.h"

#include "CmxJetTobByteStreamxAODCnv.h"

namespace LVL1BS {

CmxJetTobByteStreamxAODCnv::CmxJetTobByteStreamxAODCnv(ISvcLocator* svcloc) :
    Converter(storageType(), classID(), svcloc),
    AthMessaging(svcloc != 0 ? msgSvc() : 0, "CmxJetTobByteStreamxAODCnv"),
    m_name("CmxJetTobByteStreamxAODCnv")
{

}

// CLID

const CLID& CmxJetTobByteStreamxAODCnv::classID() {
  return ClassID_traits<xAOD::CMXJetTobContainer>::ID();
}

long CmxJetTobByteStreamxAODCnv::storageType()
{
  return ByteStreamAddress::storageType();
}

//  Init method gets all necessary services etc.

#ifndef PACKAGE_VERSION
#define PACKAGE_VERSION "unknown"
#endif

StatusCode CmxJetTobByteStreamxAODCnv::initialize() {
  ATH_MSG_DEBUG(
      "Initializing " << m_name << " - package version " << PACKAGE_VERSION);

  CHECK(Converter::initialize());
  //CHECK(m_readTool.retrieve());
  return StatusCode::SUCCESS;
}

// createObj should create the RDO from bytestream.

StatusCode CmxJetTobByteStreamxAODCnv::createObj(IOpaqueAddress* pAddr,
    DataObject*& pObj) {
  ATH_MSG_DEBUG("createObj() called");
  // -------------------------------------------------------------------------
  ByteStreamAddress *pBS_Addr = dynamic_cast<ByteStreamAddress *>(pAddr);
  CHECK(pBS_Addr != nullptr);
  // -------------------------------------------------------------------------
  const std::string nm = *(pBS_Addr->par());
  const std::string nmAux = nm + "Aux.";
  ATH_MSG_DEBUG("Creating xAOD::CmxJetTob interface objects '" << nm << "'");

  xAOD::CMXJetTobContainer* const cmxJetContainer =
      new xAOD::CMXJetTobContainer;

  // Create link with AUX container
  DataLink<xAOD::CMXJetTobAuxContainer> link(nmAux);
  ATH_MSG_DEBUG("Creating store with data link to '" << nmAux);

  for(size_t i=0; i < link->size(); ++i){
     cmxJetContainer->push_back(new xAOD::CMXJetTob());
  }
  // ========================================================================== 
  cmxJetContainer->setStore(link);
  pObj = SG::asStorable(cmxJetContainer);
  ATH_MSG_DEBUG("Number of CmxJetTob created: " << cmxJetContainer->size());

  return StatusCode::SUCCESS;
}

// createRep should create the bytestream from RDOs.

StatusCode CmxJetTobByteStreamxAODCnv::createRep(DataObject* /*pObj*/,
    IOpaqueAddress*& /*pAddr*/) {
  return StatusCode::FAILURE;
}

} // end namespace
