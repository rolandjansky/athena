/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/// Class to test the McVtxFilter class
/// Author : S.Binet<binet@cern.ch>

// CppUnit includes
#include<cppunit/extensions/HelperMacros.h>
#include<cppunit/Exception.h>

// STL includes
#include <iostream>
#include <stdexcept>
#include <cmath>

// Framework includes
#include "StoreGate/StoreGateSvc.h"

// CLHEP includes
#include "CLHEP/Units/SystemOfUnits.h"

// HepMC includes
#include "AtlasHepMC/GenEvent.h"
#include "AtlasHepMC/GenVertex.h"
#include "AtlasHepMC/GenParticle.h"
#include "AtlasHepMC/SimpleVector.h"

#include "AthContainers/DataVector.h"

// McParticleUtils includes
#include "McParticleUtils/McVtxFilter.h"

class McVtxFilterTest : public CppUnit::TestFixture
{
  /// Definition of the unit test suite "FilterTest"
  CPPUNIT_TEST_SUITE( McVtxFilterTest );

  CPPUNIT_TEST( testConstructor );
  CPPUNIT_TEST( testCopyMcVtxFilter );
  CPPUNIT_TEST( testSettersAndGetters );
  
  CPPUNIT_TEST( testFullVtx );

  CPPUNIT_TEST( testZeeVertices );
  CPPUNIT_TEST( testTopWbggVertices );
  CPPUNIT_TEST( testWlnuVertices );
  CPPUNIT_TEST( testQuarkVertices );

  CPPUNIT_TEST( testStableParticle );
  
  /// end the definition test suite
  CPPUNIT_TEST_SUITE_END();

private:
  HepMC::GenEvent * m_evt;
  int               m_bcZee;
  int               m_bcZgee;
  int               m_bcTopWbgg;
  typedef HepMC::FourVector HLV_t;
  
public:

