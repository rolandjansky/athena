///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// AthExParticlesCnv.cxx 
// Implementation file for class AthExParticlesCnv
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 

// STL includes

// Framework includes
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IToolSvc.h"
#include "GaudiKernel/IAlgTool.h"
#include "StoreGate/StoreGateSvc.h"

// AthExThinning includes
#include "AthExParticlesCnv_p1.h"
#include "AthExParticlesCnv.h"

/////////////////////////////////////////////////////////////////// 
/// Public methods: 
/////////////////////////////////////////////////////////////////// 

/////////////////////////////////////////////////////////////////// 
/// Const methods: 
/////////////////////////////////////////////////////////////////// 

/////////////////////////////////////////////////////////////////// 
// Protected methods: 
/////////////////////////////////////////////////////////////////// 

AthExParticlesCnv::AthExParticlesCnv(ISvcLocator* svcLocator) :
  T_AthenaPoolCustomCnv<AthExParticles, 
                        AthExParticles_PERS>(svcLocator)
{
  if ( 0 == svcLocator ) {
    throw std::runtime_error("NULL pointer to ISvcLocator !!");
  }

}


AthExParticles_PERS* 
AthExParticlesCnv::createPersistent(AthExParticles* transCont)
{
  MsgStream msg( msgSvc(), "AthExParticlesCnv" );

  AthExParticlesCnv_p1 cnv;
  AthExParticles_PERS *persObj = cnv.createPersistent(transCont, msg);

  msg << MSG::DEBUG << "::createPersistent [Success]" << endreq;
  return persObj; 
}

AthExParticles* AthExParticlesCnv::createTransient() 
{
   MsgStream msg( msgSvc(), "AthExParticlesConverter" );

   AthExParticles *transObj = 0;

   static pool::Guid p1_guid("F64F4C19-DBDB-4347-9908-6BB69554EC64");

   if ( compareClassGuid(p1_guid) ) {

     // using auto_ptr ensures deletion of the persistent object
     std::auto_ptr<AthExParticles_p1> persObj( poolReadObject<AthExParticles_p1>() );
     AthExParticlesCnv_p1 cnv;
     transObj = cnv.createTransient( persObj.get(), msg );
     
   } else {
     throw std::runtime_error("Unsupported persistent version of AthExParticles");
   }

   return transObj;
}
