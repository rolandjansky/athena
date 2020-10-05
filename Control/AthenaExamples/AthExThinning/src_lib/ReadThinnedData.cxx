///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

// ReadThinnedData.cxx 
// Implementation file for class ReadThinnedData
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 


// STL includes

// FrameWork includes
#include "GaudiKernel/MsgStream.h"
#include "Gaudi/Property.h"
#include "GaudiKernel/SystemOfUnits.h"

// StoreGate
#include "StoreGate/StoreGateSvc.h"

// AthExThinning includes
#include "AthExThinning/AthExParticles.h"
#include "AthExThinning/AthExDecay.h"
#include "AthExThinning/AthExElephantino.h"
#include "AthExThinning/ReadThinnedData.h"

/////////////////////////////////////////////////////////////////// 
// Public methods: 
/////////////////////////////////////////////////////////////////// 

using namespace AthExThinning;

// Constructors
////////////////
ReadThinnedData::ReadThinnedData( const std::string& name, 
			  ISvcLocator* pSvcLocator ) : 
  AthAlgorithm( name, pSvcLocator )
{
  //
  // Property declaration
  // 
  //declareProperty( "Property", m_nProperty );

  declareProperty( "Particles", 
		   m_particlesName = "Particles",
		   "Input location of particles" );

  declareProperty( "Decay", 
		   m_decayName = "TwoBodyDecay",
		   "Input location of Decay" );

  declareProperty( "Elephantino", 
		   m_elephantinoName = "PinkElephantino",
		   "Input location of Elephantino" );

  std::vector<std::string> testNames; testNames.reserve(3);
  testNames.push_back("test1");
  testNames.push_back("test2");
  testNames.push_back("test3");
  declareProperty( "TestNames",
		   m_testNames = testNames,
		   "List of tests to proceed with" );

}

// Destructor
///////////////
ReadThinnedData::~ReadThinnedData()
{ 
}

// Athena Algorithm's Hooks
////////////////////////////
StatusCode ReadThinnedData::initialize()
{
  ATH_MSG_INFO("Initializing " << name() << "...");
  return StatusCode::SUCCESS;
}

StatusCode ReadThinnedData::finalize()
{
  ATH_MSG_INFO("Finalizing " << name() << "...");
  return StatusCode::SUCCESS;
}

StatusCode ReadThinnedData::execute()
{  
  ATH_MSG_DEBUG("Executing " << name() << "...");

  for ( std::vector<std::string>::const_iterator 
	  itr  = m_testNames.begin(),
	  iEnd = m_testNames.end();
	itr != iEnd;
	++itr ) {
    ATH_MSG_INFO("Reading data for [" << *itr << "]...");
    if ( !checkTest(*itr).isSuccess() ) {
      ATH_MSG_ERROR("Could not read data for [" << *itr << "] !!");
      return StatusCode::FAILURE;
    }
  }

  return StatusCode::SUCCESS;
}

StatusCode ReadThinnedData::checkTest( const std::string& testName )
{
  const std::string& test = testName;

  // fetch Particles
  const std::string particlesName = m_particlesName.value()+"_"+test;
  const AthExParticles * particles = 0;
  if ( !evtStore()->retrieve(particles, particlesName).isSuccess() ||
       0 == particles ) {
    ATH_MSG_WARNING("Could not fetch particles at ["
		    << particlesName << "] !!");
    return StatusCode::RECOVERABLE;
  }

  // fetch IParticles
  const std::string iparticlesName = m_particlesName.value()+"_"+test;
  const AthExParticles * iparticles = 0;
  if ( !evtStore()->retrieve(iparticles, iparticlesName).isSuccess() ||
       0 == iparticles ) {
    ATH_MSG_WARNING
      ("Could not fetch iparticles at [" << iparticlesName << "] !!");
    return StatusCode::RECOVERABLE;
  }

  // fetch Decay
  const std::string decayName = m_decayName.value()+"_"+test;
  const AthExDecay * decay = 0;
  if ( !evtStore()->retrieve(decay, decayName).isSuccess() ||
       0 == decay ) {
    ATH_MSG_WARNING 
      ("Could not fetch Decay at [" << decayName << "] !!");
    return StatusCode::RECOVERABLE;
  }

  // fetch Elephantino
  const std::string elephantinoName = m_elephantinoName.value()+"_"+test;
  const AthExElephantino * elephantino = 0;
  if ( !evtStore()->retrieve(elephantino, elephantinoName).isSuccess() ||
       0 == elephantino ) {
    ATH_MSG_WARNING
      ("Could not fetch Elephantino at [" << elephantinoName << "] !!");
    return StatusCode::RECOVERABLE;
  }

  const double igev = 1. / Gaudi::Units::GeV;
  ATH_MSG_DEBUG 
    ("IN  particles: " <<  particles->size() << endmsg
     << "IN iparticles: " << iparticles->size() << endmsg
     << "IN decay: " << endmsg
     << " p1: px= " << decay->p1()->px() * igev << endmsg
     << " p2: px= " << decay->p2()->px() * igev);

  msg(MSG::DEBUG) << " l1: px= ";
  if ( decay->l1() ) { msg(MSG::DEBUG) << decay->l1()->px() * igev << endmsg;
  } else             { msg(MSG::DEBUG) << "[thinned!]"            << endmsg;
  }
  msg(MSG::DEBUG) << " l2: px= ";
  if ( decay->l2() ) { msg(MSG::DEBUG) << decay->l2()->px() * igev << endmsg;
  } else             { msg(MSG::DEBUG) << "[thinned!]"            << endmsg;
  }

  msg(MSG::DEBUG)
    << "IN elephantino: " << endmsg
    << " leg1: px= " << elephantino->leg1()->px() * igev << endmsg
    << " leg2: px= " << elephantino->leg2()->px() * igev << endmsg
    << " leg3: px= " << elephantino->leg3()->px() * igev << endmsg
    << " leg4: px= " << elephantino->leg4()->px() * igev << endmsg;

  msg(MSG::DEBUG) << " ear1: px= ";
  if ( elephantino->ear1() ) { 
    msg(MSG::DEBUG) << elephantino->ear1()->px() * igev << endmsg;
  } else { 
    msg(MSG::DEBUG) << "[thinned!]" << endmsg;
  }
  msg(MSG::DEBUG) << " ear2: px= ";
  if ( elephantino->ear2() ) { 
    msg(MSG::DEBUG) << elephantino->ear2()->px() * igev << endmsg;
  } else { 
    msg(MSG::DEBUG) << "[thinned!]" << endmsg;
  }

  msg(MSG::INFO) << "Particles | IParticles : " << endmsg;
  for ( unsigned int i = 0; i != particles->size(); ++i ) {
    msg(MSG::INFO)
      << std::setw(9) << (* particles)[i]->px() * igev
      << " | "
      << std::setw(9) << (*iparticles)[i]->px() * igev
      << endmsg;
  }
  msg(MSG::INFO) << "======================" << endmsg;

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

