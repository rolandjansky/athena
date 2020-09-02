/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/// Class to test the PdgIdFilter class
/// Author : S.Binet<binet@cern.ch>

// CppUnit includes
#include<cppunit/extensions/HelperMacros.h>
#include<cppunit/Exception.h>

// STL includes
#include <iostream>
#include <stdexcept>
#include <cmath>

// CLHEP includes
#include "CLHEP/Units/SystemOfUnits.h"
#include "AtlasHepMC/GenParticle.h"

#include "AthContainers/DataVector.h"

// EventKernel includes
#include "EventKernel/IParticle.h"

// FourMom includes
#include "FourMom/P4PxPyPzE.h"
#include "FourMom/P4EEtaPhiM.h"

// JetEvent
#include "JetEvent/Jet.h"
#include "JetEvent/JetCollection.h"

// McParticleEvent
#include "McParticleEvent/TruthParticle.h"

// AnalysisUtils includes
#include "AnalysisUtils/AnalysisSelection.h"
#include "AnalysisUtils/AnalysisCombination.h"
#include "AnalysisUtils/PdgIdFilter.h"

class PdgIdFilterTest : public CppUnit::TestFixture
{
  /// Definition of the unit test suite " PdgIdFilterTest"
  CPPUNIT_TEST_SUITE( PdgIdFilterTest );

  CPPUNIT_TEST( testCopyPdgIdFilter );
  CPPUNIT_TEST( testSettersAndGetters );

  CPPUNIT_TEST( testBQuarkFilter );

  CPPUNIT_TEST( testLightFlavourFilter );
  CPPUNIT_TEST( testHeavyFlavourFilter );

  /// end the definition test suite
  CPPUNIT_TEST_SUITE_END();

private:
  unsigned int           m_nJets;
  unsigned int           m_nLightJets;
  unsigned int           m_nBJets;
  JetCollection * m_jets;

public:

  /// Set up the data members
  void setUp()
  {
    m_nJets      = 10;
    m_nLightJets =  8;
    m_nBJets     =  2;
    m_jets  = new JetCollection;

    const double px = 100.*GeV;
    const double py = 100.*GeV;
    const double pz = 100.*GeV;
    const double E  = 200.*GeV;

    std::vector<PDG::pidType> pdgs;
    pdgs.push_back( PDG::null );
    pdgs.push_back( PDG::d );
    pdgs.push_back( PDG::s );
    pdgs.push_back( PDG::b );

    for ( unsigned int i = 0; i < m_nJets; ++i ) {
      Jet * jet = 0;
      jet = new Jet();

      jet->set4Mom( P4PxPyPzE( ((i%4)+1)*px, ((i%4)+1)*py, 
			       ((i%4)+1)*pz, ((i%4)+1)*E ) );
      jet->set_pdgId( pdgs[i%4] );
      m_jets->push_back(jet);
      /*
      std::cout << "\tjet "  << i 
		<< "pdgId= " << jet->pdgId() 
		<< std::endl;
      */
    }
  }

  /// destroy any on-the-heap-created data member
  void tearDown()
  {
    if ( m_jets ) {
      delete m_jets;
      m_jets = 0;
    }
  }

  /// Test that PdgIdFilter cuts are well copied
  void testCopyPdgIdFilter()
  {
    PdgIdFilter<IParticle> orig;
    
    Jet bbar;
    bbar.set_pdgId( PDG::b );
    CPPUNIT_ASSERT( orig.isAccepted( &bbar ) );

    bbar.set_pdgId( PDG::anti_b );
    CPPUNIT_ASSERT( orig.isAccepted( &bbar ) );

    PdgIdFilter<IParticle> filter;
    filter.setPdgId( PDG::b );
    CPPUNIT_ASSERT( !filter.isAccepted( &bbar ) );

    filter.setFilter( &orig );
    CPPUNIT_ASSERT( filter.isAccepted( &bbar ) );

    orig.setPdgId( PDG::b );
    CPPUNIT_ASSERT( !orig.isAccepted( &bbar ) );

    filter.setFilter( &orig );
    CPPUNIT_ASSERT( !filter.isAccepted( &bbar ) );
    
  }