  /// Set up the data members
  void setUp()
  {
    const int signalProcessId = 1000082;
    const int evtNbr = 1;
    m_evt = new HepMC::GenEvent( signalProcessId, evtNbr );
    m_evt->set_event_scale( -1 );
    m_evt->set_alphaQCD( -1 );
    m_evt->set_alphaQED( -1 );
    std::vector<double> weights(3);
    weights[0] = 1;
    weights[1] = 1;
    weights[2] = 1;
    std::vector<long> rdmStates(2);
    rdmStates[0] = 85909879;
    rdmStates[1] = 9707499;
    m_evt->weights() = weights;
    m_evt->set_random_states( rdmStates );
    
    // Add 2 vertices
    HepMC::GenVertex * v1 = new HepMC::GenVertex();
    m_evt->add_vertex( v1 );
    v1->add_particle_in( new HepMC::GenParticle( HLV_t(0,0,
						       7000*GeV,
						       7000*GeV),
						 2212, 3 ) );
    HepMC::GenVertex* v2 = new HepMC::GenVertex;
    m_evt->add_vertex( v2 );
    v2->add_particle_in( new HepMC::GenParticle( HLV_t(0,0,
						       -7000*GeV,
						       +7000*GeV),
						 2212, 3 ) );

    //
    // create the outgoing particles of v1 and v2
    HepMC::GenParticle* p3 = 
      new HepMC::GenParticle( HLV_t(.750*GeV,
				    -1.569*GeV,
				    32.191*GeV,
				    32.238*GeV), 1, 3 );
    v1->add_particle_out( p3 );
    HepMC::GenParticle* p4 = 
      new HepMC::GenParticle( HLV_t( -3.047*GeV,
				     -19.*GeV,
				     -54.629*GeV,
				     57.920*GeV), -2, 3 );
    v2->add_particle_out( p4 );
    
    //
    // create v3
    HepMC::GenVertex* v3 = new HepMC::GenVertex();
    m_evt->add_vertex( v3 );
    v3->add_particle_in( p3 );
    v3->add_particle_in( p4 );
    v3->add_particle_out( 
	new HepMC::GenParticle( HLV_t(-3.813,0.113,-1.833,4.233 ), 
				22, 1 )
	);
    HepMC::GenParticle* p5 = 
	new HepMC::GenParticle( HLV_t(1.517,-20.68,-20.605,85.925), 
				-24,3);
    v3->add_particle_out( p5 );

    //    
    // tell the event which vertex is the signal process vertex
    m_evt->set_signal_process_vertex( v3 );

    //
    // add a Z0->4gammas,e+,e-
    //
    HepMC::GenVertex * vZgee = new HepMC::GenVertex;
    m_evt->add_vertex( vZgee );
    // Z0
    vZgee->add_particle_in( new HepMC::GenParticle( HLV_t( -4.49e+04,
							   +8.36e+03,
							   -2.70e+05,
							   +2.89e+05 ),
						    23, 2 ) );
    // Gammas
    vZgee->add_particle_out( new HepMC::GenParticle( HLV_t( -1.28e+03,
							    +1.03e+03,
							    -5.47e+03,
							    +5.71e+03 ),
						     22, 1 ) );
    vZgee->add_particle_out( new HepMC::GenParticle( HLV_t( +3.89e+02,
							    -3.16e+02,
							    -6.69e+03,
							    +6.70e+03 ),
						     22, 1 ) );
    vZgee->add_particle_out( new HepMC::GenParticle( HLV_t( +7.34e+00,
							    -2.71e+01,
							    -4.12e+01,
							    +4.98e+01 ),
						     22, 1 ) );
    vZgee->add_particle_out( new HepMC::GenParticle( HLV_t( -1.36e+02,
							    +9.38e+01,
							    -5.62e+02,
							    +5.86e+02 ),
						     22, 1 ) );
    // Electrons
    vZgee->add_particle_out( new HepMC::GenParticle( HLV_t( +8.01e+03,
							    -2.96e+04,
							    -4.50e+04,
							    +5.44e+04 ),
						     -11, 1 ) );
    vZgee->add_particle_out( new HepMC::GenParticle( HLV_t( -5.19e+04,
							    +3.72e+04,
							    -2.13e+05,
							    +2.22e+05 ),
						     11, 1 ) );
    // store its barcode for later use
    m_bcZgee = vZgee->barcode();

    //
    // Add a Z->e+e-
    HepMC::GenVertex * vZee = new HepMC::GenVertex;
    m_evt->add_vertex( vZee );
    vZee->add_particle_in( new HepMC::GenParticle( HLV_t( +7.29e+03,
							  +2.34e+04,
							  +2.81e+05,
							  +2.96e+05 ),
						   23, 2 ) );
    
    vZee->add_particle_out( new HepMC::GenParticle( HLV_t( +2.74e+04,
							   -1.83e+04,
							   +4.70e+04,
							   +5.74e+04 ),
						    11, 1 ) );
    vZee->add_particle_out( new HepMC::GenParticle( HLV_t( -2.01e+04,
							   +4.17e+04,
							   +2.34e+05,
							   +2.38e+05 ),
						    -11, 1 ) );
    m_bcZee = vZee->barcode();
   
    //
    // Add a t->W+bgg
    HepMC::GenVertex * vtWbgg = new HepMC::GenVertex;
    m_evt->add_vertex( vtWbgg );
    // top
    vtWbgg->add_particle_in(new HepMC::GenParticle(HLV_t(-2.35e+05,
							 +7.34e+04,
							 +3.60e+04,
							 +3.04e+05),
						   6, 3 ) );

    // Wbgg
    vtWbgg->add_particle_out(new HepMC::GenParticle(HLV_t(-1.09e+05,
							  +6.99e+04,
							  -3.86e+04,
							  +1.57e+05),
						    24, 2 ) );
    vtWbgg->add_particle_out(new HepMC::GenParticle(HLV_t(-9.23e+04,
							  +2.54e+03,
							  +5.32e+04,
							  +1.07e+05),
						    5, 2 ) );
    vtWbgg->add_particle_out(new HepMC::GenParticle(HLV_t(-4.76e+03,
							  +6.72e+02,
							  +2.90e+03,
							  +5.62e+03),
						    21, 2 ) );
    vtWbgg->add_particle_out(new HepMC::GenParticle(HLV_t(-2.93e+04,
							  +2.13e+02,
							  +1.85e+04,
							  +3.46e+04),
						    21, 2 ) );
    m_bcTopWbgg = vtWbgg->barcode();

  }

