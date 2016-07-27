///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// TruthParticleContainerCnv_p1.cxx 
// Implementation file for class TruthParticleContainerCnv_p1
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

// McParticleEventTPCnv includes
#include "McParticleEventTPCnv/TruthParticleContainerCnv_p1.h"

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
TruthParticleContainerCnv_p1::persToTrans( const TruthParticleContainer_p1* pers, 
					   TruthParticleContainer* trans, 
					   MsgStream &log ) 
{
  log << MSG::DEBUG 
      << "Loading TruthParticleContainer from persistent state..."
      << endmsg;

  if ( 0 == m_cnvTool ) {
    log << MSG::ERROR
	<< "NULL pointer to ITruthParticleCnvTool !!" 
	<< endmsg
	<< "Can't create *ANY* TruthParticleContainer !!" 
	<< endmsg;
    throw std::runtime_error("NULL pointer to ITruthParticleCnvTool !!");
  }

  if ( 0 == m_storeGate ) {
    log << MSG::ERROR
	<< "NULL pointer to StoreGateSvc !!" 
	<< endmsg
	<< "Can't create *ANY* TruthParticleContainer !!" 
	<< endmsg;
    throw std::runtime_error("NULL pointer to StoreGateSvc !!");
  }

  const McEventCollection* mcEvt = 0;
  if ( m_storeGate->retrieve(mcEvt, pers->m_parentKey).isFailure() ||
       0 == mcEvt ) {
    log << MSG::ERROR
	<< "Could not retrieve McEventCollection at ["
	<< pers->m_parentKey
	<< "] !!"
	<< endmsg;
    throw std::runtime_error("No McEventCollection retrieved !!");
  }
    
  // at that time, the default was to write the first element
  // of the McEventCollection (now we have more flexibility)
  const unsigned int genEventIndex = 0;
  if ( !m_cnvTool->convert( mcEvt, 
			    genEventIndex,
			    trans ).isSuccess() ) {
    log << MSG::ERROR
	<< "Problem converting McEventCollection to TruthParticleContainer !"
	<< endmsg;
    throw std::runtime_error("No TruthParticleContainer created !!");
  }

  log << MSG::DEBUG 
      << "Loaded TruthParticleContainer from persistent state [OK]"
      << endmsg;
  return;
}

void 
TruthParticleContainerCnv_p1::transToPers( const TruthParticleContainer* /*trans*/, 
					   TruthParticleContainer_p1* /*pers*/, 
					   MsgStream &log ) 
{
  log << MSG::DEBUG 
      << "Creating persistent state of TruthParticleContainer..."
      << endmsg;

  log << MSG::ERROR
      << "This transient-to-persistent converter method has been RETIRED !!"
      << endmsg
      << "You are not supposed to end-up here ! Go away !"
      << endmsg;

  throw std::runtime_error( "Retired TruthParticleContainerCnv_p1::transToPers() !!" );
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

