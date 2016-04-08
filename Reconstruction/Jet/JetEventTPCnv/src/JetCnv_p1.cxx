///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// JetCnv_p1.cxx 
// Implementation file for class JetCnv_p1
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 


// STL includes

// JetEvent includes
#include "JetEvent/Jet.h"

// DataModelAthenaPool includes
#include "DataModelAthenaPool/NavigableCnv_p1.h"

// EventCommonTPCnv includes
#include "EventCommonTPCnv/P4ImplPxPyPzECnv_p1.h"

// JetEventTPCnv includes
#include "JetEventTPCnv/JetCnv_p1.h"

typedef NavigableCnv_p1<
           Navigable<INavigable4MomentumCollection,double>
           > NavigableCnv_t;

// pre-allocate converters
static P4ImplPxPyPzECnv_p1   momCnv;
static NavigableCnv_t    navCnv;

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

void JetCnv_p1::persToTrans( const Jet_p1* pers,
                             Jet* trans, 
                             MsgStream& msg ) 
{
//   msg << MSG::DEBUG << "Loading Jet from persistent state..."
//       << endreq;
  navCnv.persToTrans( &pers->m_nav,      
		      &trans->navigableBase(), 
		      msg );
  momCnv.persToTrans( &pers->m_momentum, &trans->momentumBase(),  msg );

  trans->setSignalState(P4SignalState::JETFINAL);
//   msg << MSG::DEBUG << "Loaded Jet from persistent state [OK]"
//       << endreq;

  // Jet comes from a DataPool.
  // So we need to be sure to re-initialize everything in the Jet.
  trans->setCombinedLikelihood (std::vector<double>());
  trans->setJetAuthor ("Unknown");
  if (trans->m_tagInfoStore)
    trans->m_tagInfoStore->clear();
  // if (trans->m_shapeStore)
  //   trans->m_shapeStore->clear();
  if (trans->m_assocStore)
    trans->m_assocStore->clear();

  trans->setRawE( trans->e() ) ;
  trans->setRawPx( trans->px() ) ;
  trans->setRawPy( trans->py() ) ;
  trans->setRawPz( trans->pz() ) ;

  static Jet jtmp;
  trans->particleBase() = jtmp.particleBase();
  
  return;
}

void JetCnv_p1::transToPers( const Jet* /*trans*/, 
                             Jet_p1* /*pers*/, 
                             MsgStream& /*msg*/ ) 
{
//   msg << MSG::DEBUG << "Creating persistent state of Jet..."
//       << endreq;

  // navCnv.transToPers( &trans->navigableBase(), 
  //       	      &pers->m_nav,      msg );
  // momCnv.transToPers( &trans->momentumBase(),  &pers->m_momentum, msg );

//   msg << MSG::DEBUG << "Created persistent state of Jet [OK]"
//       << endreq;
  return;
}
