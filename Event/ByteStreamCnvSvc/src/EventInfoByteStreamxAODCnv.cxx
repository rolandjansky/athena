/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "EventInfoByteStreamxAODCnv.h"
#include "ByteStreamCnvSvcBase/ByteStreamAddress.h"

#include "AthenaKernel/errorcheck.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/DataObject.h"
#include "GaudiKernel/IRegistry.h"

#include "xAODEventInfo/EventInfo.h"
#include "xAODEventInfo/EventAuxInfo.h"

#include "StoreGate/StoreGateSvc.h"

EventInfoByteStreamxAODCnv::EventInfoByteStreamxAODCnv(ISvcLocator* svcloc)
  : Converter(storageType(), classID(), svcloc)
  , AthMessaging(svcloc != nullptr ? msgSvc() : nullptr, "EventInfoByteStreamxAODCnv")
{
}

const CLID& EventInfoByteStreamxAODCnv::classID() 
{
  return ClassID_traits<xAOD::EventInfo>::ID();
}

long EventInfoByteStreamxAODCnv::storageType() 
{
  return ByteStreamAddress::storageType();
}

StatusCode EventInfoByteStreamxAODCnv::initialize() 
{
  ATH_MSG_DEBUG("Initialize");
  CHECK(Converter::initialize());
  return StatusCode::SUCCESS;
}

StatusCode EventInfoByteStreamxAODCnv::finalize() 
{
  ATH_MSG_DEBUG("Finalize");
  
  StatusCode sc = Converter::finalize();
  if (sc.isFailure()) {
    ATH_MSG_WARNING("Converter::finalize() failed");
  }
  return sc;
}

StatusCode EventInfoByteStreamxAODCnv::createObj(IOpaqueAddress* pAddr, DataObject*& pObj) 
{
  ByteStreamAddress *pRE_Addr{nullptr};
  pRE_Addr = dynamic_cast<ByteStreamAddress*>(pAddr);
  if (!pRE_Addr) {
    ATH_MSG_ERROR("Cannot cast to ByteStreamAddress ");
    return StatusCode::FAILURE;
  }
               
  ATH_MSG_DEBUG("Creating Objects");

  const std::string nm = *(pRE_Addr->par());
  const std::string nmAux = nm + "Aux.";

  xAOD::EventInfo* pEvtInfo = new xAOD::EventInfo();
  DataLink<xAOD::EventAuxInfo> link(nmAux);
  pEvtInfo->setStore(link);
  pObj = StoreGateSvc::asStorable(pEvtInfo);

  ATH_MSG_DEBUG(" New EventInfo made, run/event= " << pEvtInfo->runNumber() << " " << pEvtInfo->eventNumber());

  return StatusCode::SUCCESS;
}

StatusCode EventInfoByteStreamxAODCnv::createRep(DataObject* /*pObj*/, IOpaqueAddress*& /*pAddr*/) 
{
  ATH_MSG_DEBUG("Nothing to be done for xAOD::EventInfo createReps");
  return StatusCode::SUCCESS;
}
