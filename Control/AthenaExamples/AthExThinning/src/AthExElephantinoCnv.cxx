///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

// ElephantinoCnv.cxx 
// Implementation file for class ElephantinoCnv
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 

// STL includes

// Framework includes
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IToolSvc.h"
#include "GaudiKernel/IAlgTool.h"
#include "StoreGate/StoreGateSvc.h"

// AthExThinning includes
#include "AthExElephantinoCnv_p1.h"
#include "AthExElephantinoCnv.h"

/////////////////////////////////////////////////////////////////// 
/// Public methods: 
/////////////////////////////////////////////////////////////////// 

/////////////////////////////////////////////////////////////////// 
/// Const methods: 
/////////////////////////////////////////////////////////////////// 

/////////////////////////////////////////////////////////////////// 
// Protected methods: 
/////////////////////////////////////////////////////////////////// 

AthExElephantinoCnv::AthExElephantinoCnv(ISvcLocator* svcLocator) :
  T_AthenaPoolCustomCnv<AthExElephantino, 
                        AthExElephantino_PERS>(svcLocator)
{
  if ( 0 == svcLocator ) {
    throw std::runtime_error("NULL pointer to ISvcLocator !!");
  }

}


AthExElephantino_PERS* 
AthExElephantinoCnv::createPersistent(AthExElephantino* transCont)
{
  MsgStream msg( msgSvc(), "AthExElephantinoCnv" );

  AthExElephantinoCnv_p1 cnv;
  AthExElephantino_PERS *persObj = cnv.createPersistent(transCont, msg);

  msg << MSG::DEBUG << "::createPersistent [Success]" << endmsg;
  return persObj; 
}

AthExElephantino* AthExElephantinoCnv::createTransient() 
{
   MsgStream msg( msgSvc(), "AthExElephantinoConverter" );

   AthExElephantino *transObj = 0;

   static const pool::Guid p1_guid("F90BE215-64AD-45E3-B5E4-6AD7DE35BDF9");

   if ( compareClassGuid(p1_guid) ) {

     // using unique_ptr ensures deletion of the persistent object
     std::unique_ptr<AthExElephantino_p1> persObj( poolReadObject<AthExElephantino_p1>() );
     AthExElephantinoCnv_p1 cnv;
     transObj = cnv.createTransient( persObj.get(), msg );
     
   } else {
     throw std::runtime_error("Unsupported persistent version of Elephantino");
   }

   return transObj;
}
