///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// TrackParticleAssocsCnv.cxx 
// Implementation file for class TrackParticleAssocsCnv
// Author: S.Binet<binet@cern.ch> C.Anastopoulos
/////////////////////////////////////////////////////////////////// 

// STL includes

// Framework includes
#include "GaudiKernel/MsgStream.h"
#include "StoreGate/StoreGateSvc.h"

// EventCommonAthenaPool includes
#include "TrackParticleAssocsCnv.h"

/////////////////////////////////////////////////////////////////// 
/// Public methods: 
/////////////////////////////////////////////////////////////////// 

TrackParticleAssocsCnv::TrackParticleAssocsCnv( ISvcLocator* svcLocator ) :
  T_AthenaPoolCustomCnv<TrackParticleAssocs, TrackParticleAssocs_PERS>(svcLocator)
{
  if ( 0 == svcLocator ) {
    throw std::runtime_error("NULL pointer to ISvcLocator !!");
  }

  if ( svcLocator->service("StoreGateSvc",m_storeGate).isFailure() || 
       0 == m_storeGate ) {
    throw std::runtime_error("Could not fetch StoreGateSvc !!");
  }
}

/////////////////////////////////////////////////////////////////// 
/// Const methods: 
/////////////////////////////////////////////////////////////////// 

/////////////////////////////////////////////////////////////////// 
// Protected methods: 
/////////////////////////////////////////////////////////////////// 


TrackParticleAssocs_PERS* 
TrackParticleAssocsCnv::createPersistent( TrackParticleAssocs* transCont ) 
{
  MsgStream log( msgSvc(), "TrackParticleAssocsCnv" );

  TrackParticleAssocs_PERS *persObj = m_tpConverter_p1.createPersistent( transCont, log );
  
  log << MSG::DEBUG << "::createPersistent [Success]" << endreq;
  return persObj; 
}

TrackParticleAssocs* TrackParticleAssocsCnv::createTransient() 
{
  MsgStream log( msgSvc(), "TrackParticleAssocsConverter" );

  TrackParticleAssocs *transObj = 0;

  static pool::Guid tr_guid("6BB74DC6-5142-4FF4-A8C8-C0FDF1C5F25F");
  static pool::Guid p1_guid("42E21200-3433-489A-BED9-D4F2BDC97102");

  if( compareClassGuid(p1_guid) ) {
    // using auto_ptr ensures deletion of the persistent object
    std::auto_ptr<TrackParticleAssocs_p1> persObj( poolReadObject<TrackParticleAssocs_p1>() );
    transObj =  m_tpConverter_p1.createTransient( persObj.get(), log );
  }
  else if( compareClassGuid(tr_guid) ) {
     // regular object from before the T/P separation
     return poolReadObject<TrackParticleAssocs>();
  }
  else {
    throw std::runtime_error("Unsupported persistent version of TrackParticleAssocs");
  }

  return transObj;
}