  /// destroy any on-the-heap-created data member
  void tearDown()
  {
    if ( m_evt ) {
      delete m_evt;
      m_evt = 0;
    }
  }

  /// Test the McVtxFilter constructors
  void testConstructor()
  {
    const unsigned int defaultCand = 0;
    McVtxFilter filter;
    CPPUNIT_ASSERT( false == filter.matchSign() );
    CPPUNIT_ASSERT( false == filter.matchBranches() );
    CPPUNIT_ASSERT( ""    == filter.decayPattern() );
    CPPUNIT_ASSERT( defaultCand == filter.parentList().size() );
    CPPUNIT_ASSERT( defaultCand == filter.childList().size()  );

    McVtxFilter f2( false, false );
    CPPUNIT_ASSERT( false == f2.matchSign() );
    CPPUNIT_ASSERT( false == f2.matchBranches() );
    CPPUNIT_ASSERT( ""    == f2.decayPattern() );
    CPPUNIT_ASSERT( defaultCand == f2.parentList().size() );
    CPPUNIT_ASSERT( defaultCand == f2.childList().size()  );

    McVtxFilter f3( true, false );
    CPPUNIT_ASSERT( true  == f3.matchSign() );
    CPPUNIT_ASSERT( false == f3.matchBranches() );
    CPPUNIT_ASSERT( ""    == f3.decayPattern() );
    CPPUNIT_ASSERT( defaultCand == f3.parentList().size() );
    CPPUNIT_ASSERT( defaultCand == f3.childList().size()  );

    McVtxFilter f4( false, true );
    CPPUNIT_ASSERT( false == f4.matchSign() );
    CPPUNIT_ASSERT( true  == f4.matchBranches() );
    CPPUNIT_ASSERT( ""    == f4.decayPattern() );
    CPPUNIT_ASSERT( defaultCand == f4.parentList().size() );
    CPPUNIT_ASSERT( defaultCand == f4.childList().size()  );

    McVtxFilter f5( f4 );
    CPPUNIT_ASSERT( false == f5.matchSign() );
    CPPUNIT_ASSERT( true  == f5.matchBranches() );
    CPPUNIT_ASSERT( ""    == f5.decayPattern() );
    CPPUNIT_ASSERT( defaultCand == f5.parentList().size() );
    CPPUNIT_ASSERT( defaultCand == f5.childList().size()  );

    McVtxFilter f6 = f4;
    CPPUNIT_ASSERT( false == f6.matchSign() );
    CPPUNIT_ASSERT( true  == f6.matchBranches() );
    CPPUNIT_ASSERT( ""    == f6.decayPattern() );
    CPPUNIT_ASSERT( defaultCand == f6.parentList().size() );
    CPPUNIT_ASSERT( defaultCand == f6.childList().size()  );

    {
      McVtxFilter f( "6 -> 24 + -5" );
      CPPUNIT_ASSERT( false == f.matchSign() );
      CPPUNIT_ASSERT( false == f.matchBranches() );
      CPPUNIT_ASSERT( "6 -> 24 + -5" == f.decayPattern() );
      CPPUNIT_ASSERT( 1 == f.parentList().size() );
      CPPUNIT_ASSERT( 2 == f.childList().size()  );
      CPPUNIT_ASSERT( f.isFullVtx() );
    }
    {
      McVtxFilter f( "6 -> 24 + -5", true, false );
      CPPUNIT_ASSERT( true  == f.matchSign() );
      CPPUNIT_ASSERT( false == f.matchBranches() );
      CPPUNIT_ASSERT( "6 -> 24 + -5" == f.decayPattern() );
      CPPUNIT_ASSERT( 1 == f.parentList().size() );
      CPPUNIT_ASSERT( 2 == f.childList().size()  );
      CPPUNIT_ASSERT( f.isFullVtx() );
    }
    {
      McVtxFilter f( "6 -> 24 + -5", false, true );
      CPPUNIT_ASSERT( false == f.matchSign() );
      CPPUNIT_ASSERT( true  == f.matchBranches() );
      CPPUNIT_ASSERT( "6 -> 24 + -5" == f.decayPattern() );
      CPPUNIT_ASSERT( 1 == f.parentList().size() );
      CPPUNIT_ASSERT( 2 == f.childList().size()  );
      CPPUNIT_ASSERT( f.isFullVtx() );
    }
    {
      McVtxFilter f( "6 -> ", false, true );
      CPPUNIT_ASSERT( false == f.matchSign() );
      CPPUNIT_ASSERT( true  == f.matchBranches() );
      CPPUNIT_ASSERT( "6 -> " == f.decayPattern() );
      CPPUNIT_ASSERT( 1 == f.parentList().size() );
      CPPUNIT_ASSERT( 0 == f.childList().size()  );
      CPPUNIT_ASSERT( !f.isFullVtx() );
    }
  }

