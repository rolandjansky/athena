///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// TruthParticleContainerCnv_p3.cxx 
// Implementation file for class TruthParticleContainerCnv_p3
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 


// STL includes

// Framework includes
#include "GaudiKernel/MsgStream.h"
#include "StoreGate/StoreGateSvc.h"

// HepMC includes
#include "HepMC/GenParticle.h"
#include "GeneratorObjects/McEventCollection.h"

// McParticleKernel includes
#include "McParticleKernel/ITruthParticleCnvTool.h"

// McParticleEvent includes
#define private public
#define protected public
#include "McParticleEvent/TruthParticle.h"
#undef private
#undef protected

// McParticleEventTPCnv includes
#include "McEtIsolVisitor.h"
#include "McParticleEventTPCnv/TruthParticleContainerCnv_p3.h"

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
TruthParticleContainerCnv_p3::persToTrans( const TruthParticleContainer_p3* pers, 
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

  if ( 0 == m_storeGate ) {
    msg << MSG::ERROR
	<< "NULL pointer to StoreGateSvc !!" 
	<< endreq
	<< "Can't create *ANY* TruthParticleContainer !!" 
	<< endreq;
    throw std::runtime_error("NULL pointer to StoreGateSvc !!");
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

  // now extract the etIsolations from the persistent representation of
  // the TruthParticleContainer and put them into a TruthEtIsolations container
  TruthEtIsolationsContainer * etIsols = new TruthEtIsolationsContainer;
  const std::string etIsolName = "TruthEtIsol_" + trans->genEventName();
  if ( !m_storeGate->record( etIsols, etIsolName ).isSuccess() ) {
    msg << MSG::ERROR
	<< "Could not record a TruthEtIsolationsContainer at ["
	<< etIsolName << "] !!"
	<< endreq;
    delete etIsols; etIsols = 0;
    throw std::runtime_error("No TruthEtIsolationsContainer could be stored !!");
  }
  if ( !m_storeGate->setConst( etIsols ).isSuccess() ) {
    msg << MSG::WARNING
	<< "Could not setConst the TruthEtIsolationsContainer at ["
	<< etIsolName << "] !!"
	<< endreq;
  }

  const unsigned int genEventIndex = pers->m_genEvent.m_elementIndex;

  TruthEtIsolations * etIsol = new TruthEtIsolations( trans->genEventName(),
						      genEventIndex );
  etIsols->push_back( etIsol );

  // create a visitor which takes the persistent representation of the 
  // TruthParticleContainer as a parameter.
  // This visitor will setup each TruthParticle with its EtIsol cone vector.
  const McEtIsolVisitor<TruthParticleContainer_p3> visitor( *pers,
							    etIsol,
							    msg );

  if ( !m_cnvTool->convert( evt, 
			    genEventIndex,
			    trans,
			    &visitor ).isSuccess() ) {
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

  // connect the TruthParticleContainer to the container of TruthEtIsolations
  trans->setEtIsolations( etIsols, genEventIndex );

  msg << MSG::DEBUG 
      << "Loaded TruthParticleContainer from persistent state [OK]"
      << endreq;
  return;
}

void 
TruthParticleContainerCnv_p3::transToPers( const TruthParticleContainer* /*trans*/, 
					   TruthParticleContainer_p3* /*pers*/, 
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

  throw std::runtime_error( "Retired TruthParticleContainerCnv_p3::transToPers() !!" );

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

