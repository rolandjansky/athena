///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// NeutrinoContainerCnv.cxx 
// Implementation file for class NeutrinoContainerCnv
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 

// STL includes

// Framework includes
#include "GaudiKernel/MsgStream.h"

// ParticleEventTPCnv includes
#include "ParticleEventTPCnv/NeutrinoContainerCnv_p1.h"
#include "ParticleEventTPCnv/NeutrinoContainerCnv_p2.h"

// ParticleEventAthenaPool includes
#include "NeutrinoContainerCnv.h"

/////////////////////////////////////////////////////////////////// 
/// Public methods: 
/////////////////////////////////////////////////////////////////// 

/////////////////////////////////////////////////////////////////// 
/// Const methods: 
/////////////////////////////////////////////////////////////////// 

/////////////////////////////////////////////////////////////////// 
// Protected methods: 
/////////////////////////////////////////////////////////////////// 


NeutrinoContainer_PERS* 
NeutrinoContainerCnv::createPersistent( NeutrinoContainer* transCont ) 
{
  MsgStream msg( msgSvc(), "NeutrinoContainerCnv" );

  NeutrinoContainerCnv_p2 cnv;
  NeutrinoContainer_PERS *persObj = cnv.createPersistent( transCont, msg );

  msg << MSG::DEBUG << "::createPersistent [Success]" << endreq;
  return persObj; 
}

NeutrinoContainer* NeutrinoContainerCnv::createTransient() 
{
  MsgStream msg( msgSvc(), "NeutrinoContainerConverter" );

  NeutrinoContainer *transObj = 0;

  static pool::Guid tr_guid("A83B8195-8650-4F93-A616-6059A47D3EA3");
  static pool::Guid p1_guid("2E5A2E46-E9FC-47C1-AA32-89889B3B9A85");
  static pool::Guid p2_guid("7B72B6DD-2B74-4097-8489-749EB57B47B3");

  if ( compareClassGuid(p2_guid) ) {

    // using auto_ptr ensures deletion of the persistent object
    std::auto_ptr<NeutrinoContainer_p2> persObj( poolReadObject<NeutrinoContainer_p2>() );
    NeutrinoContainerCnv_p2 cnv;
    transObj = cnv.createTransient( persObj.get(), msg );

  } else if ( compareClassGuid(p1_guid) ) {

    // using auto_ptr ensures deletion of the persistent object
    std::auto_ptr<NeutrinoContainer_p1> persObj( poolReadObject<NeutrinoContainer_p1>() );
    NeutrinoContainerCnv_p1 cnv;
    transObj = cnv.createTransient( persObj.get(), msg );

  } else if ( compareClassGuid(tr_guid) ) {

    // regular object from before the T/P separation
    return poolReadObject<NeutrinoContainer>();

  } else {
    throw std::runtime_error("Unsupported persistent version of NeutrinoContainer");
  }

  return transObj;
}