  /// Test that McVtxFilter cuts are well copied
  void testCopyMcVtxFilter()
  {
    const unsigned int defaultCand = 0;
    McVtxFilter filter( false, true );
    CPPUNIT_ASSERT( false == filter.matchSign() );
    CPPUNIT_ASSERT( true  == filter.matchBranches() );
    CPPUNIT_ASSERT( ""    == filter.decayPattern() );
    CPPUNIT_ASSERT( defaultCand == filter.parentList().size() );
    CPPUNIT_ASSERT( defaultCand == filter.childList().size()  );
    CPPUNIT_ASSERT( false == filter.isFullVtx() );

    filter.setDecayPattern( "6 -> 24 + -5" );
    CPPUNIT_ASSERT( "6 -> 24 + -5" == filter.decayPattern() );
    CPPUNIT_ASSERT(         1  == filter.parentList().size() );
    CPPUNIT_ASSERT(         2  == filter.childList().size()  );
    CPPUNIT_ASSERT( true == filter.isFullVtx() );

    McVtxFilter copy;
    CPPUNIT_ASSERT( false == copy.matchSign() );
    CPPUNIT_ASSERT( false == copy.matchBranches() );
    CPPUNIT_ASSERT( ""    == copy.decayPattern() );
    CPPUNIT_ASSERT( defaultCand == copy.parentList().size() );
    CPPUNIT_ASSERT( defaultCand == copy.childList().size()  );
    CPPUNIT_ASSERT( false == copy.isFullVtx() );

    copy.setFilter( &filter );
    CPPUNIT_ASSERT( false      == copy.matchSign() );
    CPPUNIT_ASSERT( true       == copy.matchBranches() );
    CPPUNIT_ASSERT( "6 -> 24 + -5" == copy.decayPattern() );
    CPPUNIT_ASSERT(    1       == copy.parentList().size() );
    CPPUNIT_ASSERT(    2       == copy.childList().size()  );
    CPPUNIT_ASSERT( true == copy.isFullVtx() );
  }

