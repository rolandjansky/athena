///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

// CreateData.cxx 
// Implementation file for class CreateData
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 


// STL includes

// FrameWork includes
#include "Gaudi/Property.h"
#include "GaudiKernel/SystemOfUnits.h"

// StoreGate
#include "StoreGate/StoreGateSvc.h"

// DataModel includes
#include "AthContainers/DataVector.h"
#include "AthLinks/ElementLinkVector.h"

// AthExThinning includes
#include "AthExThinning/AthExIParticles.h"
#include "AthExThinning/AthExParticles.h"
#include "AthExThinning/AthExDecay.h"
#include "AthExThinning/AthExElephantino.h"
#include "AthExThinning/CreateData.h"

/////////////////////////////////////////////////////////////////// 
// Public methods: 
/////////////////////////////////////////////////////////////////// 

using namespace AthExThinning;

// Constructors
////////////////
CreateData::CreateData( const std::string& name, 
			  ISvcLocator* pSvcLocator ) : 
  AthAlgorithm( name, pSvcLocator )
{
  //
  // Property declaration
  // 
  //declareProperty( "Property", m_nProperty );

  declareProperty( "ParticlesOutput", 
		   m_particlesOutputName = "Particles",
		   "Output location of the particles" );

  declareProperty( "NbrOfParticles",
		   m_nbrParticles = 10,
		   "Number of particles to create" );

  declareProperty( "DecayOutput",
		   m_decayOutputName = "TwoBodyDecay",
		   "Output location of a 2-body decay" );

  declareProperty( "ElephantinoOutput",
		   m_elephantinoOutputName = "PinkElephantino",
		   "Output location of an Elephantino with 4 legs" );

  declareProperty( "TestNames",
		   m_testNames,
		   "list of test-names to create data for "
		   "(default: test1,test2,test3)" );
  m_testNames = std::vector<std::string>(3);
  m_testNames[0] = "test1";
  m_testNames[1] = "test2";
  m_testNames[2] = "test3";
}

// Destructor
///////////////
CreateData::~CreateData()
{}

// Athena Algorithm's Hooks
////////////////////////////
StatusCode CreateData::initialize()
{
  ATH_MSG_INFO ("Initializing " << name() << "...");
  return StatusCode::SUCCESS;
}

StatusCode CreateData::finalize()
{
  ATH_MSG_INFO ("Finalizing " << name() << "...");
  return StatusCode::SUCCESS;
}

StatusCode CreateData::execute()
{  
  ATH_MSG_DEBUG ("Executing " << name() << "...");

  for ( std::vector<std::string>::const_iterator 
	  itr  = m_testNames.begin(),
	  iEnd = m_testNames.end();
	itr != iEnd;
	++itr ) {
    ATH_MSG_INFO("Generating data for [" << *itr << "]...");
    if ( !makeData(*itr).isSuccess() ) {
      ATH_MSG_ERROR ("Could not generate data for [" << *itr << "] !!");
      return StatusCode::FAILURE;
    }
  }

  return StatusCode::SUCCESS;
}

