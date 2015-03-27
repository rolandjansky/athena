///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ParticleBaseCnv_p2.cxx 
// Implementation file for class ParticleBaseCnv_p2
// Author: K.Cranmer<cranmer@cern.ch>
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 


// STL includes

// DataModelAthenaPool includes
#include "DataModelAthenaPool/ElementLinkCnv_p3.h"
#include "DataModelAthenaPool/AthenaBarCodeCnv_p1.h"

// VxVertex includes
#include "VxVertex/VxContainer.h"

// ParticleEvent includes
#define private public
#define protected public
#include "ParticleEvent/ParticleBase.h"
#include "ParticleEvent/Lib/Base.h"
#undef private
#undef protected

// ParticleEventTPCnv includes
#include "ParticleEventTPCnv/ParticleBaseCnv_p2.h"

typedef ElementLinkCnv_p3<ElementLink<VxContainer> > VxLinkCnv_t;

// pre-allocate converters
static VxLinkCnv_t    vxCnv;
static AthenaBarCodeCnv_p1   s_abcCnv;

/** d-tor
 */
ParticleBaseCnv_p2::~ParticleBaseCnv_p2()
{}

/////////////////////////////////////////////////////////////////// 
// Non-Const methods: 
///////////////////////////////////////////////////////////////////

void ParticleBaseCnv_p2::persToTrans( const ParticleBase_p2* pers,
				      ParticleBase* trans, 
				      MsgStream& msg ) 
{
//   msg << MSG::DEBUG << "Loading ParticleBase from persistent state..."
//       << endreq;

  vxCnv.persToTrans( &pers->m_origin, &trans->m_origin, msg );

  trans->m_charge    = pers->m_charge;
  trans->m_hasCharge = pers->m_hasCharge;
  trans->m_pdgId     = static_cast<PDG::pidType>( pers->m_pdgId );
  trans->m_hasPdgId  = pers->m_hasPdgId;
  trans->m_dataType  = static_cast<ParticleDataType::DataType>( pers->m_dataType );

//   msg << MSG::DEBUG << "Loaded ParticleBase from persistent state [OK]"
//       << endreq;

  return;
}

void ParticleBaseCnv_p2::transToPers( const ParticleBase* trans, 
				      ParticleBase_p2* pers, 
				      MsgStream& msg ) 
{
//   msg << MSG::DEBUG << "Creating persistent state of ParticleBase..."
//       << endreq;

  vxCnv.transToPers( &trans->m_origin, &pers->m_origin, msg );

  pers->m_charge    = trans->m_charge;
  pers->m_hasCharge = trans->m_hasCharge;
  pers->m_pdgId     = trans->m_pdgId;
  pers->m_hasPdgId  = trans->m_hasPdgId;
  pers->m_dataType  = trans->m_dataType;

//   msg << MSG::DEBUG << "Created persistent state of ParticleBase [OK]"
//       << endreq;
  return;
}


void ParticleBaseCnv_p2::persToTrans( const ParticleBase_p2* pers,
				      ParticleEvent::Base* trans, 
				      MsgStream& msg ) 
{
//   msg << MSG::DEBUG << "Loading ParticleBase from persistent state..."
//       << endreq;

  vxCnv.persToTrans( &pers->m_origin, &trans->m_origin, msg );

  trans->m_charge    = pers->m_charge;
  trans->m_hasCharge = pers->m_hasCharge;
  trans->m_pdgId     = static_cast<PDG::pidType>( pers->m_pdgId );
  trans->m_hasPdgId  = pers->m_hasPdgId;
  trans->m_dataType  = static_cast<ParticleDataType::DataType>( pers->m_dataType );

  // convert AthenaBarCode
  s_abcCnv.persToTrans((const AthenaBarCode_p1*)(&(pers->m_athenabarcode)),
		       &(trans->getAthenaBarCodeImpl()), msg);


//   msg << MSG::DEBUG << "Loaded ParticleBase from persistent state [OK]"
//       << endreq;

  return;
}

void ParticleBaseCnv_p2::transToPers( const ParticleEvent::Base* trans, 
				      ParticleBase_p2* pers, 
				      MsgStream& msg ) 
{
//   msg << MSG::DEBUG << "Creating persistent state of ParticleBase..."
//       << endreq;

  vxCnv.transToPers( &trans->m_origin, &pers->m_origin, msg );

  pers->m_charge    = trans->m_charge;
  pers->m_hasCharge = trans->m_hasCharge;
  pers->m_pdgId     = trans->m_pdgId;
  pers->m_hasPdgId  = trans->m_hasPdgId;
  pers->m_dataType  = trans->m_dataType;

  // convert AthenaBarCode
  s_abcCnv.transToPers((const AthenaBarCodeImpl*)(&(trans->getAthenaBarCodeImpl())), (AthenaBarCode_p1*)(&(pers->m_athenabarcode)),msg);

//   msg << MSG::DEBUG << "Created persistent state of ParticleBase [OK]"
//       << endreq;
  return;
}
