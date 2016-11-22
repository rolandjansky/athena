/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/** @file AthenaAttributeListCnv.cxx
 *  @brief This file contains the implementation for the AthenaAttributeListCnv class.
 *  @author RD Schaffer <R.D.Schaffer@cern.ch>
 *  $Id: AthenaAttributeListCnv.cxx,v 1.4 2008-12-12 05:47:05 ssnyder Exp $
 **/

#include "AthenaAttributeListCnv.h"

#include "GaudiKernel/ClassID.h"
#include "GaudiKernel/DataObject.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/StatusCode.h"

// AthenaAttributeList and address
#include "AthenaPoolUtilities/AthenaAttributeList.h"
#include "AthenaPoolUtilities/AthenaAttrListAddress.h"

#include "StoreGate/StoreGateSvc.h"

//--------------------------------------------------------------------

long int   
AthenaAttributeListCnv::repSvcType() const {
  return(storageType());
}

//__________________________________________________________________________
AthenaAttributeListCnv::~AthenaAttributeListCnv() {
}
//__________________________________________________________________________
StatusCode AthenaAttributeListCnv::initialize() {
   MsgStream log(msgSvc(), "AthenaAttributeListCnv");

   log << MSG::DEBUG << "initialize() in AthenaAttributeListCnv" << endmsg;
   StatusCode status = Converter::initialize();
   if (!status.isSuccess()) {
      log << MSG::ERROR << "Can not initialize Converter base class." << endmsg;
      return(status);
   }
   return(status);
}
//__________________________________________________________________________
StatusCode AthenaAttributeListCnv::createObj(IOpaqueAddress* pAddr, DataObject*& pObj) {

    // This is the "read" method for an AttributeList. Here we only
    // extract an AthenaAttributeList pointer from an
    // AthenaAttrListAddress and return it

    MsgStream log(msgSvc(), "AthenaAttributeListCnv");
    log << MSG::DEBUG << "Creating AthenaAttrListAddress Object" << endmsg;

    AthenaAttrListAddress* addr = dynamic_cast<AthenaAttrListAddress*>(pAddr);

    if (addr) {
	// Successful cast
	pObj = SG::asStorable(addr->attrList());
	if (!pObj) {
	    log << MSG::ERROR << "AttributeList ptr is 0 from AthenaAttrListAddress " << endmsg;
	    return(StatusCode::FAILURE);       
	}
    } else {
	log << MSG::ERROR << "Could not extract ptr for AthenaAttrListAddress " << endmsg;
	return(StatusCode::FAILURE);       
    }

    return(StatusCode::SUCCESS);
}
//__________________________________________________________________________
StatusCode AthenaAttributeListCnv::createRep(DataObject* pObj, IOpaqueAddress*& pAddr) {

    // This is the "write" method for an AttributeList. Here we only
    // create an AthenaAttrListAddress and fill it with a pointer to
    // the AthenaAttributeList

    MsgStream log(msgSvc(), "AthenaAttributeListCnv");
    log << MSG::DEBUG << "Copy AthenaAttributeList ptr into AthenaAttrListAddress" << endmsg;

    AthenaAttributeList* list = 0; //dynamic_cast<AthenaAttributeList*>(pObj);
    SG::fromStorable(pObj, list);
    AthenaAttrListAddress* addr = new AthenaAttrListAddress(POOL_StorageType,
		    classID(),
		    "POOLContainer_AthenaAttributeList][CLID=x");
    addr->setAttrList(list);
    pAddr = addr;

    log << MSG::DEBUG << "Created AthenaAttrListAddress with list ptr: " << list << endmsg;
    return(StatusCode::SUCCESS);
}
//__________________________________________________________________________
long AthenaAttributeListCnv::storageType() {
   return(POOL_StorageType);
}
//__________________________________________________________________________
const CLID& AthenaAttributeListCnv::classID() {
   return(ClassID_traits<AthenaAttributeList>::ID());
}
//__________________________________________________________________________
AthenaAttributeListCnv::AthenaAttributeListCnv(ISvcLocator* svcloc) :
	Converter(POOL_StorageType, classID(), svcloc) {
}
//__________________________________________________________________________
