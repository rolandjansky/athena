/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/** @file CondAttrListVecCnv.cxx
 *  @brief This file contains the implementation for the CondAttrListVecCnv class.
 *  @author Richard Hawkings <richard.hawkings@cern.ch>
 **/

#include "CondAttrListVecCnv.h"

#include "GaudiKernel/ClassID.h"
#include "GaudiKernel/DataObject.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/StatusCode.h"

// AthenaAttributeList and address
#include "AthenaPoolUtilities/CondAttrListVec.h"
#include "AthenaPoolUtilities/CondAttrListVecAddress.h"

#include "StoreGate/StoreGateSvc.h"

//--------------------------------------------------------------------

long int   
CondAttrListVecCnv::repSvcType() const {
  return(storageType());
}

//__________________________________________________________________________
CondAttrListVecCnv::~CondAttrListVecCnv() {
}
//__________________________________________________________________________
StatusCode CondAttrListVecCnv::initialize() {
   MsgStream log(msgSvc(), "CondAttrListVecCnv");

   log << MSG::DEBUG << "initialize() in CondAttrListVecCnv" << endmsg;
   StatusCode status = Converter::initialize();
   if (!status.isSuccess()) {
      log << MSG::ERROR << "Can not initialize Converter base class." << endmsg;
      return(status);
   }
   return(status);
}
//__________________________________________________________________________
StatusCode CondAttrListVecCnv::createObj(IOpaqueAddress* pAddr, DataObject*& pObj) {

    // This is the "read" method for an AttributeList. Here we only
    // extract an AthenaAttributeList pointer from an
    // AthenaAttrListAddress and return it

    MsgStream log(msgSvc(), "CondAttrListVecCnv");
    log << MSG::DEBUG << "Creating CondAttrListVecAddress Object" << endmsg;

    CondAttrListVecAddress* addr = dynamic_cast<CondAttrListVecAddress*>(pAddr);

    if (addr) {
	// Successful cast
	pObj = SG::asStorable(addr->attrListVec());
	if (!pObj) {
	    log << MSG::ERROR << "CondAttrListVec ptr is 0 from CondAttrListVecAddress " << endmsg;
	    return(StatusCode::FAILURE);       
	}
    } else {
	log << MSG::ERROR << "Could not extract ptr for CondAttrListVecAddress " << endmsg;
	return(StatusCode::FAILURE);       
    }

    return(StatusCode::SUCCESS);
}
//__________________________________________________________________________
StatusCode CondAttrListVecCnv::createRep(DataObject* pObj, IOpaqueAddress*& pAddr) {

    // This is the "write" method for an AttributeList. Here we only
    // create an AthenaAttrListAddress and fill it with a pointer to
    // the AthenaAttributeList

    MsgStream log(msgSvc(), "CondAttrListVecCnv");
    log << MSG::DEBUG << "Copy CondAttrListVec ptr into CondAttrListVecAddress" << endmsg;

    CondAttrListVec* list = 0; //dynamic_cast<AthenaAttributeList*>(pObj);
    SG::fromStorable(pObj, list);
    CondAttrListVecAddress* addr = new CondAttrListVecAddress(POOL_StorageType,
		    classID(),
		    "POOLContainer_CondAttrListVec][CLID=x");
    addr->setAttrListVec(list);
    pAddr = addr;
    log << MSG::DEBUG << "Created CondAttrListVec with list ptr: " << list << endmsg;
    return(StatusCode::SUCCESS);
}
//__________________________________________________________________________
long CondAttrListVecCnv::storageType() {
   return(POOL_StorageType);
}
//__________________________________________________________________________
const CLID& CondAttrListVecCnv::classID() {
   return(ClassID_traits<CondAttrListVec>::ID());
}
//__________________________________________________________________________
CondAttrListVecCnv::CondAttrListVecCnv(ISvcLocator* svcloc) :
	Converter(POOL_StorageType, classID(), svcloc) {
}
//__________________________________________________________________________
