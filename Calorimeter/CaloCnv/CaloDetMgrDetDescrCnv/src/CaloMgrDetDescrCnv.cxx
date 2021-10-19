/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "CaloMgrDetDescrCnv.h"

#include "CaloDetDescrUtils/CaloDetDescrBuilder.h"

#include "DetDescrCnvSvc/DetDescrConverter.h"
#include "DetDescrCnvSvc/DetDescrAddress.h"
#include "GaudiKernel/MsgStream.h"
#include "StoreGate/StoreGateSvc.h"
#include "AthenaKernel/getMessageSvc.h"
#include "GaudiKernel/ThreadLocalContext.h"
#include "AthenaBaseComps/AthCheckMacros.h"
#include "AthenaKernel/StorableConversions.h"

#include "CaloDetDescr/CaloDetDescrManager.h"

CaloMgrDetDescrCnv::CaloMgrDetDescrCnv(ISvcLocator* svcloc)
  : DetDescrConverter(ClassID_traits<CaloDetDescrManager>::ID(), svcloc)
  , AthMessaging(Athena::getMessageSvc(), "CaloMgrDetDescrCnv")
{
}

long int CaloMgrDetDescrCnv::repSvcType() const
{
  return storageType();
}

StatusCode CaloMgrDetDescrCnv::initialize()
{
  ATH_MSG_DEBUG("in initialize");
  ATH_CHECK(DetDescrConverter::initialize());
  return StatusCode::SUCCESS;
}

StatusCode CaloMgrDetDescrCnv::finalize()
{
  ATH_MSG_DEBUG("in finalize");
  return StatusCode::SUCCESS;
}

StatusCode CaloMgrDetDescrCnv::createObj(IOpaqueAddress* pAddr, DataObject*& pObj)
{
  // Ensure that this object is created inside the event loop
  const EventContext& ctx = Gaudi::Hive::currentContext();
  if (!ctx.valid()) {
    ATH_MSG_WARNING("Attempting to create a Calo Detector Manager object outside of the event loop. Geometry may not be aligned.");
  }
  ATH_MSG_INFO("in createObj: creating a Calo Detector Manager object in the detector store");

  DetDescrAddress* ddAddr = dynamic_cast<DetDescrAddress*> (pAddr);
  if(!ddAddr) {
    ATH_MSG_FATAL("Could not cast to DetDescrAddress.");
    return StatusCode::FAILURE;
  }

  std::string mgrKey  = *(ddAddr->par());
  if(mgrKey.empty()) {
    ATH_MSG_DEBUG("No Manager key ");
  }
  else {
    ATH_MSG_DEBUG("Manager key is " << mgrKey);
  }

  // --- --- Create CaloDetDescrManager object --- ---
  std::unique_ptr<CaloDetDescrManager> caloMgr = buildCaloDetDescr(serviceLocator()
								   , Athena::getMessageSvc()
								   , nullptr
								   , nullptr);
  pObj = SG::asStorable<CaloDetDescrManager>(caloMgr.release());

  return StatusCode::SUCCESS;
}

//--------------------------------------------------------------------

long CaloMgrDetDescrCnv::storageType()
{
  return DetDescr_StorageType;
}

//--------------------------------------------------------------------
const CLID& CaloMgrDetDescrCnv::classID() 
{
  return ClassID_traits<CaloDetDescrManager>::ID();
}
