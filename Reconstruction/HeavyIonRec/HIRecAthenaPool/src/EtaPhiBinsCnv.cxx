/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/********************************************************************

NAME:     EtaPhiBinsCnv.h
PACKAGE:  offline/Reconstruction/RecAthenaPool

AUTHORS:  S. Resconi
CREATED:  Jul 2007

PURPOSE:  Implementation file for class EtaPhiBinsCnv
********************************************************************/

// STL includes

// Framework includes
#include "GaudiKernel/MsgStream.h"

// RecTPCnv includes
#include "HIRecTPCnv/EtaPhiBinsCnv_p1.h"

// RecAthenaPool includes
#include "EtaPhiBinsCnv.h"

/////////////////////////////////////////////////////////////////// 
/// Public methods: 
/////////////////////////////////////////////////////////////////// 

/////////////////////////////////////////////////////////////////// 
/// Const methods: 
/////////////////////////////////////////////////////////////////// 

/////////////////////////////////////////////////////////////////// 
// Protected methods: 
/////////////////////////////////////////////////////////////////// 


EtaPhiBins_PERS* 
EtaPhiBinsCnv::createPersistent( EtaPhiBins* transCont ) 
{
  MsgStream msg( msgSvc(), "EtaPhiBinsCnv" );

  EtaPhiBinsCnv_p1 cnv;
  EtaPhiBins_PERS *persObj = cnv.createPersistent( transCont, msg );

  msg << MSG::DEBUG << "::createPersistent [Success]" << endreq;
  return persObj; 
}

EtaPhiBins* EtaPhiBinsCnv::createTransient() 
{
  MsgStream msg( msgSvc(), "EtaPhiBinsCnv" );

  EtaPhiBins *transObj = 0;

  static pool::Guid tr_guid("080267DD-E545-4469-ABED-BC7EB62D4C64");
  //static pool::Guid p1_guid("C0D9AEE8-A2B0-4EED-BD75-53D67DF9736F");
  static pool::Guid p1_guid("C653008A-0C79-4102-A318-F42703FD7EE4");

  if ( compareClassGuid(tr_guid) ) {

    // regular object from before the T/P separation
    return poolReadObject<EtaPhiBins>();

  } else if ( compareClassGuid(p1_guid) ) {

    // using auto_ptr ensures deletion of the persistent object
    std::auto_ptr<EtaPhiBins_p1> persObj( poolReadObject<EtaPhiBins_p1>() );
    EtaPhiBinsCnv_p1 cnv;
    transObj = cnv.createTransient( persObj.get(), msg );
  } else {
    throw std::runtime_error("Unsupported persistent version of EtaPhiBins");
  }

  return transObj;
}
