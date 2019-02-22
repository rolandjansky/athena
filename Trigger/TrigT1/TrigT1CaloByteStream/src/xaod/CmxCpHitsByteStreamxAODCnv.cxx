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

#include "xAODTrigL1Calo/CMXCPHits.h"
#include "xAODTrigL1Calo/CMXCPHitsContainer.h"
#include "xAODTrigL1Calo/CMXCPHitsAuxContainer.h"

#include "CmxCpHitsByteStreamxAODCnv.h"

namespace LVL1BS {

CmxCpHitsByteStreamxAODCnv::CmxCpHitsByteStreamxAODCnv(ISvcLocator* svcloc) :
    Converter(storageType(), classID(), svcloc),
    AthMessaging(svcloc != 0 ? msgSvc() : 0, "CmxCpHitsByteStreamxAODCnv"),
    m_name("CmxCpHitsByteStreamxAODCnv")
{

}

// CLID

const CLID& CmxCpHitsByteStreamxAODCnv::classID() {
  return ClassID_traits<xAOD::CMXCPHitsContainer>::ID();
}

long CmxCpHitsByteStreamxAODCnv::storageType()
{
  return ByteStreamAddress::storageType();
}

//  Init method gets all necessary services etc.


StatusCode CmxCpHitsByteStreamxAODCnv::initialize() {
  ATH_MSG_DEBUG(
      "Initializing " << m_name << " - package version " << PACKAGE_VERSION);

  CHECK(Converter::initialize());
  return StatusCode::SUCCESS;
}

// createObj should create the RDO from bytestream.

StatusCode CmxCpHitsByteStreamxAODCnv::createObj(IOpaqueAddress* pAddr,
    DataObject*& pObj) {
  ATH_MSG_DEBUG("createObj() called");
  // -------------------------------------------------------------------------
  ByteStreamAddress *pBS_Addr = dynamic_cast<ByteStreamAddress *>(pAddr);
  CHECK(pBS_Addr != nullptr);
  // -------------------------------------------------------------------------
  const std::string nm = *(pBS_Addr->par());
  const std::string nmAux = nm + "Aux.";
  ATH_MSG_DEBUG("Creating xAOD::CMXCPHits interface objects '" << nm << "'");

  xAOD::CMXCPHitsContainer* const cphitsCollection =
      new xAOD::CMXCPHitsContainer;

  // Create link with AUX container
  DataLink<xAOD::CMXCPHitsAuxContainer> link(nmAux);
  ATH_MSG_DEBUG("Creating store with data link to '" << nmAux);

  for(size_t i=0; i < link->size(); ++i){
     cphitsCollection->push_back(new xAOD::CMXCPHits());
  }
  // ========================================================================== 
  cphitsCollection->setStore(link);
  pObj = SG::asStorable(cphitsCollection);
  ATH_MSG_DEBUG("Number of xAOD::CMXCPHits created: " << cphitsCollection->size());

  return StatusCode::SUCCESS;
}

// createRep should create the bytestream from RDOs.

StatusCode CmxCpHitsByteStreamxAODCnv::createRep(DataObject* /*pObj*/,
    IOpaqueAddress*& /*pAddr*/) {
  return StatusCode::FAILURE;
}

} // end namespace
