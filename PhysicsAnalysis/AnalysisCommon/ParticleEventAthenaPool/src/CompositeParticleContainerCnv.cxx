///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// CompositeParticleContainerCnv.cxx 
// Implementation file for class CompositeParticleContainerCnv
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 

// STL includes

// Framework includes
#include "GaudiKernel/MsgStream.h"

// ParticleEventTPCnv includes
#include "ParticleEventTPCnv/CompositeParticleContainerCnv_p1.h"

// ParticleEventAthenaPool includes
#include "CompositeParticleContainerCnv.h"

/////////////////////////////////////////////////////////////////// 
/// Public methods: 
/////////////////////////////////////////////////////////////////// 

CompositeParticleContainerCnv::CompositeParticleContainerCnv( ISvcLocator* svcLocator ) :
  T_AthenaPoolCustomCnv<CompositeParticleContainer, 
                        CompositeParticleContainer_PERS>(svcLocator)
{}

/////////////////////////////////////////////////////////////////// 
/// Const methods: 
/////////////////////////////////////////////////////////////////// 

/////////////////////////////////////////////////////////////////// 
// Protected methods: 
/////////////////////////////////////////////////////////////////// 


CompositeParticleContainer_PERS* 
CompositeParticleContainerCnv::createPersistent( CompositeParticleContainer* transCont ) 
{
  MsgStream msg( msgSvc(), "CompositeParticleContainerCnv" );

  CompositeParticleContainerCnv_p1 cnv;
  CompositeParticleContainer_PERS *persObj = cnv.createPersistent( transCont, msg );

  msg << MSG::DEBUG << "::createPersistent [Success]" << endreq;
  return persObj; 
}

CompositeParticleContainer* CompositeParticleContainerCnv::createTransient() 
{
  MsgStream msg( msgSvc(), "CompositeParticleContainerConverter" );

  CompositeParticleContainer *transObj = 0;

  static pool::Guid tr_guid("E14D5B90-5CCE-38F2-B2AD-E5EFFB2ADB37");
  static pool::Guid p1_guid("6CFA134E-F669-435B-B20E-FA9EB2F51DCD");

  if ( compareClassGuid(tr_guid) ) {

    throw std::runtime_error("CompositeParticleContainer from before T/P separation is not readable anymore");

  } else if ( compareClassGuid(p1_guid) ) {

    // using auto_ptr ensures deletion of the persistent object
    std::auto_ptr<CompositeParticleContainer_p1> persObj( poolReadObject<CompositeParticleContainer_p1>() );
    CompositeParticleContainerCnv_p1 cnv;
    transObj = cnv.createTransient( persObj.get(), msg );
  } else {
    throw std::runtime_error("Unsupported persistent version of CompositeParticleContainer");
  }

  return transObj;
}