  /// Test setters and getters
  void testSettersAndGetters()
  {
    PdgIdFilter<IParticle> filter;
    CPPUNIT_ASSERT( true  == filter.matchSign() );

    filter.setMatchSign( false );
    CPPUNIT_ASSERT( false == filter.matchSign() );

    filter.setMatchSign( true );
    CPPUNIT_ASSERT( true  == filter.matchSign() );
    
  }
  
  /// Test that PdgIdFilter selects b-quarks
  void testBQuarkFilter()
  {
    PdgIdFilter<IParticle> filter;
    filter.setPdgId( PDG::b );
    
    Jet bjet;

    bjet.set_pdgId( PDG::b );
    CPPUNIT_ASSERT( filter.isAccepted( &bjet ) );

    bjet.set_pdgId( PDG::anti_b );
    CPPUNIT_ASSERT( !filter.isAccepted( &bjet ) );

    filter.setMatchSign( false );
    CPPUNIT_ASSERT( filter.isAccepted( &bjet ) );

    HepMC::GenParticle hepBQuark;
    hepBQuark.set_pdg_id( static_cast<int>(PDG::anti_b) );
    TruthParticle bquark(&hepBQuark);
    filter.setMatchSign( false );
    CPPUNIT_ASSERT( filter.isAccepted( &bquark ) );

    filter.setMatchSign( true );
    CPPUNIT_ASSERT( !filter.isAccepted( &bquark ) );
  }

  /// Test the filtering on light flavour
  void testLightFlavourFilter() 
  {
    ParticleCandidateList pdgIds; 
    pdgIds.clear();
    pdgIds.addLightJets();
    
    //> Create a light jet filter
    PdgIdFilter<IParticle> ljetFilter;
    ljetFilter.setPdgIds( pdgIds );

    // Test with a std::vector
    std::vector<Jet*> lightJets;
    CPPUNIT_ASSERT( lightJets.size() == static_cast<unsigned int>(0) );
    AnalysisUtils::copy_if( m_jets->begin(),   
			    m_jets->end(),
			    std::back_inserter(lightJets), 
			    ljetFilter );
    CPPUNIT_ASSERT( lightJets.size() == m_nLightJets );

    // Test with a Selection
    AnalysisUtils::Selection<PdgIdFilter<IParticle> > ljetFilterSel;
    ljetFilterSel.setPdgIds( pdgIds );

    lightJets.clear();
    CPPUNIT_ASSERT( lightJets.size() == static_cast<unsigned int>(0) );
    ljetFilterSel.getObjects( m_jets, lightJets);
    
    CPPUNIT_ASSERT( (lightJets.size() == m_nLightJets) );

    // Test with a JetCollection
    JetCollection lJets(SG::VIEW_ELEMENTS);
    CPPUNIT_ASSERT( lJets.size() == static_cast<unsigned int>(0) );
    AnalysisUtils::copy_if( m_jets->begin(),   
			    m_jets->end(),
			    std::back_inserter(lJets), 
			    ljetFilter );
    CPPUNIT_ASSERT( lJets.size() == m_nLightJets );
  }

  /// Test the filtering on heavy flavour
  void testHeavyFlavourFilter() 
  {
    ParticleCandidateList pdgIds; 

    //> Create a b-jet filter
    PdgIdFilter<IParticle> bjetFilter;
    pdgIds.clear();
    pdgIds.add( PDG::b );
    pdgIds.add( PDG::anti_b );
    bjetFilter.setPdgIds( pdgIds );
    
    std::vector<Jet*> bJets;
    AnalysisUtils::copy_if( m_jets->begin(), 
			    m_jets->end(),
			    std::back_inserter(bJets), 
			    bjetFilter );

    CPPUNIT_ASSERT( bJets.size() == m_nBJets );

    return;
  }
};

/// Registration of the test suite "PdgIdFilterTest" so it will be 
/// recognized by the main test program which drives the different tests
CPPUNIT_TEST_SUITE_REGISTRATION( PdgIdFilterTest );

/// CppUnit test-driver common for all the cppunit test classes.
#include <TestTools/CppUnit_SGtestdriver.cxx>
