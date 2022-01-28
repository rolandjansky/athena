///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

// DecayCnv.cxx 
// Implementation file for class DecayCnv
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 

// STL includes

// Framework includes
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IToolSvc.h"
#include "GaudiKernel/IAlgTool.h"
#include "StoreGate/StoreGateSvc.h"

// AthExThinning includes
#include "AthExDecayCnv_p1.h"
#include "AthExDecayCnv.h"

/////////////////////////////////////////////////////////////////// 
/// Public methods: 
/////////////////////////////////////////////////////////////////// 

/////////////////////////////////////////////////////////////////// 
/// Const methods: 
/////////////////////////////////////////////////////////////////// 

/////////////////////////////////////////////////////////////////// 
// Protected methods: 
/////////////////////////////////////////////////////////////////// 

AthExDecayCnv::AthExDecayCnv(ISvcLocator* svcLocator) :
  T_AthenaPoolCustomCnv<AthExDecay, 
                        AthExDecay_PERS>(svcLocator)
{
  if ( 0 == svcLocator ) {
    throw std::runtime_error("NULL pointer to ISvcLocator !!");
  }

}


AthExDecay_PERS* 
AthExDecayCnv::createPersistent(AthExDecay* transCont)
{
  MsgStream msg( msgSvc(), "AthExDecayCnv" );

  AthExDecayCnv_p1 cnv;
  AthExDecay_PERS *persObj = cnv.createPersistent(transCont, msg);

  msg << MSG::DEBUG << "::createPersistent [Success]" << endmsg;
  return persObj; 
}

AthExDecay* AthExDecayCnv::createTransient() 
{
   MsgStream msg( msgSvc(), "AthExDecayConverter" );

   AthExDecay *transObj = 0;

   static const pool::Guid p1_guid("FD6FC38A-2EE4-4EC6-916B-DBF6A8E88A03");

   if ( compareClassGuid(p1_guid) ) {

     // using unique_ptr ensures deletion of the persistent object
     std::unique_ptr<AthExDecay_p1> persObj( poolReadObject<AthExDecay_p1>() );
     AthExDecayCnv_p1 cnv;
     transObj = cnv.createTransient( persObj.get(), msg );
     
   } else {
     throw std::runtime_error("Unsupported persistent version of Decay");
   }

   return transObj;
}
