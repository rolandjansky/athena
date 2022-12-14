///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// CompositeParticleCnv_p1.cxx 
// Implementation file for class CompositeParticleCnv_p1
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 


// STL includes

// ParticleEvent includes
#include "ParticleEvent/CompositeParticle.h"

// DataModelAthenaPool includes
#include "DataModelAthenaPool/NavigableCnv_p1.h"

// EventCommonTPCnv includes
#include "EventCommonTPCnv/P4ImplPxPyPzECnv_p1.h"

// ParticleEventTPCnv includes
#include "ParticleEventTPCnv/ParticleBaseCnv_p1.h"
#include "ParticleEventTPCnv/CompositeParticleCnv_p1.h"

typedef NavigableCnv_p1<
           Navigable<INavigable4MomentumCollection,double>
           > NavigableCnv_t;

// pre-allocate converters
static const P4ImplPxPyPzECnv_p1   s_momCnv;
static const NavigableCnv_t        s_navCnv;
static const ParticleBaseCnv_p1    s_pbCnv;


void CompositeParticleCnv_p1::persToTrans( const CompositeParticle_p1* pers,
                                           CompositeParticle* trans, 
                                           MsgStream& msg ) const
{
//   msg << MSG::DEBUG << "Loading CompositeParticle from persistent state..."
//       << endmsg;
  s_navCnv.persToTrans( &pers->m_nav,      
                        &trans->navigableBase(), 
                        msg );
  s_momCnv.persToTrans( &pers->m_momentum,
                        &trans->momentumBase(),  
                        msg );
  s_pbCnv.persToTrans( &pers->m_particle,
                       &trans->particleBase(),
                       msg );

//   msg << MSG::DEBUG << "Loaded CompositeParticle from persistent state [OK]"
//       << endmsg;
  return;
}

void CompositeParticleCnv_p1::transToPers( const CompositeParticle* trans, 
                                           CompositeParticle_p1* pers, 
                                           MsgStream& msg ) const
{
//   msg << MSG::DEBUG << "Creating persistent state of CompositeParticle..."
//       << endmsg;

  s_navCnv.transToPers( &trans->navigableBase(), 
                        &pers->m_nav,      
                        msg );
  s_momCnv.transToPers( &trans->momentumBase(),  
                        &pers->m_momentum,
                        msg );
  s_pbCnv.transToPers( &trans->particleBase(),  
                       &pers->m_particle,
                       msg );

//   msg << MSG::DEBUG << "Created persistent state of CompositeParticle [OK]"
//       << endmsg;
  return;
}
