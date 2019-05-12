///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// McAodSymLinkTests.cxx 
// Implementation file for class McAodSymLinkTests
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 


// STL includes
#include <limits>

// FrameWork includes
#include "GaudiKernel/Property.h"

// StoreGate
#include "StoreGate/StoreGateSvc.h"

// NavFourMom includes
#include "NavFourMom/INavigable4MomentumCollection.h"
#include "NavFourMom/IParticleContainer.h"

// McParticleEvent includes
#include "McParticleEvent/TruthParticleContainer.h"

// McParticleTests includes
#include "McAodSymLinkTests.h"

/////////////////////////////////////////////////////////////////// 
// Public methods: 
/////////////////////////////////////////////////////////////////// 

// Constructors
////////////////
McAodSymLinkTests::McAodSymLinkTests( const std::string& name, 
				      ISvcLocator* pSvcLocator ) : 
  AthAlgorithm( name, pSvcLocator ),
  m_storeGate  ( "StoreGateSvc", name ),
  m_msg        ( msgSvc(),       name )
{
  //
  // Property declaration
  // 
  //declareProperty( "Property", m_nProperty );

  std::string descr;
  descr = "StoreGate location of the TruthParticleContainer we want to test";
  declareProperty( "TruthParticles",
		   m_truthParticlesName = "SpclMC",
		   descr );
}

// Destructor
///////////////
McAodSymLinkTests::~McAodSymLinkTests()
{ 
  m_msg << MSG::DEBUG << "Calling destructor" << endreq;
}

// Athena Algorithm's Hooks
////////////////////////////
StatusCode McAodSymLinkTests::initialize()
{
  // configure our MsgStream
  m_msg.setLevel( msgLevel() );

  m_msg << MSG::INFO 
	<< "Initializing " << name() << "..." 
	<< endreq;

  // Get pointer to StoreGateSvc and cache it :
  if ( !m_storeGate.retrieve().isSuccess() ) {
    m_msg << MSG::ERROR 	
	  << "Unable to retrieve pointer to StoreGateSvc"
	  << endreq;
    return StatusCode::FAILURE;
  }
  
  return StatusCode::SUCCESS;
}

StatusCode McAodSymLinkTests::finalize()
{
  m_msg << MSG::INFO 
	<< "Finalizing " << name() << "..." 
	<< endreq;

  return StatusCode::SUCCESS;
}

StatusCode McAodSymLinkTests::execute()
{  
  m_msg << MSG::DEBUG << "Executing " << name() << "..." 
	<< endreq;

  const TruthParticleContainer * mcParts = 0;
  if ( !m_storeGate->retrieve( mcParts, 
			       m_truthParticlesName.value() ).isSuccess() ||
       0 == mcParts ) {
    m_msg << MSG::ERROR
	  << "Could not retrieve a TruthParticleContainer at ["
	  << m_truthParticlesName.value()
	  << "] !!"
	  << endreq;
    return StatusCode::FAILURE;
  }

  const IParticleContainer* iparts = 0;
  if ( !m_storeGate->retrieve( iparts, 
			       m_truthParticlesName.value() ).isSuccess() ||
       0 == iparts ) {
    m_msg << MSG::ERROR
	  << "Could not retrieve an IParticleContainer at ["
	  << m_truthParticlesName.value()
	  << "] !!"
	  << endreq;
    return StatusCode::FAILURE;
  }
  
  const INavigable4MomentumCollection* inav = 0;
  if ( !m_storeGate->retrieve( inav, 
			       m_truthParticlesName.value() ).isSuccess() ||
       0 == inav ) {
    m_msg << MSG::ERROR
	  << "Could not retrieve an INavigable4MomentumCollection at ["
	  << m_truthParticlesName.value()
	  << "] !!"
	  << endreq;
    return StatusCode::FAILURE;
  }
  
  const std::size_t mcPartsSize = mcParts->size();
  const std::size_t ipartsSize  = iparts->size();
  const std::size_t inavSize    = inav->size();

  if ( !( mcPartsSize == ipartsSize  &&
	  mcPartsSize == inavSize    &&
	  ipartsSize  == inavSize    ) ) {
    m_msg << MSG::ERROR
	  << "Symlinked containers do not have the same size !!" << endreq
	  << " TruthParticleContainer :        " << mcPartsSize << endreq
	  << " IParticleContainer :            " << ipartsSize  << endreq
	  << " INavigable4MomentumCollection : " << inavSize    << endreq;
    return StatusCode::FAILURE;
  }

  bool allGood = true;
  static const double eps = std::numeric_limits<double>::epsilon();
  for ( std::size_t i = 0; i != mcPartsSize; ++i ) {
    const double tp_ene = (*mcParts)[i]->e();
    const double ip_ene = (*iparts)[i]->e();
    const double in_ene = (*inav)[i]->e();
    if ( ! ( (tp_ene - ip_ene) < eps &&
	     (tp_ene - in_ene) < eps &&
	     (ip_ene - in_ene) < eps ) ) {
      m_msg << MSG::ERROR
	    << "symlink FAILS at index [" << i << "]: " << endreq
	    << " TruthParticle::e(): " << tp_ene << endreq
	    << " IParticle::e():     " << ip_ene << endreq
	    << " INav4Mom::e():      " << in_ene << endreq
	    << " epsilon<double>:    " << eps << endreq;
      allGood = false;
    }
  }
  if (!allGood) {
    return StatusCode::FAILURE;
  }

  // this string is needed for the unit-test
  m_msg << MSG::INFO << "McAodSymLink tests OK" << endreq;

  return StatusCode::SUCCESS;
}

/////////////////////////////////////////////////////////////////// 
// Const methods: 
///////////////////////////////////////////////////////////////////

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

