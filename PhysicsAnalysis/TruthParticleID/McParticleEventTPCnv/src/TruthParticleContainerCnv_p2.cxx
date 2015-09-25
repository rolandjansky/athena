///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// TruthParticleContainerCnv_p2.cxx 
// Implementation file for class TruthParticleContainerCnv_p2
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
#include "McParticleEvent/TruthParticleParamDefs.h"

// McParticleEventTPCnv includes
#include "McParticleEventTPCnv/TruthParticleContainerCnv_p2.h"
#include "McEtIsolVisitor.h"

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
TruthParticleContainerCnv_p2::persToTrans( const TruthParticleContainer_p2* pers, 
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

  const McEventCollection* mcEvt = 0;
  if ( m_storeGate->retrieve(mcEvt, pers->m_parentKey).isFailure() ||
       0 == mcEvt ) {
    msg << MSG::ERROR
	<< "Could not retrieve McEventCollection at ["
	<< pers->m_parentKey
	<< "] !!"
	<< endreq;
    throw std::runtime_error("No McEventCollection retrieved !!");
  }
  
  // now extract the etIsolations from the persistent representation of
  // the TruthParticleContainer and put them into a TruthEtIsolations container
  TruthEtIsolationsContainer * etIsols = new TruthEtIsolationsContainer;
  const std::string etIsolName = "TruthEtIsol_" + pers->m_parentKey;
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

  // at that time, the default was to write the first element
  // of the McEventCollection (now we have more flexibility)
  const unsigned int genEventIndex = 0;

  TruthEtIsolations * etIsol = new TruthEtIsolations( pers->m_parentKey,
						      genEventIndex );
  etIsols->push_back( etIsol );

  // Create a visitor to fill the TruthEtIsolations container as we walk
  // through the (huge) loop over TruthParticles
  McEtIsolVisitor<TruthParticleContainer_p2> visitor( *pers,
						      etIsol,
						      msg );

  if ( !m_cnvTool->convert( mcEvt, 
			    genEventIndex,
			    trans,
			    &visitor ).isSuccess() ) {
    msg << MSG::ERROR
	<< "Problem converting McEventCollection to TruthParticleContainer !"
	<< endreq;
    throw std::runtime_error("No TruthParticleContainer created !!");
  }

  // connect the TruthParticleContainer to the container of TruthEtIsolations
  trans->setEtIsolations( etIsols, genEventIndex );

  msg << MSG::DEBUG 
      << "Loaded TruthParticleContainer from persistent state [OK]"
      << endreq;
  return;
}

void 
TruthParticleContainerCnv_p2::transToPers( const TruthParticleContainer* /*trans*/, 
					   TruthParticleContainer_p2* /*pers*/, 
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

  throw std::runtime_error( "Retired TruthParticleContainerCnv_p2::transToPers() !!" );
  // not reached
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

