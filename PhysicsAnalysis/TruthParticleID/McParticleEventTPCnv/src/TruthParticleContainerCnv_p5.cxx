///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// TruthParticleContainerCnv_p5.cxx 
// Implementation file for class TruthParticleContainerCnv_p5
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
#include "McParticleEventTPCnv/TruthParticleContainerCnv_p5.h"

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
TruthParticleContainerCnv_p5::persToTrans( const TruthParticleContainer_p5* pers, 
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

  if ( !m_cnvTool->convert( evt, 
			    pers->m_genEvent.m_elementIndex,
			    trans ).isSuccess() ) {
    msg << MSG::ERROR
	<< "Problem converting HepMC::GenEvent ["
	<< pers->m_genEvent.m_contName     << "][" 
	<< pers->m_genEvent.m_elementIndex << "]"
	<< " to TruthParticleContainer !"
	<< endreq;
    throw std::runtime_error("No TruthParticleContainer created !!");
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
TruthParticleContainerCnv_p5::transToPers( const TruthParticleContainer*,
					   TruthParticleContainer_p5*, 
					   MsgStream& msg ) 
{
  msg << MSG::DEBUG 
      << "Creating persistent state of TruthParticleContainer..."
      << endreq;
  
  msg << MSG::ERROR
      << "This transient-to-persistent converter method has been RETIRED !!"
      << endreq
      << "You are not supposed to end-up here ! Go away !"
      << endreq;

  throw std::runtime_error( "Retired TruthParticleContainerCnv_p4::transToPers() !!" );

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
T_TPCnv<TruthParticleContainer, TruthParticleContainer_p5>::T_TPCnv()
{
  m_cnvTool = new RootTruthParticleCnvTool;
}


T_TPCnv<TruthParticleContainer, TruthParticleContainer_p5>::~T_TPCnv()
{
  delete m_cnvTool;
}
