///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// WriteThinnedData.cxx 
// Implementation file for class WriteThinnedData
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 


// STL includes
#include <vector>
#include <sstream>

// FrameWork includes
#include "GaudiKernel/Property.h"
#include "GaudiKernel/Incident.h"
#include "GaudiKernel/IIncidentSvc.h"

// CLHEP
#include "CLHEP/Units/SystemOfUnits.h"

// StoreGate
#include "StoreGate/StoreGateSvc.h"

// AthenaKernel includes
#include "AthenaKernel/IThinningSvc.h"

// DataModel includes
#include "AthContainers/DataVector.h"

// AthExThinning includes
#include "AthExThinning/AthExParticles.h"
#include "AthExThinning/AthExDecay.h"
#include "AthExThinning/AthExElephantino.h"
#include "AthExThinning/WriteThinnedData.h"

/////////////////////////////////////////////////////////////////// 
// Public methods: 
/////////////////////////////////////////////////////////////////// 

using namespace AthExThinning;

// Constructors
////////////////
WriteThinnedData::WriteThinnedData( const std::string& name, 
				    ISvcLocator* pSvcLocator ) : 
  ::AthAlgorithm( name, pSvcLocator ),
  m_thinningSvc( "ThinningSvc",  name )
{
  //
  // Property declaration
  // 
  //declareProperty( "Property", m_nProperty );

  declareProperty( "Particles", 
                   m_particlesName = "Particles",
                   "Input location of particles (to be thinned)" );

  declareProperty( "Decay", 
                   m_decayName = "TwoBodyDecay",
                   "Input location of Decay" );

  declareProperty( "Elephantino", 
                   m_elephantinoName = "PinkElephantino",
                   "Input location of Elephantino" );

  declareProperty( "Filter", 
                   m_filter, 
                   "Filter to apply on Particles (true == keep element)" );
  std::vector<bool> filter( 10, false );
  m_filter.set( filter );

}

// Destructor
///////////////
WriteThinnedData::~WriteThinnedData()
{}

// Athena Algorithm's Hooks
////////////////////////////
StatusCode WriteThinnedData::initialize()
{
  ATH_MSG_INFO("Initializing " << name() << "...");

  // Get pointer to ThinningSvc and cache it :
  // m_thinningSvc is of type IThinningSvc
  if ( !m_thinningSvc.retrieve().isSuccess() ) {
    ATH_MSG_ERROR("Unable to retrieve pointer to ThinningSvc");
    return StatusCode::FAILURE;
  }
  
  return StatusCode::SUCCESS;
}

StatusCode WriteThinnedData::finalize()
{
  ATH_MSG_INFO("Finalizing " << name() << "...");
  return StatusCode::SUCCESS;
}

StatusCode WriteThinnedData::execute()
{  
  ATH_MSG_DEBUG("Executing " << name() << "...");

  bool allGood = true;
  if ( !test( "test1" ).isSuccess() ) {
    ATH_MSG_WARNING("Could not perform 'thinning test1' !!");
    allGood = false;
  }

  if ( !test( "test2" ).isSuccess() ) {
    ATH_MSG_WARNING("Could not perform 'thinning test2' !!");
    allGood = false;
  }

  if ( !test( "test3" ).isSuccess() ) {
    ATH_MSG_WARNING("Could not perform 'thinning test3' !!");
    allGood = false;
  }

  return allGood ? StatusCode::SUCCESS : StatusCode::FAILURE;
}

