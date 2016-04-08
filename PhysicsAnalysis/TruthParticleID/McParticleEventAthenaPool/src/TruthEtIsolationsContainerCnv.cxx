///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// TruthEtIsolationsContainerCnv.cxx 
// Implementation file for class TruthEtIsolationsContainerCnv
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 

// STL includes

// Framework includes
#include "GaudiKernel/MsgStream.h"

// McParticleEventTPCnv includes
#include "McParticleEventTPCnv/TruthEtIsolationsContainerCnv_p1.h"

// McParticleEventAthenaPool includes
#include "TruthEtIsolationsContainerCnv.h"

/////////////////////////////////////////////////////////////////// 
/// Public methods: 
/////////////////////////////////////////////////////////////////// 

/////////////////////////////////////////////////////////////////// 
/// Const methods: 
/////////////////////////////////////////////////////////////////// 

/////////////////////////////////////////////////////////////////// 
// Protected methods: 
/////////////////////////////////////////////////////////////////// 

TruthEtIsolationsContainerCnv::TruthEtIsolationsContainerCnv(ISvcLocator* svcLocator) :
  T_AthenaPoolCustomCnv<TruthEtIsolationsContainer, 
                        TruthEtIsolationsContainer_PERS>(svcLocator)
{}


TruthEtIsolationsContainer_PERS* 
TruthEtIsolationsContainerCnv::createPersistent(TruthEtIsolationsContainer* trans)
{
  MsgStream msg( msgSvc(), "TruthEtIsolationsContainerCnv" );

  TruthEtIsolationsContainerCnv_p1 cnv;
  TruthEtIsolationsContainer_PERS *pers = cnv.createPersistent(trans, msg);

  msg << MSG::DEBUG << "::createPersistent [Success]" << endreq;
  return pers; 
}

TruthEtIsolationsContainer* TruthEtIsolationsContainerCnv::createTransient() 
{
   MsgStream msg( msgSvc(), "TruthEtIsolationsContainerCnv" );

   TruthEtIsolationsContainer *trans = 0;

   //static pool::Guid tr_guid("003AE8C0-B033-4470-9F3D-869724F34E48");
   static pool::Guid p1_guid("EB6EC9D5-4D99-4565-9E4A-65BE1C21B35D");

   if ( compareClassGuid(p1_guid) ) {
     
     // using auto_ptr ensures deletion of the persistent object
     std::auto_ptr<TruthEtIsolationsContainer_p1> pers( poolReadObject<TruthEtIsolationsContainer_p1>() );
     TruthEtIsolationsContainerCnv_p1 cnv;
     trans = cnv.createTransient( pers.get(), msg );

   } else {
     throw std::runtime_error("Unsupported persistent version of TruthEtIsolationsContainer");
   }

   return trans;
}
