///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

// FatObjectCnv.cxx 
// Implementation file for class FatObjectCnv
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 

// STL includes

// Framework includes
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IToolSvc.h"
#include "GaudiKernel/IAlgTool.h"
#include "StoreGate/StoreGateSvc.h"

// AthExThinning includes
#include "AthExFatObjectCnv_p1.h"
#include "AthExFatObjectCnv.h"

/////////////////////////////////////////////////////////////////// 
/// Public methods: 
/////////////////////////////////////////////////////////////////// 

/////////////////////////////////////////////////////////////////// 
/// Const methods: 
/////////////////////////////////////////////////////////////////// 

/////////////////////////////////////////////////////////////////// 
// Protected methods: 
/////////////////////////////////////////////////////////////////// 

AthExFatObjectCnv::AthExFatObjectCnv(ISvcLocator* svcLocator) :
  T_AthenaPoolCustomCnv<AthExFatObject, 
                        AthExFatObject_PERS>(svcLocator)
{
  if ( 0 == svcLocator ) {
    throw std::runtime_error("NULL pointer to ISvcLocator !!");
  }

}


AthExFatObject_PERS* 
AthExFatObjectCnv::createPersistent(AthExFatObject* transCont)
{
  MsgStream msg( msgSvc(), "AthExFatObjectCnv" );

  AthExFatObjectCnv_p1 cnv;
  AthExFatObject_PERS *persObj = cnv.createPersistent(transCont, msg);

  msg << MSG::DEBUG << "::createPersistent [Success]" << endmsg;
  return persObj; 
}

AthExFatObject* AthExFatObjectCnv::createTransient() 
{
   MsgStream msg( msgSvc(), "AthExFatObjectConverter" );

   AthExFatObject *transObj = 0;

   static const pool::Guid p1_guid("C757B4CB-7C1F-4E79-895E-C77D76FB960B");

   if ( compareClassGuid(p1_guid) ) {

     // using unique_ptr ensures deletion of the persistent object
     std::unique_ptr<AthExFatObject_p1> persObj( poolReadObject<AthExFatObject_p1>() );
     AthExFatObjectCnv_p1 cnv;
     transObj = cnv.createTransient( persObj.get(), msg );
     
   } else {
     throw std::runtime_error("Unsupported persistent version of FatObject");
   }

   return transObj;
}