StatusCode CreateData::makeData( const std::string& test )
{
  const std::string testName = "_"+test;
  const std::string particlesOutputName = m_particlesOutputName.value()
                                        + testName;
  // create particles
  AthExParticles * particles = new AthExParticles;
  if ( !evtStore()->record(particles, particlesOutputName).isSuccess() ) {
    ATH_MSG_ERROR("Could not store particles at ["
		  << particlesOutputName << "]");
    delete particles;
    particles = 0;
    return StatusCode::RECOVERABLE;
  }
  if ( !evtStore()->setConst(particles).isSuccess() ) {
    ATH_MSG_WARNING("Could not setConst particles at ["
		    << particlesOutputName << "]");
  }

  for ( unsigned int i = 0; i != m_nbrParticles.value(); ++i ) {
    AthExParticle * p = new AthExParticle( (i+1) * 10. * Gaudi::Units::GeV,
					   (i+1) * 10. * Gaudi::Units::GeV,
					   (i+1) * 10. * Gaudi::Units::GeV,
					   (i+2) * 10. * Gaudi::Units::GeV );
    particles->push_back(p);
  }

  ATH_MSG_DEBUG ("particles: " << particles->size() 
		 << " [" << particles << "]");

  const std::string decayOutputName = m_decayOutputName.value() + testName;
  // create a dumb object from 2 particles
  AthExDecay * dcy = new AthExDecay;
  if ( !evtStore()->record(dcy, decayOutputName).isSuccess() ) {
    ATH_MSG_ERROR("Could not store Decay at [" << decayOutputName << "] !!");
    delete dcy;
    dcy = 0;
    return StatusCode::RECOVERABLE;
  }
  if ( !evtStore()->setConst(dcy).isSuccess() ) {
    ATH_MSG_WARNING("Could not setConst Decay at [" << decayOutputName << "]");
  }
  
  if ( particles->size() < 5 ) {
    ATH_MSG_WARNING("Not enough particles to make a Decay !!");
    return StatusCode::RECOVERABLE;
  }

  // we need to test all the possible states of ElementLinks
  //  -> created from an index
  const ElementLink<AthExParticles> p1( *particles, 0 );
  //  -> created from a pointer
  const ElementLink<AthExParticles> p2( (*particles)[4], *particles );

  //  -> created from an index
  const ElementLink<AthExParticles> l1( *particles, 1 );
  //  -> created from a pointer
  const ElementLink<AthExParticles> l2( (*particles)[9], *particles );

  dcy->setDecay( p1, p2, l1, l2 );

  const double igev = 1. / Gaudi::Units::GeV;
  ATH_MSG_INFO
    ("Created a Decay from :" << endmsg
     << " p1: px= " << dcy->p1()->px() * igev << endmsg
     << " p2: px= " << dcy->p2()->px() * igev << endmsg
     << " l1: px= " << dcy->l1()->px() * igev << endmsg
     << " l2: px= " << dcy->l2()->px() * igev
     );

  // create the elephantino
  const std::string elephantinoOutputName = m_elephantinoOutputName.value()
                                          + testName;
  AthExElephantino * eleph = new AthExElephantino;
  if ( !evtStore()->record(eleph, elephantinoOutputName).isSuccess() ) {
    ATH_MSG_ERROR("Could not store Elephantino at ["
		  << elephantinoOutputName << "] !!");
    delete eleph;
    eleph = 0;
    return StatusCode::RECOVERABLE;
  }
  if ( !evtStore()->setConst(eleph).isSuccess() ) {
    ATH_MSG_WARNING("Could not setConst Elephantino at ["
		    << elephantinoOutputName << "]");
  }

  {
    const AthExIParticles* iparticles = 0;
    if ( !evtStore()->symLink( particles, iparticles ).isSuccess() ) {
      ATH_MSG_WARNING("Could not symlink AthExParticles to AthExIParticles !!");
      return StatusCode::RECOVERABLE;
    }
  }

  const AthExIParticles* iparticles = 0;
  if ( !evtStore()->retrieve( iparticles, particlesOutputName).isSuccess() ||
       0 == iparticles ) {
    ATH_MSG_WARNING("Could not retrieve the (symlinked) AthExIParticles at ["
		    << particlesOutputName << "] !!");
    return StatusCode::RECOVERABLE;
  }

  if ( iparticles->size() < 9 ) {
    ATH_MSG_WARNING("Not enough particles to make an Elephantino !!");
    return StatusCode::RECOVERABLE;
  }

  ATH_MSG_INFO("  particles: " <<  particles->size()
	       << endmsg <<
	       " iparticles: " << iparticles->size());
    
  if ( iparticles->size() != particles->size() ) {
    ATH_MSG_ERROR("Error performing the symlink !" << endmsg
		  << " #iparticles: " << iparticles->size() << endmsg
		  << "  #particles: " <<  particles->size() << endmsg
		  << " @iparticles: " << iparticles << endmsg
		  << "  @particles: " <<  particles << endmsg
		  << "=== StoreGate content ===\n"
		  << evtStore()->dump());
    return StatusCode::FAILURE;
  }

  // we need to test all the possible states of ElementLinks
  //  -> created from an index
  //  -> created from a pointer
  const ElementLink<AthExIParticles> ip1(  *iparticles,      0 );
  const ElementLink<AthExIParticles> ip2( (*iparticles)[4], *iparticles );
  const ElementLink<AthExIParticles> ip3(  *iparticles,      7 );
  const ElementLink<AthExIParticles> ip4( (*iparticles)[8], *iparticles );

  eleph->setLegs( ip1, ip2, ip3, ip4 );

  const ElementLink<AthExIParticles> ie1(  *iparticles,      1 );
  const ElementLink<AthExIParticles> ie2( (*iparticles)[9], *iparticles );
  eleph->setEars( ie1, ie2 );

  ATH_MSG_INFO("Created an Elephantino from :" << endmsg
	       << " leg1: px= " << eleph->leg1()->px() * igev << endmsg
	       << " leg2: px= " << eleph->leg2()->px() * igev << endmsg
	       << " leg3: px= " << eleph->leg3()->px() * igev << endmsg
	       << " leg4: px= " << eleph->leg4()->px() * igev << endmsg
	       << " ear1: px= " << eleph->ear1()->px() * igev << endmsg
	       << " ear2: px= " << eleph->ear2()->px() * igev);

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

