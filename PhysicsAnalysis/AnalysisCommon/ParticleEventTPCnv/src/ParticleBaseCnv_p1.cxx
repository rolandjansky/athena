///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// ParticleBaseCnv_p1.cxx 
// Implementation file for class ParticleBaseCnv_p1
// Author: K.Cranmer<cranmer@cern.ch>
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 


// STL includes

// DataModelAthenaPool includes
#include "DataModelAthenaPool/ElementLinkCnv_p1.h"
#include "DataModelAthenaPool/AthenaBarCodeCnv_p1.h"

// VxVertex includes
#include "VxVertex/VxContainer.h"

// ParticleEvent includes
#include "ParticleEvent/ParticleBase.h"
#include "ParticleEvent/Lib/Base.h"

// ParticleEventTPCnv includes
#include "ParticleEventTPCnv/ParticleBaseCnv_p1.h"

// internal: do not change
//#define PBC_P1_VERBOSE 1
//

typedef ElementLinkCnv_p1<ElementLink<VxContainer> > VxLinkCnv_t;

// pre-allocate converters
static const VxLinkCnv_t    vxCnv;
static const AthenaBarCodeCnv_p1   s_abcCnv;

/** d-tor
 */
ParticleBaseCnv_p1::~ParticleBaseCnv_p1()
{}

/////////////////////////////////////////////////////////////////// 
// Non-Const methods: 
///////////////////////////////////////////////////////////////////

void ParticleBaseCnv_p1::persToTrans( const ParticleBase_p1* pers,
				      ParticleBase* trans, 
				      MsgStream& msg ) const
{
//   msg << MSG::DEBUG << "Loading ParticleBase from persistent state..."
//       << endmsg;

  ElementLink<VxContainer> origin;
  vxCnv.persToTrans( &pers->m_origin, &origin, msg );

  trans->set_origin    (origin);
  if (pers->m_hasCharge) {
    trans->set_charge    (pers->m_charge);
  }
  else {
    trans->reset_charge();
  }
  if (pers->m_hasPdgId) {
    trans->set_pdgId     (static_cast<PDG::pidType>( pers->m_pdgId ));
  }
  else {
    trans->reset_pdgId();
  }
  trans->set_dataType  (static_cast<ParticleDataType::DataType>( pers->m_dataType ));

//   msg << MSG::DEBUG << "Loaded ParticleBase from persistent state [OK]"
//       << endmsg;

  return;
}

void ParticleBaseCnv_p1::transToPers( const ParticleBase* trans, 
				      ParticleBase_p1* pers, 
				      MsgStream& msg ) const
{
//   msg << MSG::DEBUG << "Creating persistent state of ParticleBase..."
//       << endmsg;

  vxCnv.transToPers( &trans->originLink(), &pers->m_origin, msg );

  pers->m_charge    = trans->charge();
  pers->m_hasCharge = trans->hasCharge();
  pers->m_pdgId     = trans->pdgId();
  pers->m_hasPdgId  = trans->hasPdgId();
  pers->m_dataType  = trans->dataType();

//   msg << MSG::DEBUG << "Created persistent state of ParticleBase [OK]"
//       << endmsg;
  return;
}


void ParticleBaseCnv_p1::persToTrans( const ParticleBase_p1* pers,
				      ParticleEvent::Base* trans, 
				      MsgStream& msg ) const
{
//   msg << MSG::DEBUG << "Loading ParticleBase from persistent state..."
//       << endmsg;

  ElementLink<VxContainer> origin;
  vxCnv.persToTrans( &pers->m_origin, &origin, msg );

  trans->set_origin    (origin);
  if (pers->m_hasCharge) {
    trans->set_charge    (pers->m_charge);
  }
  else {
    trans->reset_charge();
  }
  if (pers->m_hasPdgId) {
    trans->set_pdgId     (static_cast<PDG::pidType>( pers->m_pdgId ));
  }
  else {
    trans->reset_pdgId();
  }
  trans->set_dataType  (static_cast<ParticleDataType::DataType>( pers->m_dataType ));

  // convert AthenaBarCode
  s_abcCnv.persToTrans((const AthenaBarCode_p1*)(&(pers->m_athenabarcode)),
		       &(trans->getAthenaBarCodeImpl()), msg);


//   msg << MSG::DEBUG << "Loaded ParticleBase from persistent state [OK]"
//       << endmsg;

  return;
}

void ParticleBaseCnv_p1::transToPers( const ParticleEvent::Base* trans, 
				      ParticleBase_p1* pers, 
				      MsgStream& msg ) const
{
//   msg << MSG::DEBUG << "Creating persistent state of ParticleBase..."
//       << endmsg;

#ifdef PBC_P1_VERBOSE
    msg << MSG::WARNING 
	<< "an old T->P ParticleBase converter is being called!"
	// << endmsg
	// << "consider migrating to ParticleBase(Cnv)_p2 !" << endmsg
	// << "this method will be removed for 16.0.0" 
	<< endmsg;
#endif
  

  vxCnv.transToPers( &trans->originLink(), &pers->m_origin, msg );

  pers->m_charge    = trans->charge();
  pers->m_hasCharge = trans->hasCharge();
  pers->m_pdgId     = trans->pdgId();
  pers->m_hasPdgId  = trans->hasPdgId();
  pers->m_dataType  = trans->dataType();

  // convert AthenaBarCode
  s_abcCnv.transToPers((const AthenaBarCodeImpl*)(&(trans->getAthenaBarCodeImpl())), (AthenaBarCode_p1*)(&(pers->m_athenabarcode)),msg);

//   msg << MSG::DEBUG << "Created persistent state of ParticleBase [OK]"
//       << endmsg;
  return;
}
