/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/********************************************************************
NAME:     MissingEtTruthCnv.h
PACKAGE:  offline/Reconstruction/RecAthenaPool
PURPOSE:  Implementation file for class MissingETCnv
********************************************************************/

// Framework includes
#include "GaudiKernel/MsgStream.h"

// RecTPCnv includes
#include "RecTPCnv/MissingEtTruthCnv_p1.h"
#include "RecTPCnv/MissingEtTruthCnv_p2.h"
#include "RecTPCnv/MissingEtTruthCnv_p3.h"

// RecAthenaPool includes
#include "MissingEtTruthCnv.h"


MissingEtTruth_PERS* 
MissingEtTruthCnv::createPersistent( MissingEtTruth* transCont ) 
{
  MsgStream msg( msgSvc(), "MissingEtTruthCnv" );

  MissingEtTruthCnv_p3 cnv;
  MissingEtTruth_PERS *persObj = cnv.createPersistent( transCont, msg );

  msg << MSG::DEBUG << "::createPersistent [Success]" << endreq;
  return persObj; 
}

MissingEtTruth* MissingEtTruthCnv::createTransient() 
{
  MsgStream msg( msgSvc(), "MissingEtTruthCnv" );

  MissingEtTruth *transObj = 0;

  static pool::Guid tr_guid("2147ED1C-7337-4717-900B-1B52BD8DC316");
  static pool::Guid p1_guid("E7D6D6B8-08A8-409E-9477-2A61467738B2");
  static pool::Guid p2_guid("291A1BDB-A25E-412D-8E49-EF0E1EA1A835");
  static pool::Guid p3_guid("3FA8CC45-8798-4C24-9D0E-22E5F11AD85F");

  if (  compareClassGuid(p3_guid)) {
	
    // using unique_ptr ensures deletion of the persistent object
    std::unique_ptr<MissingEtTruth_p3> persObj( poolReadObject<MissingEtTruth_p3>() );
    MissingEtTruthCnv_p3 cnv;
    transObj = cnv.createTransient( persObj.get(), msg );
	
  } else if (  compareClassGuid(p2_guid)) {
	
    // using unique_ptr ensures deletion of the persistent object
    std::unique_ptr<MissingEtTruth_p2> persObj( poolReadObject<MissingEtTruth_p2>() );
    MissingEtTruthCnv_p2 cnv;
    transObj = cnv.createTransient( persObj.get(), msg );
	
  } else if (  compareClassGuid(p1_guid)) {
	
    // using unique_ptr ensures deletion of the persistent object
    std::unique_ptr<MissingEtTruth_p1> persObj( poolReadObject<MissingEtTruth_p1>() );
    MissingEtTruthCnv_p1 cnv;
    transObj = cnv.createTransient( persObj.get(), msg );
	
  } else if ( compareClassGuid(tr_guid) ) { // regular object from before the T/P separation
    return poolReadObject<MissingEtTruth>();
  } else {
    throw std::runtime_error("Unsupported persistent version of MissingEtTruth");
  }

  return transObj;
}