  /// Test setters and getters
  void testSettersAndGetters()
  {
    std::cout << std::endl;
    //
    // Print event
    //
    m_evt->print();

    const unsigned int defaultCand = 0;
    McVtxFilter filter( false, true );
    CPPUNIT_ASSERT( false == filter.matchSign() );
    CPPUNIT_ASSERT( true  == filter.matchBranches() );
    CPPUNIT_ASSERT( ""    == filter.decayPattern() );
    CPPUNIT_ASSERT( defaultCand == filter.parentList().size() );
    CPPUNIT_ASSERT( defaultCand == filter.childList().size()  );

    filter.setMatchSign( true );
    CPPUNIT_ASSERT( true  == filter.matchSign() );
    filter.setMatchSign( false );
    CPPUNIT_ASSERT( false == filter.matchSign() );

    filter.setMatchBranches( false );
    CPPUNIT_ASSERT( false  == filter.matchBranches() );
    filter.setMatchBranches( true );
    CPPUNIT_ASSERT( true   == filter.matchBranches() );

    filter.setDecayPattern( "6 -> 24 + -5" );
    CPPUNIT_ASSERT( "6 -> 24 + -5" == filter.decayPattern() );
    CPPUNIT_ASSERT(             1  == filter.parentList().size() );
    CPPUNIT_ASSERT(             2  == filter.childList().size()  );

    filter.setDecayPattern( "23 -> -11|-13 + 11|13|15" );
    CPPUNIT_ASSERT( "23 -> -11|-13 + 11|13|15" == filter.decayPattern() );
    CPPUNIT_ASSERT(                         1  == filter.parentList().size() );
    CPPUNIT_ASSERT(                         2  == filter.childList().size()  );

    bool matchSign = false;
    CPPUNIT_ASSERT( (filter.parentList()[0])->hasInList(PDG::Z0, matchSign) );
    matchSign = true;
    CPPUNIT_ASSERT( (filter.parentList()[0])->hasInList(PDG::Z0, matchSign) );

    matchSign = false;
    CPPUNIT_ASSERT((filter.childList()[0])->hasInList(PDG::e_plus, matchSign));
    CPPUNIT_ASSERT((filter.childList()[0])->hasInList(PDG::mu_plus,matchSign));
    matchSign = true;
    CPPUNIT_ASSERT((filter.childList()[0])->hasInList(PDG::e_plus, matchSign));
    CPPUNIT_ASSERT((filter.childList()[0])->hasInList(PDG::mu_plus,matchSign));

    matchSign = true;
    CPPUNIT_ASSERT(!(filter.childList()[0])->hasInList( PDG::e_minus, 
							matchSign));
    CPPUNIT_ASSERT(!(filter.childList()[0])->hasInList( PDG::mu_minus,
							matchSign));
    
    CPPUNIT_ASSERT( 1 == (filter.parentList()[0])->size() );
    std::vector<PDG::pidType> parent;
    for ( std::list<PDG::pidType>::const_iterator itr = 
	    (filter.parentList()[0])->begin();
	  itr != (filter.parentList()[0])->end();
	  ++itr ) {
      parent.push_back( *itr );
    }
    
    CPPUNIT_ASSERT( PDG::Z0 == parent[0] );

    CPPUNIT_ASSERT( 2 == (filter.childList()[0])->size() );
    std::vector<PDG::pidType> child1;
    for ( std::list<PDG::pidType>::const_iterator itr = 
	    (filter.childList()[0])->begin();
	  itr != (filter.childList()[0])->end();
	  ++itr ) {
      child1.push_back( *itr );
    }
    CPPUNIT_ASSERT( 2 == child1.size() );
    CPPUNIT_ASSERT( PDG::e_plus  == child1[0] );
    CPPUNIT_ASSERT( PDG::mu_plus == child1[1] );

    CPPUNIT_ASSERT( 3 == (filter.childList()[1])->size() );
    std::vector<PDG::pidType> child2;
    for ( std::list<PDG::pidType>::const_iterator itr = 
	    (filter.childList()[1])->begin();
	  itr != (filter.childList()[1])->end();
	  ++itr ) {
      child2.push_back( *itr );
    }
    CPPUNIT_ASSERT( 3 == child2.size() );

    CPPUNIT_ASSERT( PDG::e_minus   == child2[0] );
    CPPUNIT_ASSERT( PDG::mu_minus  == child2[1] );
    CPPUNIT_ASSERT( PDG::tau_minus == child2[2] );

  }