StatusCode WriteThinnedData::test( const std::string& testName )
{
  const std::string& test = testName;
  ATH_MSG_INFO("Performing thinning test [" << test << "]...");

  // fetch Particles
  const std::string particlesName = m_particlesName.value() + "_" + test;
  const AthExParticles * particles = 0;
  if ( !evtStore()->retrieve(particles, particlesName).isSuccess() ||
       0 == particles ) {
    ATH_MSG_WARNING
      ("Could not fetch particles at [" << particlesName << "] !!");
    return StatusCode::RECOVERABLE;
  }

  // fetch IParticles
  const std::string iparticlesName = m_particlesName.value() + "_" + test;
  const AthExIParticles * iparticles = 0;
  if ( !evtStore()->retrieve(iparticles, particlesName).isSuccess() ||
       0 == iparticles ) {
    ATH_MSG_WARNING
      ("Could not fetch iparticles at [" << particlesName << "] !!");
    return StatusCode::RECOVERABLE;
  }

  if ( iparticles->size()      != particles->size()     ||
       iparticles->at(0)->px() != particles->at(0)->px() ) {
    ATH_MSG_WARNING
      ("symlinked containers are corrupted: " << endmsg
       << " #iparticles: " << iparticles->size() << endmsg
       << " # particles: " <<  particles->size() << endmsg
       << "  ipx[0] = " << iparticles->at(0)->px() << endmsg
       << "   px[0] = " <<  particles->at(0)->px());
    return StatusCode::RECOVERABLE;
  }

  // fetch Decay
  const std::string decayName = m_decayName.value()+"_"+test;
  const AthExDecay * decay = 0;
  if ( !evtStore()->retrieve(decay, decayName).isSuccess() ||
       0 == decay ) {
    ATH_MSG_WARNING("Could not fetch Decay at [" << decayName << "] !!");
    return StatusCode::RECOVERABLE;
  }

  // fetch Elephantino
  const std::string elephantinoName = m_elephantinoName.value() + "_" + test;
  const AthExElephantino * elephantino = 0;
  if ( !evtStore()->retrieve(elephantino, elephantinoName).isSuccess() ||
       0 == elephantino ) {
    ATH_MSG_WARNING
      ("Could not fetch Elephantino at [" << elephantinoName << "] !!");
    return StatusCode::RECOVERABLE;
  }

  const double igev = 1. / CLHEP::GeV;
  ATH_MSG_DEBUG("IN particles: " << particles->size() << endmsg
		<< "IN decay: " << endmsg
		<< " p1: px= " << decay->p1()->px() * igev << endmsg
		<< " p2: px= " << decay->p2()->px() * igev << endmsg
		<< " l1: px= " << decay->l1()->px() * igev << endmsg
		<< " l2: px= " << decay->l2()->px() * igev);

  ATH_MSG_DEBUG("IN elephantino: " << endmsg
		<< " leg1: px= " << elephantino->leg1()->px() * igev << endmsg
		<< " leg2: px= " << elephantino->leg2()->px() * igev << endmsg
		<< " leg3: px= " << elephantino->leg3()->px() * igev << endmsg
		<< " leg4: px= " << elephantino->leg4()->px() * igev << endmsg
		<< " ear1: px= " << elephantino->ear1()->px() * igev << endmsg
		<< " ear2: px= " << elephantino->ear2()->px() * igev);

  ////////////////////////////////////////////////////////////////////
  /// thinning 
  if ( test == "test1" ) {
    if ( !doThinningTest1(*particles).isSuccess() ) {
      ATH_MSG_WARNING("Could not exercize Thinning !!");
    }
  } else if ( test == "test2" ) {
    if ( !doThinningTest2(*particles).isSuccess() ) {
      ATH_MSG_WARNING("Could not exercize Thinning !!");
    }
  } else if ( test == "test3" ) {
    if ( !doThinningTest3(*iparticles).isSuccess() ) {
      ATH_MSG_WARNING("Could not exercize Thinning !!");
    }
  } else {
    ATH_MSG_ERROR("Unknown test: [" << test << "]");
    return StatusCode::FAILURE;
  }
  ////////////////////////////////////////////////////////////////////

  ATH_MSG_INFO
    ("Decay is now: " << endmsg
     << " p1: px= " << decay->p1()->px() * igev << endmsg
     << " p2: px= " << decay->p2()->px() * igev << endmsg
     << " l1: px= " << decay->l1()->px() * igev << endmsg
     << " l2: px= " << decay->l2()->px() * igev);
    
  ATH_MSG_INFO
    ("Elephantino is now: " << endmsg
     << " leg1: px= " << elephantino->leg1()->px() * igev << endmsg
     << " leg2: px= " << elephantino->leg2()->px() * igev << endmsg
     << " leg3: px= " << elephantino->leg3()->px() * igev << endmsg
     << " leg4: px= " << elephantino->leg4()->px() * igev << endmsg
     << " ear1: px= " << elephantino->ear1()->px() * igev << endmsg
     << " ear2: px= " << elephantino->ear2()->px() * igev);
   
  // test IThinning::thinningOccurred api
  if (!m_thinningSvc->thinningOccurred()) {
    ATH_MSG_ERROR("IThinningSvc::thinningOccurred tells lies !");
    return StatusCode::FAILURE;
  }

  if (!m_thinningSvc->thinningOccurred(particles)) {
    ATH_MSG_ERROR("IThinningSvc::thinningOccurred(particles) tells lies !");
    return StatusCode::FAILURE;
  }

  if (!m_thinningSvc->thinningOccurred(iparticles)) {
    ATH_MSG_ERROR("IThinningSvc::thinningOccurred(iparticles) tells lies !");
    return StatusCode::FAILURE;
  }

  {
    AthExParticles * pristine_particles = new AthExParticles;
    const std::string key = "NonThinned-" + particlesName;
    ATH_CHECK(evtStore()->record(pristine_particles, key));

    if (m_thinningSvc->thinningOccurred(pristine_particles)) {
      ATH_MSG_ERROR("IThinningSvc::thinningOccurred(non-thinned) tells lies !");
      return StatusCode::FAILURE;
    }
  }

  ATH_MSG_INFO("[" << test << "] has been performed.");
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

StatusCode WriteThinnedData::doThinningTest1( const AthExParticles& particles )
{
  const AthExParticles * cont = &particles;
  std::vector<bool> filter = m_filter.value();
  
  const double igev = 1. / CLHEP::GeV;
  msg(MSG::INFO) << "Particles | filter :" << endmsg;
  for ( unsigned int i = 0; i != particles.size(); ++i ) {
    const std::string dec = filter[i] ? "keep" : "remove";
    msg(MSG::INFO)
      << std::setw(9) << particles[i]->px() * igev
      << " | " << dec
      << endmsg;
  }
  msg(MSG::INFO) << "===================" << endmsg;

  std::fill( filter.begin() + (filter.size() / 2), 
	     filter.end(),
	     true );
  msg(MSG::INFO) << "Filter [" << std::boolalpha;
  std::copy( filter.begin(), filter.end(),
	     std::ostream_iterator<bool>(msg(MSG::INFO).stream(), " ") );
  msg(MSG::INFO) << "]" << endmsg;

  msg(MSG::INFO) << "... Processing [pre-thinning] ..." << endmsg;
  if ( !m_thinningSvc->filter( particles, filter ).isSuccess() ) {
    msg(MSG::WARNING)
      << "Could not filter particles !!"
      << endmsg;
  }
  msg(MSG::INFO) << "======== Index table =========" << endmsg;
  for ( std::size_t i = 0; i != particles.size(); ++i ) {
    std::size_t newIdx = m_thinningSvc->index( cont, i );
    std::stringstream newIdxStr; 
    newIdxStr << newIdx;
    msg(MSG::INFO)
      << " idx " << i 
      << " -> "  << (newIdx == IThinningSvc::RemovedIdx 
		     ? "-"
		     : newIdxStr.str() )
      << endmsg;
  }


  filter = m_filter.value();
  std::fill( filter.begin(),
	     filter.begin() + (filter.size() / 2), 
	     true );

  msg(MSG::INFO) << "Filter [" << std::boolalpha;
  std::copy( filter.begin(), filter.end(),
	     std::ostream_iterator<bool>(msg(MSG::INFO).stream(), " ") );
  msg(MSG::INFO) << "]" << endmsg;

  msg(MSG::INFO) << "... Processing [thinning] ..." << endmsg;
  if ( m_thinningSvc->filter( particles, filter ).isFailure() ) {
    msg(MSG::WARNING)
      << "Could not punch holes !!"
      << endmsg;
  }

  msg(MSG::INFO) << "======== Index table =========" << endmsg;
  for ( std::size_t i = 0; i != particles.size(); ++i ) {
    std::size_t newIdx = m_thinningSvc->index( cont, i );
    std::stringstream newIdxStr; 
    newIdxStr << newIdx;
    msg(MSG::INFO)
      << " idx " << i 
      << " -> "  << (newIdx == IThinningSvc::RemovedIdx 
		     ? "-"
		     : newIdxStr.str() )
      << endmsg;
  }
  
  return StatusCode::SUCCESS;
}

StatusCode WriteThinnedData::doThinningTest2( const AthExParticles& particles )
{
  const AthExParticles * cont = &particles;
  std::vector<bool> filter = m_filter.value();

  const double igev = 1. / CLHEP::GeV;
  msg(MSG::INFO) << "Particles | filter :" << endmsg;
  for ( unsigned int i = 0; i != particles.size(); ++i ) {
    const std::string dec = filter[i] ? "keep" : "remove";
    msg(MSG::INFO)
      << std::setw(9) << particles[i]->px() * igev
      << " | " << dec
      << endmsg;
  }
  msg(MSG::INFO) << "===================" << endmsg;

  std::fill( filter.begin() + (filter.size() / 2), 
	     filter.end(),
	     false );
  msg(MSG::INFO) << "Filter [" << std::boolalpha;
  std::copy( filter.begin(), filter.end(),
	     std::ostream_iterator<bool>(msg(MSG::INFO).stream(), " ") );
  msg(MSG::INFO) << "]" << endmsg;

  msg(MSG::INFO) << "... Processing [pre-thinning] ..." << endmsg;
  if ( !m_thinningSvc->filter( particles, filter,
			       IThinningSvc::Operator::Or ).isSuccess() ) {
    msg(MSG::WARNING)
      << "Could not filter particles !!"
      << endmsg;
  }
  msg(MSG::INFO) << "======== Index table =========" << endmsg;
  for ( std::size_t i = 0; i != particles.size(); ++i ) {
    std::size_t newIdx = m_thinningSvc->index( cont, i );
    std::stringstream newIdxStr; 
    newIdxStr << newIdx;
    msg(MSG::INFO)
      << " idx " << i 
      << " -> "  << (newIdx == IThinningSvc::RemovedIdx 
		     ? "-"
		     : newIdxStr.str() )
      << endmsg;
  }


  filter = m_filter.value();
  std::fill( filter.begin(),
	     filter.begin() + (filter.size() / 2), 
	     false );

  msg(MSG::INFO) << "Filter [" << std::boolalpha;
  std::copy( filter.begin(), filter.end(),
	     std::ostream_iterator<bool>(msg(MSG::INFO).stream(), " ") );
  msg(MSG::INFO) << "]" << endmsg;

  msg(MSG::INFO) << "... Processing [thinning] ..." << endmsg;
  if ( m_thinningSvc->filter( particles, filter,
			      IThinningSvc::Operator::Or ).isFailure() ) {
    msg(MSG::WARNING)
      << "Could not punch holes !!"
      << endmsg;
  }

  msg(MSG::INFO) << "======== Index table =========" << endmsg;
  for ( std::size_t i = 0; i != particles.size(); ++i ) {
    std::size_t newIdx = m_thinningSvc->index( cont, i );
    std::stringstream newIdxStr; 
    newIdxStr << newIdx;
    msg(MSG::INFO) 
      << " idx " << i 
      << " -> "  << (newIdx == IThinningSvc::RemovedIdx 
		     ? "-"
		     : newIdxStr.str() )
      << endmsg;
  }
  
  return StatusCode::SUCCESS;
}

StatusCode 
WriteThinnedData::doThinningTest3( const AthExIParticles& iparticles )
{
  const AthExIParticles * cont = &iparticles;
  std::vector<bool> filter = m_filter.value();

  const double igev = 1. / CLHEP::GeV;
  msg(MSG::INFO) << "IParticles | filter :" << endmsg;
  for ( unsigned int i = 0; i != iparticles.size(); ++i ) {
    const std::string dec = filter[i] ? "keep" : "remove";
    msg(MSG::INFO)
      << std::setw(9) << iparticles[i]->px() * igev
      << " | " << dec
      << endmsg;
  }
  msg(MSG::INFO) << "===================" << endmsg;

  std::fill( filter.begin() + (filter.size() / 2), 
	     filter.end(),
	     true );
  msg(MSG::INFO) << "Filter [" << std::boolalpha;
  std::copy( filter.begin(), filter.end(),
	     std::ostream_iterator<bool>(msg(MSG::INFO).stream(), " ") );
  msg(MSG::INFO) << "]" << endmsg;

  msg(MSG::INFO) << "... Processing [pre-thinning] ..." << endmsg;
  if ( !m_thinningSvc->filter( iparticles, filter ).isSuccess() ) {
    msg(MSG::WARNING)
      << "Could not filter iparticles !!"
      << endmsg;
  }
  msg(MSG::INFO) << "======== Index table =========" << endmsg;
  for ( std::size_t i = 0; i != iparticles.size(); ++i ) {
    std::size_t newIdx = m_thinningSvc->index( cont, i );
    std::stringstream newIdxStr; 
    newIdxStr << newIdx;
    msg(MSG::INFO)
      << " idx " << i 
      << " -> "  << (newIdx == IThinningSvc::RemovedIdx 
		     ? "-"
		     : newIdxStr.str() )
      << endmsg;
  }


  filter = m_filter.value();
  std::fill( filter.begin(),
	     filter.begin() + (filter.size() / 2), 
	     true );

  msg(MSG::INFO) << "Filter [" << std::boolalpha;
  std::copy( filter.begin(), filter.end(),
	     std::ostream_iterator<bool>(msg(MSG::INFO).stream(), " ") );
  msg(MSG::INFO) << "]" << endmsg;

  msg(MSG::INFO) << "... Processing [thinning] ..." << endmsg;
  if ( m_thinningSvc->filter( iparticles, filter ).isFailure() ) {
    msg(MSG::WARNING)
      << "Could not punch holes !!"
      << endmsg;
  }

  msg(MSG::INFO) << "======== Index table =========" << endmsg;
  for ( std::size_t i = 0; i != iparticles.size(); ++i ) {
    std::size_t newIdx = m_thinningSvc->index( cont, i );
    std::stringstream newIdxStr; 
    newIdxStr << newIdx;
    msg(MSG::INFO)
      << " idx " << i 
      << " -> "  << (newIdx == IThinningSvc::RemovedIdx 
		     ? "-"
		     : newIdxStr.str() )
      << endmsg;
  }
  
  return StatusCode::SUCCESS;
}
