///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// NeutrinoCnv_p1.cxx 
// Implementation file for class NeutrinoCnv_p1
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 


// STL includes

// ParticleEvent includes
#include "ParticleEvent/Neutrino.h"

// EventCommonTPCnv includes
#include "EventCommonTPCnv/P4ImplPxPyPzECnv_p1.h"

// ParticleEventTPCnv includes
#include "ParticleEventTPCnv/ParticleBaseCnv_p1.h"

// ParticleEventTPCnv includes
#include "ParticleEventTPCnv/NeutrinoCnv_p1.h"

// converters
static P4ImplPxPyPzECnv_p1 s_momCnv;
static ParticleBaseCnv_p1  s_partBaseCnv;

/////////////////////////////////////////////////////////////////// 
// Public methods: 
/////////////////////////////////////////////////////////////////// 

// Constructors
////////////////

// Destructor
///////////////

/////////////////////////////////////////////////////////////////// 
// Const methods: 
///////////////////////////////////////////////////////////////////

void 
NeutrinoCnv_p1::persToTrans( const Neutrino_p1* pers,
			     Neutrino* trans, 
			     MsgStream& msg ) 
{
//   msg << MSG::DEBUG << "Loading Neutrino from persistent state..."
//       << endreq;

  // base classes
  s_momCnv.persToTrans( &pers->m_momentum,     
			&trans->momentumBase(), 
			msg );
  s_partBaseCnv.persToTrans( &pers->m_particleBase, 
			     &trans->particleBase(), 
			     msg );

//   msg << MSG::DEBUG << "Loaded Neutrino from persistent state [OK]"
//       << endreq;
  return;
}

void 
NeutrinoCnv_p1::transToPers( const Neutrino* trans, 
			     Neutrino_p1* pers, 
			     MsgStream& msg ) 
{
//   msg << MSG::DEBUG << "Creating persistent state of Neutrino..."
//       << endreq;

  // base classes
  s_momCnv.transToPers( &trans->momentumBase(), 
			&pers->m_momentum,
			msg );
  s_partBaseCnv.transToPers( &trans->particleBase(), 
			     &pers->m_particleBase, 
			     msg );

//   msg << MSG::DEBUG << "Created persistent state of Neutrino [OK]"
//       << endreq;
  return;
}
