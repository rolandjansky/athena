///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

// WriteThinnedData.cxx 
// Implementation file for class WriteThinnedData
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 


// STL includes
#include <vector>
#include <sstream>

// FrameWork includes
#include "Gaudi/Property.h"
#include "GaudiKernel/SystemOfUnits.h"

// StoreGate
#include "StoreGate/StoreGateSvc.h"
#include "StoreGate/ReadHandle.h"
#include "StoreGate/ThinningHandle.h"

// DataModel includes
#include "AthContainers/DataVector.h"
#include "AthContainers/ThinningDecision.h"

// AthExThinning includes
#include "AthExThinning/AthExParticles.h"
#include "AthExThinning/AthExDecay.h"
#include "AthExThinning/AthExElephantino.h"
#include "AthExThinning/WriteThinnedData.h"


using namespace AthExThinning;

// Constructors
////////////////
WriteThinnedData::WriteThinnedData( const std::string& name, 
				    ISvcLocator* pSvcLocator ) : 
  ::AthAlgorithm( name, pSvcLocator )
{
  //
  // Property declaration
  // 
  //declareProperty( "Property", m_nProperty );

  declareProperty( "Particles", 
                   m_particlesName = "Particles",
                   "Input location of particles (to be thinned)" );

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

  for (int i = 0; i < 3; i++) {
    std::ostringstream ss;
    ss << "_test" << (i+1);
    m_elephantinoKeys.emplace_back (m_elephantinoName.value() + ss.str());
    m_decayKeys.emplace_back (m_decayName.value() + ss.str());
    m_iparticlesKeys.emplace_back (m_particlesName.value() + ss.str());
  }
  ATH_CHECK( m_elephantinoKeys.initialize() );
  ATH_CHECK( m_decayKeys.initialize() );
  ATH_CHECK( m_iparticlesKeys.initialize() );

  m_particlesKey1 = m_particlesName.value() + "_test1";
  m_particlesKey2 = m_particlesName.value() + "_test2";
  ATH_CHECK( m_particlesKey1.initialize ("StreamUSR_0") );
  ATH_CHECK( m_particlesKey2.initialize ("StreamUSR_0") );

  m_iparticlesKey3 = m_particlesName.value() + "_test3";
  ATH_CHECK( m_iparticlesKey3.initialize ("StreamUSR_0") );

  return StatusCode::SUCCESS;
}

StatusCode WriteThinnedData::finalize()
{
  ATH_MSG_INFO("Finalizing " << name() << "...");
  return StatusCode::SUCCESS;
}

StatusCode WriteThinnedData::execute()
{
  const EventContext& ctx = Gaudi::Hive::currentContext();
  ATH_MSG_DEBUG("Executing " << name() << "...");

  bool allGood = true;
  if ( !test( ctx, 0, "test1" ).isSuccess() ) {
    ATH_MSG_WARNING("Could not perform 'thinning test1' !!");
    allGood = false;
  }

  if ( !test( ctx, 1, "test2" ).isSuccess() ) {
    ATH_MSG_WARNING("Could not perform 'thinning test2' !!");
    allGood = false;
  }

  if ( !test( ctx, 2, "test3" ).isSuccess() ) {
    ATH_MSG_WARNING("Could not perform 'thinning test3' !!");
    allGood = false;
  }

  return allGood ? StatusCode::SUCCESS : StatusCode::FAILURE;
}

