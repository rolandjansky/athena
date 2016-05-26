/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/********************************************************************
NAME:     MissingETCnv.h
PACKAGE:  offline/Reconstruction/RecAthenaPool
PURPOSE:  Implementation file for class MissingETCnv
********************************************************************/

// Framework includes
#include "GaudiKernel/MsgStream.h"

// RecTPCnv includes
#include "RecTPCnv/MissingETCnv_p1.h"
#include "RecTPCnv/MissingETCnv_p2.h"
#include "RecTPCnv/MissingETCnv_p3.h"

// RecAthenaPool includes
#include "MissingETCnv.h"

MissingET_PERS* 
MissingETCnv::createPersistent( MissingET* transCont ) 
{
  MsgStream msg( msgSvc(), "MissingETCnv" );

  MissingETCnv_p3 cnv;
  MissingET_PERS *persObj = cnv.createPersistent( transCont, msg );

  msg << MSG::DEBUG << "::createPersistent [Success]" << endreq;
  return persObj; 
}

MissingET* MissingETCnv::createTransient() 
{
  MsgStream msg( msgSvc(), "MissingETCnv" );

  MissingET *transObj = 0;

  static pool::Guid tr_guid("14E6F88E-BA7F-4241-BF3D-99DE583B4709");
  static pool::Guid p1_guid("C0D9AEE8-A2B0-4EED-BD75-53D67DF9736F");
  static pool::Guid p2_guid("2D9729C7-646D-4BF5-B404-380AE2D9D308");
  static pool::Guid p3_guid("AD744C55-6CA6-4D44-8E2F-C47FF3CE980E");

  if ( compareClassGuid(tr_guid) ) { // regular object from before the T/P separation
        return poolReadObject<MissingET>();
  } else if ( compareClassGuid(p1_guid) ) {
	    // using unique_ptr ensures deletion of the persistent object
	    std::unique_ptr<MissingET_p1> persObj( poolReadObject<MissingET_p1>() );
	    MissingETCnv_p1 cnv;
	    transObj = cnv.createTransient( persObj.get(), msg );
  }	else if ( compareClassGuid(p2_guid) ) {
	    std::unique_ptr<MissingET_p2> persObj( poolReadObject<MissingET_p2>() );
	    MissingETCnv_p2 cnv;
	    transObj = cnv.createTransient( persObj.get(), msg );
  }	else if ( compareClassGuid(p3_guid) ) {
	    std::unique_ptr<MissingET_p3> persObj( poolReadObject<MissingET_p3>() );
	    MissingETCnv_p3 cnv;
	    transObj = cnv.createTransient( persObj.get(), msg );		    
  } else {
        throw std::runtime_error("Unsupported persistent version of MissingET");
  }

  return transObj;
}
