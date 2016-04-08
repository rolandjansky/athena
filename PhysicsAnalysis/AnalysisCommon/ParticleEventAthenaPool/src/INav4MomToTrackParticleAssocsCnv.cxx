///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// INav4MomToTrackParticleAssocsCnv.cxx 
// Implementation file for class INav4MomToTrackParticleAssocsCnv
// Author: Karsten Koeneke
/////////////////////////////////////////////////////////////////// 

// STL includes

// Framework includes
#include "GaudiKernel/MsgStream.h"
#include "StoreGate/StoreGateSvc.h"

// EventCommonAthenaPool includes
#include "INav4MomToTrackParticleAssocsCnv.h"

/////////////////////////////////////////////////////////////////// 
/// Public methods: 
/////////////////////////////////////////////////////////////////// 

INav4MomToTrackParticleAssocsCnv::INav4MomToTrackParticleAssocsCnv( ISvcLocator* svcLocator ) :
  T_AthenaPoolCustomCnv<INav4MomToTrackParticleAssocs, INav4MomToTrackParticleAssocs_PERS>(svcLocator)
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


INav4MomToTrackParticleAssocs_PERS* 
INav4MomToTrackParticleAssocsCnv::createPersistent( INav4MomToTrackParticleAssocs* transCont ) 
{
  MsgStream log( msgSvc(), "INav4MomToTrackParticleAssocsCnv" );

  INav4MomToTrackParticleAssocs_PERS *persObj = m_tpConverter_p1.createPersistent( transCont, log );
  
  log << MSG::DEBUG << "::createPersistent [Success]" << endreq;
  return persObj; 
}



INav4MomToTrackParticleAssocs* INav4MomToTrackParticleAssocsCnv::createTransient() 
{
  MsgStream log( msgSvc(), "INav4MomToTrackParticleAssocsConverter" );

  INav4MomToTrackParticleAssocs *transObj = 0;

  static pool::Guid tr_guid("90773961-FC8A-4535-B621-6BB62E4AE2D9");
  static pool::Guid p1_guid("7DDB5B30-918F-44C2-BBEE-12D671C1C3DC");

  if( compareClassGuid(p1_guid) ) {
    // using auto_ptr ensures deletion of the persistent object
    std::auto_ptr<INav4MomToTrackParticleAssocs_p1> persObj( poolReadObject<INav4MomToTrackParticleAssocs_p1>() );
    transObj =  m_tpConverter_p1.createTransient( persObj.get(), log );
  }
  else if( compareClassGuid(tr_guid) ) {
     // regular object from before the T/P separation
     return poolReadObject<INav4MomToTrackParticleAssocs>();
  }
  else {
    throw std::runtime_error("Unsupported persistent version of INav4MomToTrackParticleAssocs");
  }

  return transObj;
}
