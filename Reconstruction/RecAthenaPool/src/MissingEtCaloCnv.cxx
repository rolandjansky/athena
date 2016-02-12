/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/********************************************************************
NAME:     MissingEtCaloCnv.h
PURPOSE:  Implementation file for class MissingEtCaloCnv
********************************************************************/

// Framework includes
#include "GaudiKernel/MsgStream.h"

// RecTPCnv includes
#include "RecTPCnv/MissingEtCaloCnv_p1.h"
#include "RecTPCnv/MissingEtCaloCnv_p2.h"
#include "RecTPCnv/MissingEtCaloCnv_p3.h"

// RecAthenaPool includes
#include "MissingEtCaloCnv.h"

MissingEtCalo_PERS* 
MissingEtCaloCnv::createPersistent( MissingEtCalo* transCont ) 
{
  MsgStream msg( msgSvc(), "MissingEtCaloCnv" );

  MissingEtCaloCnv_p3 cnv;
  MissingEtCalo_PERS *persObj = cnv.createPersistent( transCont, msg );

  msg << MSG::DEBUG << "::createPersistent [Success]" << endreq;
  return persObj; 
}

MissingEtCalo* MissingEtCaloCnv::createTransient() 
{
  MsgStream msg( msgSvc(), "MissingEtCaloCnv" );

  MissingEtCalo *transObj = 0;

  static pool::Guid tr_guid("BD14D6E7-C771-4079-BF31-F43FE1068C95");
  static pool::Guid p1_guid("877E907D-D7E1-49DE-B01F-9CDA344E6004");
  static pool::Guid p2_guid("F16ECA98-8809-436C-A00A-06DBBF9C3087");
  static pool::Guid p3_guid("0B9C9E87-92C8-49D3-8DF1-45ACDDD3336F");

  if ( compareClassGuid(tr_guid) ) { // regular object from before the T/P separation
    return poolReadObject<MissingEtCalo>();
  }else if ( compareClassGuid(p1_guid) ) {
    // using unique_ptr ensures deletion of the persistent object
    std::unique_ptr<MissingEtCalo_p1> persObj( poolReadObject<MissingEtCalo_p1>() );
    MissingEtCaloCnv_p1 cnv;
    transObj = cnv.createTransient( persObj.get(), msg );
  }else if ( compareClassGuid(p2_guid) ) {
    std::unique_ptr<MissingEtCalo_p2> persObj( poolReadObject<MissingEtCalo_p2>() );
    MissingEtCaloCnv_p2 cnv;
    transObj = cnv.createTransient( persObj.get(), msg );
  }else if ( compareClassGuid(p3_guid) ) {
    std::unique_ptr<MissingEtCalo_p3> persObj( poolReadObject<MissingEtCalo_p3>() );
    MissingEtCaloCnv_p3 cnv;
    transObj = cnv.createTransient( persObj.get(), msg );    
  } else {
    throw std::runtime_error("Unsupported persistent version of MissingEtCalo");
  }

  return transObj;
}
