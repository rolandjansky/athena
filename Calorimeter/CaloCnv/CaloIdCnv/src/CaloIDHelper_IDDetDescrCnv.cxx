/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file CaloIdCnv/src/CaloIDHelper_IDDetDescrCnv.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Aug, 2012
 * @brief Common code for creating calo ID helpers in the detector store.
 */


#include "CaloIdCnv/CaloIDHelper_IDDetDescrCnv.h"
#include "IdDictDetDescr/IdDictManager.h"
#include "DetDescrCnvSvc/DetDescrAddress.h"
#include "StoreGate/StoreGateSvc.h"
#include "AthenaKernel/ClassID_traits.h"
#include "Identifier/IdHelper.h"
#include "AthenaKernel/errorcheck.h"
#include "GaudiKernel/IClassIDSvc.h"
#include "GaudiKernel/MsgStream.h"


namespace CaloIdCnv {


/**
 * @brief Called by the converter infrastructure to create an object.
 * @param pAddr Address of the object to create.
 * @param pObj[out] Set to a reference to the created helper.
 */
StatusCode CaloIDHelper_IDDetDescrCnv::createObj (IOpaqueAddress* pAddr,
                                                  DataObject*& pObj)
{
  // Get the a name of the class we're converting.
  std::string type_name;
  IClassIDSvc* clidsvc = 0;
  CHECK( service ("ClassIDSvc", clidsvc) );
  CHECK( clidsvc->getTypeNameOfID (objType(), type_name) );

  MsgStream log(msgSvc(), "CaloIDHelper_IDDetDescrCnv");
  log << MSG::INFO << "in createObj: creating a " << type_name
      << " helper object in the detector store" << endmsg;

  // Get the SG key.
  DetDescrAddress* ddAddr;
  ddAddr = dynamic_cast<DetDescrAddress*> (pAddr);
  if(!ddAddr) {
    log << MSG::FATAL << "Could not cast to DetDescrAddress." << endmsg;
    return StatusCode::FAILURE;
  }
  std::string helperKey  = *( ddAddr->par() );
  if ("" == helperKey) {
    log << MSG::DEBUG << "No Helper key " << endmsg;
  }
  else {
    log << MSG::DEBUG << "Helper key is " << helperKey << endmsg;
  }
    
  // get DetectorStore service
  StoreGateSvc * detStore = 0;
  CHECK( service("DetectorStore", detStore) );
 
  // Get the dictionary manager from the detector store
  const IdDictManager* idDictMgr = 0;
  CHECK( detStore->retrieve(idDictMgr, "IdDict") );

  // Create the helper.
  IdHelper* idhelper = 0;
  CHECK( createHelper (helperKey, idhelper, pObj) );

  // Initialize the helper.
  idhelper->setMessageSvc (msgSvc());
  if (idDictMgr->initializeHelper(*idhelper)) {
    log << MSG::ERROR << "Unable to initialize " << type_name << endmsg;
    return StatusCode::FAILURE;
  } 
  else {
    log << MSG::DEBUG << " Initialized " << type_name << endmsg;
  }

  return StatusCode::SUCCESS; 
}


/**
 * @brief Return the service type.  Required by the base class.
 */
long int CaloIDHelper_IDDetDescrCnv::repSvcType() const
{
  return storageType();
}


/**
 * @brief Constructor.
 * @param clid The CLID if the class we're constructing.
 * @param svcloc Gaudi service locator.
 */
CaloIDHelper_IDDetDescrCnv::CaloIDHelper_IDDetDescrCnv (const CLID& clid,
                                                        ISvcLocator* svcloc)
  : DetDescrConverter (clid, svcloc)
{
}


} // namespace CaloIdCnv
