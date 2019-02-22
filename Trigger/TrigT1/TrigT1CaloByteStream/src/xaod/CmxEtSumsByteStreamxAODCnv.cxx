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

#include "xAODTrigL1Calo/CMXEtSums.h"
#include "xAODTrigL1Calo/CMXEtSumsContainer.h"
#include "xAODTrigL1Calo/CMXEtSumsAuxContainer.h"

#include "CmxEtSumsByteStreamxAODCnv.h"

namespace LVL1BS {

CmxEtSumsByteStreamxAODCnv::CmxEtSumsByteStreamxAODCnv(ISvcLocator* svcloc) :
    Converter(storageType(), classID(), svcloc),
    AthMessaging(svcloc != 0 ? msgSvc() : 0, "CmxEtSumsByteStreamxAODCnv"),
    m_name("CmxEtSumsByteStreamxAODCnv")
{

}

// CLID

const CLID& CmxEtSumsByteStreamxAODCnv::classID() {
  return ClassID_traits<xAOD::CMXEtSumsContainer>::ID();
}

long CmxEtSumsByteStreamxAODCnv::storageType()
{
  return ByteStreamAddress::storageType();
}

//  Init method gets all necessary services etc.


StatusCode CmxEtSumsByteStreamxAODCnv::initialize() {
  ATH_MSG_DEBUG(
      "Initializing " << m_name << " - package version " << PACKAGE_VERSION);

  CHECK(Converter::initialize());
  //CHECK(m_readTool.retrieve());
  return StatusCode::SUCCESS;
}

// createObj should create the RDO from bytestream.

StatusCode CmxEtSumsByteStreamxAODCnv::createObj(IOpaqueAddress* pAddr,
    DataObject*& pObj) {
  ATH_MSG_DEBUG("createObj() called");
  // -------------------------------------------------------------------------
  ByteStreamAddress *pBS_Addr = dynamic_cast<ByteStreamAddress *>(pAddr);
  CHECK(pBS_Addr != nullptr);
  // -------------------------------------------------------------------------
  const std::string nm = *(pBS_Addr->par());
  const std::string nmAux = nm + "Aux.";
  ATH_MSG_DEBUG("Creating xAOD::CmxEtSums interface objects '" << nm << "'");

  xAOD::CMXEtSumsContainer* const cmxJetContainer =
      new xAOD::CMXEtSumsContainer;

  // Create link with AUX container
  DataLink<xAOD::CMXEtSumsAuxContainer> link(nmAux);
  ATH_MSG_DEBUG("Creating store with data link to '" << nmAux);

  for(size_t i=0; i < link->size(); ++i){
     cmxJetContainer->push_back(new xAOD::CMXEtSums());
  }
  // ========================================================================== 
  cmxJetContainer->setStore(link);
  pObj = SG::asStorable(cmxJetContainer);
  ATH_MSG_DEBUG("Number of CMXEtSums created: " << cmxJetContainer->size());

  return StatusCode::SUCCESS;
}

// createRep should create the bytestream from RDOs.

StatusCode CmxEtSumsByteStreamxAODCnv::createRep(DataObject* /*pObj*/,
    IOpaqueAddress*& /*pAddr*/) {
  return StatusCode::FAILURE;
}

} // end namespace