  /// Test full vtx
  void testFullVtx()
  {
    HepMC::GenVertex * vtx = new HepMC::GenVertex;
    m_evt->add_vertex( vtx );
    vtx->add_particle_in( new HepMC::GenParticle( HLV_t( -2.45e+04,
							 +1.88e+04,
							 -8.65e+05,
							 +8.65e+05 ),
						  22, 3 ) );
    vtx->add_particle_out( new HepMC::GenParticle( HLV_t( -2.45e+04,
							  +1.88e+04,
							  -8.65e+05,
							  +8.65e+05 ),
						   22, 1 ) );
    
    McVtxFilter filter;
    filter.setDecayPattern( "22->" );
    CPPUNIT_ASSERT( false == filter.isFullVtx() );
    CPPUNIT_ASSERT( filter.isAccepted( vtx ) );

    filter.setDecayPattern( "->22" );
    CPPUNIT_ASSERT( false == filter.isFullVtx() );
    CPPUNIT_ASSERT( filter.isAccepted( vtx ) );

    filter.setDecayPattern( "24->-24" );
    CPPUNIT_ASSERT( true == filter.isFullVtx() );
    CPPUNIT_ASSERT( !filter.isAccepted( vtx ) );

    filter.setDecayPattern( "-> -24 + 24" );
    CPPUNIT_ASSERT( false == filter.isFullVtx() );
    CPPUNIT_ASSERT( !filter.isAccepted( vtx ) );

    filter.setDecayPattern( "22 -> -11 + 11" );
    CPPUNIT_ASSERT( true == filter.isFullVtx() );
    CPPUNIT_ASSERT( !filter.isAccepted( vtx ) );

    filter.setDecayPattern( "22 -> -11" );
    CPPUNIT_ASSERT( true == filter.isFullVtx() );
    CPPUNIT_ASSERT( !filter.isAccepted( vtx ) );

    filter.setDecayPattern( "22 -> 22" );
    CPPUNIT_ASSERT( true == filter.isFullVtx() );
    CPPUNIT_ASSERT( filter.isAccepted( vtx ) );

  }

  /// Test some vertices
  void testZeeVertices()
  {
    McVtxFilter filter;
    filter.setDecayPattern( "23 -> -11 + 11" );

    const HepMC::GenVertex * vtx = m_evt->barcode_to_vertex(m_bcZee);
    CPPUNIT_ASSERT( 0 != vtx );

    CPPUNIT_ASSERT( filter.isAccepted(vtx) );

    filter.setMatchSign( false );
    CPPUNIT_ASSERT( filter.isAccepted(vtx) );
    filter.setMatchSign( true );
    CPPUNIT_ASSERT( filter.isAccepted(vtx) );

    filter.setMatchBranches( true );
    CPPUNIT_ASSERT( filter.isAccepted(vtx) );
    
    // Retrieve a vtx with FSR showering
    vtx = m_evt->barcode_to_vertex(m_bcZgee);
    CPPUNIT_ASSERT( 0 != vtx );

    filter.setMatchSign( false );
    filter.setMatchBranches( true );
    CPPUNIT_ASSERT( !filter.isAccepted(vtx) );

    filter.setMatchSign( false );
    filter.setMatchBranches( false );
    CPPUNIT_ASSERT( filter.isAccepted(vtx) );

    filter.setMatchSign( true );
    filter.setMatchBranches( false );
    CPPUNIT_ASSERT( filter.isAccepted(vtx) );

    filter.setMatchSign( true );
    filter.setMatchBranches( true );
    CPPUNIT_ASSERT( !filter.isAccepted(vtx) );
  }

  /// Test some vertices
  void testTopWbggVertices()
  {
    const HepMC::GenVertex * vtx = m_evt->barcode_to_vertex(m_bcTopWbgg);
    CPPUNIT_ASSERT( 0 != vtx );

    {
      McVtxFilter filter;
      filter.setDecayPattern( "6 -> 24 + 5" );
      
      filter.setMatchSign( true );
      filter.setMatchBranches( false );
      CPPUNIT_ASSERT( filter.isAccepted(vtx) );
      
      filter.setMatchSign( true );
      filter.setMatchBranches( true );
      CPPUNIT_ASSERT( !filter.isAccepted(vtx) );
      
      filter.setMatchSign( false );
      filter.setMatchBranches( false );
      CPPUNIT_ASSERT( filter.isAccepted(vtx) );
      
      filter.setMatchSign( false );
      filter.setMatchBranches( true );
      CPPUNIT_ASSERT( !filter.isAccepted(vtx) );
    }
  }

