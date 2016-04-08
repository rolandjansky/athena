///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// INav4MomAssocsCnv.cxx 
// Implementation file for class INav4MomAssocsCnv
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 

// STL includes

// Framework includes
#include "GaudiKernel/MsgStream.h"
#include "StoreGate/StoreGateSvc.h"

// EventCommonAthenaPool includes
#include "INav4MomAssocsCnv.h"

/////////////////////////////////////////////////////////////////// 
/// Public methods: 
/////////////////////////////////////////////////////////////////// 

INav4MomAssocsCnv::INav4MomAssocsCnv( ISvcLocator* svcLocator ) :
  T_AthenaPoolCustomCnv<INav4MomAssocs, INav4MomAssocs_PERS>(svcLocator)
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


INav4MomAssocs_PERS* 
INav4MomAssocsCnv::createPersistent( INav4MomAssocs* transCont ) 
{
  MsgStream log( msgSvc(), "INav4MomAssocsCnv" );

  INav4MomAssocs_PERS *persObj = m_tpConverter_p3.createPersistent( transCont, log );
  
  log << MSG::DEBUG << "::createPersistent [Success]" << endreq;
  return persObj; 
}

INav4MomAssocs* INav4MomAssocsCnv::createTransient() 
{
  MsgStream log( msgSvc(), "INav4MomAssocsConverter" );

  INav4MomAssocs *transObj = 0;

  static pool::Guid tr_guid("DF941034-CCB2-4A1A-9A8C-5F3D5EAFD08C");
  static pool::Guid p1_guid("85027AE1-4FD4-4808-B151-5485634ACD5D");
  static pool::Guid p2_guid("8040BEAA-BC65-43B8-B468-A7157C89ACB3");
  static pool::Guid p3_guid("455AEE6B-9834-4E72-8F81-2532A52E3BE7");

  if( compareClassGuid(p3_guid) ) {
    // using auto_ptr ensures deletion of the persistent object
    std::auto_ptr<INav4MomAssocs_p3> persObj( poolReadObject<INav4MomAssocs_p3>() );
    transObj =  m_tpConverter_p3.createTransient( persObj.get(), log );
  }
  else if ( compareClassGuid(p2_guid) ) {
    // using auto_ptr ensures deletion of the persistent object
    std::auto_ptr<INav4MomAssocs_p2> persObj( poolReadObject<INav4MomAssocs_p2>() );
    transObj = m_tpConverter_p2.createTransient( persObj.get(), log );
  }
  else if( compareClassGuid(p1_guid) ) {
    // using auto_ptr ensures deletion of the persistent object
    std::auto_ptr<INav4MomAssocs_p1> persObj( poolReadObject<INav4MomAssocs_p1>() );
    INav4MomAssocsCnv_p1 cnv( m_storeGate );
    transObj = cnv.createTransient( persObj.get(), log );
  }
  else if( compareClassGuid(tr_guid) ) {
     // regular object from before the T/P separation
     return poolReadObject<INav4MomAssocs>();
  }
  else {
    throw std::runtime_error("Unsupported persistent version of INav4MomAssocs");
  }

  return transObj;
}
