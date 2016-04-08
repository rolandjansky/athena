///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ParticleShallowCloneContainerCnv.cxx 
// Implementation file for class ParticleShallowCloneContainerCnv
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 

// STL includes

// Framework includes
#include "GaudiKernel/MsgStream.h"

// ParticleEventTPCnv includes
#include "ParticleEventTPCnv/ParticleShallowCloneContainerCnv_p1.h"

// ParticleEventAthenaPool includes
#include "ParticleShallowCloneContainerCnv.h"

/////////////////////////////////////////////////////////////////// 
/// Public methods: 
/////////////////////////////////////////////////////////////////// 

ParticleShallowCloneContainerCnv::ParticleShallowCloneContainerCnv( ISvcLocator* svcLocator ) :
  T_AthenaPoolCustomCnv<ParticleShallowCloneContainer, 
                        ParticleShallowCloneContainer_PERS>(svcLocator)
{}

/////////////////////////////////////////////////////////////////// 
/// Const methods: 
/////////////////////////////////////////////////////////////////// 

/////////////////////////////////////////////////////////////////// 
// Protected methods: 
/////////////////////////////////////////////////////////////////// 


ParticleShallowCloneContainer_PERS* 
ParticleShallowCloneContainerCnv::createPersistent( ParticleShallowCloneContainer* transCont ) 
{
  MsgStream msg( msgSvc(), "ParticleShallowCloneContainerCnv" );

  ParticleShallowCloneContainerCnv_p1 cnv;
  ParticleShallowCloneContainer_PERS *persObj = cnv.createPersistent( transCont, msg );

  if (msgLvl(MSG::DEBUG)) msg << MSG::DEBUG << "::createPersistent [Success]" << endreq;
  return persObj; 
}

ParticleShallowCloneContainer* ParticleShallowCloneContainerCnv::createTransient() 
{
  MsgStream msg( msgSvc(), "ParticleShallowCloneContainerConverter" );

  ParticleShallowCloneContainer *transObj = 0;

  static pool::Guid tr_guid("4684AED0-3C6C-4333-91D2-07447655BF6A");
  static pool::Guid p1_guid("EF004D41-FFCD-4E68-85D7-7F39186158E5");

  if ( compareClassGuid(tr_guid) ) {

    throw std::runtime_error("ParticleShallowCloneContainer from before T/P separation is not readable anymore");

  } else if ( compareClassGuid(p1_guid) ) {

    // using auto_ptr ensures deletion of the persistent object
    std::auto_ptr<ParticleShallowCloneContainer_p1> persObj( poolReadObject<ParticleShallowCloneContainer_p1>() );
    ParticleShallowCloneContainerCnv_p1 cnv;
    transObj = cnv.createTransient( persObj.get(), msg );
  } else {
    throw std::runtime_error("Unsupported persistent version of ParticleShallowCloneContainer");
  }

  return transObj;
}
