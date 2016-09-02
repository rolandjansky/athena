/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/** @file CondAttrListCollCnv.cxx
 *  @brief This file contains the implementation for the CondAttrListCollCnv class.
 *  @author RD Schaffer <R.D.Schaffer@cern.ch>
 *  $Id: CondAttrListCollCnv.cxx,v 1.4 2008-12-12 05:47:06 ssnyder Exp $
 **/

#include "CondAttrListCollCnv.h"

#include "GaudiKernel/ClassID.h"
#include "GaudiKernel/DataObject.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/StatusCode.h"

// CondAttrListCollection and address
#include "AthenaPoolUtilities/CondAttrListCollection.h"
#include "AthenaPoolUtilities/CondAttrListCollAddress.h"

#include "StoreGate/StoreGateSvc.h"

//--------------------------------------------------------------------

long int   
CondAttrListCollCnv::repSvcType() const {
  return(storageType());
}

//__________________________________________________________________________
CondAttrListCollCnv::~CondAttrListCollCnv() {
}
//__________________________________________________________________________
StatusCode CondAttrListCollCnv::initialize() {
   MsgStream log(msgSvc(), "CondAttrListCollCnv");

   log << MSG::DEBUG << "initialize() in CondAttrListCollCnv" << endmsg;
   StatusCode status = Converter::initialize();
   if (!status.isSuccess()) {
      log << MSG::ERROR << "Can not initialize Converter base class." << endmsg;
      return(status);
   }
   return(status);
}
//__________________________________________________________________________
StatusCode CondAttrListCollCnv::createObj(IOpaqueAddress* pAddr, DataObject*& pObj) {

    // This is the "read" method for an CondAttrListCollection. Here
    // we only extract an CondAttrListCollection pointer from an
    // CondAttrListCollAddress and return it

    MsgStream log(msgSvc(), "CondAttrListCollCnv");
    log << MSG::DEBUG << "Creating CondAttrListCollAddress Object" << endmsg;

    CondAttrListCollAddress* addr = dynamic_cast<CondAttrListCollAddress*>(pAddr);

    if (addr) {
	// Successful cast
	pObj = SG::asStorable(addr->attrListColl());
	if (!pObj) {
	    log << MSG::ERROR << "CondAttrListCollection ptr is 0 from CondAttrListCollAddress " << endmsg;
	    return(StatusCode::FAILURE);       
	}
    }
    else {
	log << MSG::ERROR << "Could not extract ptr for CondAttrListCollAddress " << endmsg;
	return(StatusCode::FAILURE);       
    }

    return(StatusCode::SUCCESS);
}
//__________________________________________________________________________
StatusCode CondAttrListCollCnv::createRep(DataObject* pObj, IOpaqueAddress*& pAddr) {

    // This is the "write" method for an CondAttrListCollection. Here
    // we only create an CondAttrListCollAddress and fill it with a
    // pointer to the CondAttrListCollection

    MsgStream log(msgSvc(), "CondAttrListCollCnv");
    log << MSG::DEBUG << "Copy CondAttrListCollection ptr into CondAttrListCollAddress" << endmsg;

    CondAttrListCollection* list = 0; //dynamic_cast<CondAttrListCollection*>(pObj);
    SG::fromStorable(pObj, list);
    CondAttrListCollAddress* addr = new CondAttrListCollAddress(POOL_StorageType,
		    classID(),
		    "POOLContainer_CondAttrListCollection][CLID=x");
    addr->setAttrListColl(list);
    pAddr = addr;

    log << MSG::DEBUG << "Created CondAttrListCollAddress with list ptr: " << list << endmsg;
    return(StatusCode::SUCCESS);
}
//__________________________________________________________________________
long CondAttrListCollCnv::storageType() {
   return(POOL_StorageType);
}
//__________________________________________________________________________
const CLID& CondAttrListCollCnv::classID() {
   return(ClassID_traits<CondAttrListCollection>::ID());
}
//__________________________________________________________________________
CondAttrListCollCnv::CondAttrListCollCnv(ISvcLocator* svcloc) :
	Converter(POOL_StorageType, classID(), svcloc) {
}
//__________________________________________________________________________
