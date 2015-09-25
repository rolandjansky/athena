///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// TruthParticleContainerCnv_p6.cxx 
// Implementation file for class TruthParticleContainerCnv_p6
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 

// STL includes

// Framework includes
#include "GaudiKernel/MsgStream.h"

// HepMC includes
#include "HepMC/GenParticle.h"
#include "GeneratorObjects/McEventCollection.h"

// McParticleKernel includes
#include "McParticleKernel/ITruthParticleCnvTool.h"

// McParticleEvent includes
#define private public
#define protected public
#include "McParticleEvent/TruthParticle.h"
#include "McParticleEvent/TruthParticleContainer.h"
#undef private
#undef protected
#include "McParticleEvent/TruthEtIsolationsContainer.h"

// McParticleEventTPCnv includes
#include "McParticleEventTPCnv/TruthParticleContainerCnv_p6.h"

#include "McParticleEvent/PileUpClassification.h"

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
TruthParticleContainerCnv_p6::persToTrans( const TruthParticleContainer_p6* pers, 
					   TruthParticleContainer* trans, 
					   MsgStream& msg ) 
{
  msg << MSG::DEBUG 
      << "Loading TruthParticleContainer from persistent state..."
      << endreq;

  if ( 0 == m_cnvTool ) {
    msg << MSG::ERROR
	<< "NULL pointer to ITruthParticleCnvTool !!" 
	<< endreq
	<< "Can't create *ANY* TruthParticleContainer !!" 
	<< endreq;
    throw std::runtime_error("NULL pointer to ITruthParticleCnvTool !!");
  }

  // convert the ElementLink<McEventCollection>
  m_genEvtCnv.persToTrans( &pers->m_genEvent, 
			   &trans->m_genEvent, 
			   msg );

  const McEventCollection* evt = trans->m_genEvent.getStorableObjectPointer();
  if ( 0 == evt ) {
    const std::string error("NULL pointer to McEventCollection !!");
    msg << MSG::ERROR << error
	<< endreq;
    throw std::runtime_error(error);
  }

  // retrieve the pileup type of this container : it is the same as the GenEvent it is linked to.
  PileUpClassification::PileuType_t putype = PileUpClassification::pileUpType(evt,pers->m_genEvent.m_elementIndex);
  // retrieve all GenEvent corresponding to this pile-up type :
  size_t firstEvt, lastEvt;
  PileUpClassification::findEventIterators(putype, evt, firstEvt, lastEvt);
  msg << MSG::DEBUG << " Converting pile-up type = "<< putype << " from McEventCollection index "<< firstEvt << " to "<< lastEvt << endreq;
  // loop on the GenEvent to fill the container.
  for( ; firstEvt != lastEvt; firstEvt++){

    if ( !m_cnvTool->convert( evt, 
                              firstEvt,
                              trans ).isSuccess() ) {
      msg << MSG::ERROR
          << "Problem converting HepMC::GenEvent ["
          << trans->m_genEvent.dataID()      << "][" 
          << firstEvt << "]"
          << " to TruthParticleContainer !"
          << endreq;
      throw std::runtime_error("No TruthParticleContainer created !!");
    }
  }

  if ( !trans->m_genEvent.isValid() ) {
    const std::string err = "ElementLink to McEventCollection is not valid !";
    msg << MSG::ERROR << err << endreq;
    throw std::runtime_error(err);
  }

  // convert the ElementLink<TruthEtIsolationsContainer>
  // this needs to be done *AFTER* the GenEvent->TPContainer conversion !!
  m_etIsolCnv.persToTrans( &pers->m_etIsolations, 
			   &trans->m_etIsolations, 
			   msg );

  msg << MSG::DEBUG 
      << "Loaded TruthParticleContainer from persistent state [OK]"
      << endreq;
  return;
}

void 
TruthParticleContainerCnv_p6::transToPers( const TruthParticleContainer* trans,
					   TruthParticleContainer_p6* pers, 
					   MsgStream& msg ) 
{
  msg << MSG::DEBUG 
      << "Creating persistent state of TruthParticleContainer..."
      << endreq;

  // convert the ElementLink<McEventCollection>
  m_genEvtCnv.transToPers( &trans->m_genEvent, 
                           &pers->m_genEvent, 
                           msg );

  if ( !trans->m_genEvent.isValid() ) {
    msg << MSG::WARNING
        << "Transient ElementLink is NOT valid !!" << endreq;
  }

  // convert the ElementLink<TruthEtIsolationsContainer>
  m_etIsolCnv.transToPers( &trans->m_etIsolations, 
                           &pers->m_etIsolations, 
                           msg );

  return;
}

/////////////////////////////////////////////////////////////////// 
// Non-const methods: 
/////////////////////////////////////////////////////////////////// 

/////////////////////////////////////////////////////////////////// 
// Protected methods: 
/////////////////////////////////////////////////////////////////// 

/////////////////////////////////////////////////////////////////// 
// Const methods: 
/////////////////////////////////////////////////////////////////// 

/////////////////////////////////////////////////////////////////// 
// Non-const methods: 
/////////////////////////////////////////////////////////////////// 



#include "RootTruthParticleCnvTool.h"
T_TPCnv<TruthParticleContainer, TruthParticleContainer_p6>::T_TPCnv()
{
  m_cnvTool = new RootTruthParticleCnvTool;
}


T_TPCnv<TruthParticleContainer, TruthParticleContainer_p6>::~T_TPCnv()
{
  delete m_cnvTool;
}