StatusCode WriteThinnedData::test( const EventContext& ctx,
                                   int testNum, const std::string& testName )
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
  SG::ReadHandle<AthExIParticles> iparticles (m_iparticlesKeys[testNum], ctx);

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
  SG::ReadHandle<AthExDecay> decay (m_decayKeys[testNum], ctx);

  // fetch Elephantino
  SG::ReadHandle<AthExElephantino> elephantino (m_elephantinoKeys[testNum], ctx);

  const double igev = 1. / Gaudi::Units::GeV;
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
    if ( !doThinningTest1(ctx, m_particlesKey1).isSuccess() ) {
      ATH_MSG_WARNING("Could not exercise Thinning !!");
    }
  } else if ( test == "test2" ) {
    if ( !doThinningTest2(ctx, m_particlesKey2).isSuccess() ) {
      ATH_MSG_WARNING("Could not exercise Thinning !!");
    }
  } else if ( test == "test3" ) {
    if ( !doThinningTest3(ctx, m_iparticlesKey3).isSuccess() ) {
      ATH_MSG_WARNING("Could not exercise Thinning !!");
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


  ATH_MSG_INFO("[" << test << "] has been performed.");
  return StatusCode::SUCCESS;
}


StatusCode WriteThinnedData::doThinningTest1( const EventContext& ctx,
                                              const SG::ThinningHandleKey<AthExParticles>& particlesKey ) const
{
  SG::ThinningHandle<AthExParticles> particles (particlesKey, ctx);
  std::vector<bool> filter = m_filter.value();
  
  const double igev = 1. / Gaudi::Units::GeV;
  msg(MSG::INFO) << "Particles | filter :" << endmsg;
  for ( unsigned int i = 0; i != particles->size(); ++i ) {
    const std::string kr = filter[i] ? "keep" : "remove";
    msg(MSG::INFO)
      << std::setw(9) << (*particles)[i]->px() * igev
      << " | " << kr
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
  particles.keep (filter);
  msg(MSG::INFO) << "======== Index table =========" << endmsg;
  {
    SG::ThinningDecisionBase tmp = particles.decision();
    tmp.buildIndexMap();
    for ( std::size_t i = 0; i != particles->size(); ++i ) {
      std::size_t newIdx = tmp.index( i );
      std::stringstream newIdxStr; 
      newIdxStr << newIdx;
      msg(MSG::INFO)
        << " idx " << i 
        << " -> "  << (newIdx == SG::ThinningDecision::RemovedIdx 
                       ? "-"
                       : newIdxStr.str() )
        << endmsg;
    }
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
  particles.keep( filter, SG::ThinningDecisionBase::Op::And );

  msg(MSG::INFO) << "======== Index table =========" << endmsg;
  {
    SG::ThinningDecisionBase tmp = particles.decision();
    tmp.buildIndexMap();
    for ( std::size_t i = 0; i != particles->size(); ++i ) {
      std::size_t newIdx = tmp.index( i );
      std::stringstream newIdxStr; 
      newIdxStr << newIdx;
      msg(MSG::INFO)
        << " idx " << i 
        << " -> "  << (newIdx == SG::ThinningDecision::RemovedIdx 
                       ? "-"
                       : newIdxStr.str() )
        << endmsg;
    }
  }
  
  return StatusCode::SUCCESS;
}


StatusCode WriteThinnedData::doThinningTest2( const EventContext& ctx,
                                              const SG::ThinningHandleKey<AthExParticles>& particlesKey ) const
{
  SG::ThinningHandle<AthExParticles> particles (particlesKey, ctx);
  std::vector<bool> filter = m_filter.value();

  const double igev = 1. / Gaudi::Units::GeV;
  msg(MSG::INFO) << "Particles | filter :" << endmsg;
  for ( unsigned int i = 0; i != particles->size(); ++i ) {
    const std::string kr = filter[i] ? "keep" : "remove";
    msg(MSG::INFO)
      << std::setw(9) << (*particles)[i]->px() * igev
      << " | " << kr
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
  particles.keep (filter);
  msg(MSG::INFO) << "======== Index table =========" << endmsg;
  {
    SG::ThinningDecisionBase tmp = particles.decision();
    tmp.buildIndexMap();
    for ( std::size_t i = 0; i != particles->size(); ++i ) {
      std::size_t newIdx = tmp.index( i );
      std::stringstream newIdxStr; 
      newIdxStr << newIdx;
      msg(MSG::INFO)
        << " idx " << i 
        << " -> "  << (newIdx == SG::ThinningDecision::RemovedIdx 
                       ? "-"
                       : newIdxStr.str() )
        << endmsg;
    }
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
  particles.keep (filter, SG::ThinningDecisionBase::Op::Or);

  msg(MSG::INFO) << "======== Index table =========" << endmsg;
  {
    SG::ThinningDecisionBase tmp = particles.decision();
    tmp.buildIndexMap();
    for ( std::size_t i = 0; i != particles->size(); ++i ) {
      std::size_t newIdx = tmp.index( i );
      std::stringstream newIdxStr; 
      newIdxStr << newIdx;
      msg(MSG::INFO) 
        << " idx " << i 
        << " -> "  << (newIdx == SG::ThinningDecision::RemovedIdx 
                       ? "-"
                       : newIdxStr.str() )
        << endmsg;
    }
  }
  
  return StatusCode::SUCCESS;
}


StatusCode 
WriteThinnedData::doThinningTest3( const EventContext& ctx,
                                   const SG::ThinningHandleKey<AthExIParticles>& iparticlesKey ) const
{
  SG::ThinningHandle<AthExIParticles> iparticles (iparticlesKey, ctx);
  std::vector<bool> filter = m_filter.value();

  const double igev = 1. / Gaudi::Units::GeV;
  msg(MSG::INFO) << "IParticles | filter :" << endmsg;
  for ( unsigned int i = 0; i != iparticles->size(); ++i ) {
    const std::string kr = filter[i] ? "keep" : "remove";
    msg(MSG::INFO)
      << std::setw(9) << (*iparticles)[i]->px() * igev
      << " | " << kr
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
  iparticles.keep (filter);
  msg(MSG::INFO) << "======== Index table =========" << endmsg;
  {
    SG::ThinningDecisionBase tmp = iparticles.decision();
    tmp.buildIndexMap();
    for ( std::size_t i = 0; i != iparticles->size(); ++i ) {
      std::size_t newIdx = tmp.index( i );
      std::stringstream newIdxStr; 
      newIdxStr << newIdx;
      msg(MSG::INFO)
        << " idx " << i 
        << " -> "  << (newIdx == SG::ThinningDecision::RemovedIdx 
                       ? "-"
                       : newIdxStr.str() )
        << endmsg;
    }
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
  iparticles.keep (filter, SG::ThinningDecisionBase::Op::And);

  msg(MSG::INFO) << "======== Index table =========" << endmsg;
  {
    SG::ThinningDecisionBase tmp = iparticles.decision();
    tmp.buildIndexMap();
    for ( std::size_t i = 0; i != iparticles->size(); ++i ) {
      std::size_t newIdx = tmp.index( i );
      std::stringstream newIdxStr; 
      newIdxStr << newIdx;
      msg(MSG::INFO)
        << " idx " << i 
        << " -> "  << (newIdx == SG::ThinningDecision::RemovedIdx 
                       ? "-"
                       : newIdxStr.str() )
        << endmsg;
    }
  }
  
  return StatusCode::SUCCESS;
}