  /// Test some vertices
  void testWlnuVertices()
  {
    
    HepMC::GenVertex * vtx = new HepMC::GenVertex;
    m_evt->add_vertex( vtx );
    vtx->add_particle_in( new HepMC::GenParticle( HLV_t( -6.76e+04,
							 +4.85e+03,
							 -1.46e+03,
							 +9.51e+04 ),
						  -24, 2 ) );
    vtx->add_particle_out( new HepMC::GenParticle( HLV_t( -7.14e+04,
							  -6.17e+03,
							  +1.67e+04,
							  +7.36e+04 ),
						   13, 1 ) );
    
    vtx->add_particle_out( new HepMC::GenParticle( HLV_t( +3.75e+03,
							  +1.10e+04,
							  -1.81e+04,
							  +2.15e+04 ),
						   -14, 1 ) );
    //const int bcWlnu = vtx->barcode();

    McVtxFilter filter;
    filter.setDecayPattern( "24 -> -13 + -14" );

    filter.setMatchSign( true );
    CPPUNIT_ASSERT( !filter.isAccepted(vtx) );
    filter.setMatchSign( false );
    CPPUNIT_ASSERT( filter.isAccepted(vtx) );

    filter.setDecayPattern( "-24 -> 13 + -14" );
    filter.setMatchSign( true );
    CPPUNIT_ASSERT( filter.isAccepted(vtx) );
    filter.setMatchSign( false );
    CPPUNIT_ASSERT( filter.isAccepted(vtx) );
    
  }

  /*
GenVertex:       -5 ID:    0 (X,cT):0
 I: 2         5       21 +1.23e+04,-4.79e+03,+6.52e+04,+6.65e+04   3        -5
              6       21 +1.14e+02,+1.35e+04,-6.42e+04,+6.56e+04   3        -5
 O: 2         7        5 +4.56e+04,+1.53e+04,-5.08e+04,+7.01e+04   3        -8
              8       -5 -3.32e+04,-6.56e+03,+5.18e+04,+6.20e+04   3        -9
GenVertex:      -39 ID:    0 (X,cT):0
 I: 1       158       -5 -3.21e+04,-6.19e+03,+5.05e+04,+6.38e+04   2       -39
 O: 2       159       -5 -1.29e+04,+1.12e+03,+3.50e+04,+3.77e+04   2       -92
            160       21 -1.92e+04,-7.30e+03,+1.55e+04,+2.61e+04   2       -40
   */

  /// Test some vertices
  void testQuarkVertices()
  {
    // create a b->g+b vertex
    HepMC::GenVertex * vtxgb = new HepMC::GenVertex;
    m_evt->add_vertex( vtxgb );
    vtxgb->add_particle_in( new HepMC::GenParticle( HLV_t( -3.21e+04,
							   -6.19e+03,
							   +5.05e+04,
							   +6.38e+04 ),
						    -5, 2 ) );
    vtxgb->add_particle_out( new HepMC::GenParticle( HLV_t( -1.29e+04,
							    +1.12e+03,
							    +3.50e+04,
							    +3.77e+04 ),
						     -5, 2 ) );
    
    vtxgb->add_particle_out( new HepMC::GenParticle( HLV_t( -1.92e+04,
							    -7.30e+03,
							    +1.55e+04,
							    +2.61e+04 ),
						     21, 2 ) );

    // create a gg->b+bbar vertex
    HepMC::GenVertex * vtxbb = new HepMC::GenVertex;
    m_evt->add_vertex( vtxbb );
    vtxbb->add_particle_in( new HepMC::GenParticle( HLV_t( +1.23e+04,
							   -4.79e+03,
							   +6.52e+04,
							   +6.65e+04 ),
						    21, 3 ) );
    vtxbb->add_particle_in( new HepMC::GenParticle( HLV_t( +1.14e+02,
							   +1.35e+04,
							   -6.42e+04,
							   +6.56e+04 ),
						    21, 3 ) );
    vtxbb->add_particle_out( new HepMC::GenParticle( HLV_t( +4.56e+04,
							    +1.53e+04,
							    -5.08e+04,
							    +7.01e+04 ),
						     5, 3 ) );
    
    vtxbb->add_particle_out( new HepMC::GenParticle( HLV_t( -3.32e+04,
							    -6.56e+03,
							    +5.18e+04,
							    +6.20e+04 ),
						     -5, 3 ) );

    McVtxFilter filter;
    filter.setDecayPattern( " -> -5 + 5" );
    
    filter.setMatchSign( false );
    CPPUNIT_ASSERT(  filter.isAccepted(vtxbb) );
    CPPUNIT_ASSERT( !filter.isAccepted(vtxgb) );
    
    filter.setMatchSign( true );
    CPPUNIT_ASSERT(  filter.isAccepted(vtxbb) );
    CPPUNIT_ASSERT( !filter.isAccepted(vtxgb) );

    filter.setDecayPattern( "21+21 -> -5 + 5" );
    filter.setMatchSign( false );
    CPPUNIT_ASSERT(  filter.isAccepted(vtxbb) );
    CPPUNIT_ASSERT( !filter.isAccepted(vtxgb) );
    
    filter.setMatchSign( true );
    CPPUNIT_ASSERT(  filter.isAccepted(vtxbb) );
    CPPUNIT_ASSERT( !filter.isAccepted(vtxgb) );

    filter.setMatchBranches(true);
    filter.setMatchSign( false );
    CPPUNIT_ASSERT(  filter.isAccepted(vtxbb) );
    CPPUNIT_ASSERT( !filter.isAccepted(vtxgb) );
    
    filter.setMatchSign( true );
    CPPUNIT_ASSERT(  filter.isAccepted(vtxbb) );
    CPPUNIT_ASSERT( !filter.isAccepted(vtxgb) );

  }

  /// Test Stable particles (no end_vertex)
  void testStableParticle()
  {
    HepMC::GenVertex * vtx = new HepMC::GenVertex;
    m_evt->add_vertex( vtx );
    vtx->add_particle_in( new HepMC::GenParticle( HLV_t( -2.45e+04,
							 +1.88e+04,
							 -8.65e+05,
							 +8.65e+05 ),
						  23, 3 ) );
    HepMC::GenParticle * photon = 0;
    photon = new HepMC::GenParticle( HLV_t( -2.45e+04,
					    +1.88e+04,
					    -8.65e+05,
					    +8.65e+05 ),
				     22, 1 );
    vtx->add_particle_out( photon );

    // no end_vertex so : particle is stable
    CPPUNIT_ASSERT( photon->end_vertex() == 0 );

    McVtxFilter filter;
    filter.setDecayPattern( "23->" );
    CPPUNIT_ASSERT( false == filter.isFullVtx() );
    CPPUNIT_ASSERT( filter.isAccepted( vtx ) );

    filter.setDecayPattern( "22->" );
    CPPUNIT_ASSERT( false == filter.isFullVtx() );
    CPPUNIT_ASSERT( !filter.isAccepted( vtx ) );

    filter.setDecayPattern( "->23" );
    CPPUNIT_ASSERT( false == filter.isFullVtx() );
    CPPUNIT_ASSERT( !filter.isAccepted( vtx ) );

    filter.setDecayPattern( "->22" );
    CPPUNIT_ASSERT( false == filter.isFullVtx() );
    CPPUNIT_ASSERT( filter.isAccepted( vtx ) );

  }

};

/// Registration of the test suite "McVtxFilterTest" so it will be 
/// recognized by the main test program which drives the different tests
CPPUNIT_TEST_SUITE_REGISTRATION( McVtxFilterTest );

/// CppUnit test-driver common for all the cppunit test classes.
#include <TestTools/CppUnit_SGtestdriver.cxx>
