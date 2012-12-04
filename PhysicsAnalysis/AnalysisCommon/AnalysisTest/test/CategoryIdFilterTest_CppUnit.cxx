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

// DataModel includes
#include "DataModel/DataVector.h"

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
#include "AnalysisUtils/CategoryIdFilter.h"

class CategoryIdFilterTest : public CppUnit::TestFixture
{
  /// Definition of the unit test suite " CategoryIdFilterTest"
  CPPUNIT_TEST_SUITE( CategoryIdFilterTest );

  CPPUNIT_TEST( testCopyCategoryIdFilter );
  CPPUNIT_TEST( testSettersAndGetters );

  CPPUNIT_TEST( testQuarkFilter );

  CPPUNIT_TEST( testLightFlavourFilter );
  CPPUNIT_TEST( testHeavyFlavourFilter );

  CPPUNIT_TEST( testLeptonFilter );

  CPPUNIT_TEST( testBaryonFilter );
  CPPUNIT_TEST( testMesonFilter );

  CPPUNIT_TEST( testDiQuarkFilter );
  CPPUNIT_TEST( testBosonFilter );


  /// end the definition test suite
  CPPUNIT_TEST_SUITE_END();

private:
  unsigned int    m_nJets;
  unsigned int    m_nLightJets;
  unsigned int    m_nBJets;
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

  /// Test that CategoryIdFilter cuts are well copied
  void testCopyCategoryIdFilter()
  {
    CategoryIdFilter<IParticle> orig;
    
    Jet bbar;
    bbar.set_pdgId( PDG::b );
    CPPUNIT_ASSERT( orig.isAccepted( &bbar ) );

    bbar.set_pdgId( PDG::anti_b );
    CPPUNIT_ASSERT( orig.isAccepted( &bbar ) );

    CategoryIdFilter<IParticle> filter;
    filter.setCategory( PDGCat::CFlavour );
    CPPUNIT_ASSERT( !filter.isAccepted( &bbar ) );

    filter.setCategory( PDGCat::BFlavour );
    CPPUNIT_ASSERT( filter.isAccepted( &bbar ) );

    filter.setFilter( &orig );
    CPPUNIT_ASSERT( filter.isAccepted( &bbar ) );

    orig.setCategory( PDGCat::CFlavour );
    CPPUNIT_ASSERT( !orig.isAccepted( &bbar ) );

    filter.setFilter( &orig );
    CPPUNIT_ASSERT( !filter.isAccepted( &bbar ) );
    
    {
      CategoryIdFilter<IParticle> copy;
      CPPUNIT_ASSERT( copy.isAccepted( &bbar ) );
      copy = filter;
      CPPUNIT_ASSERT( !copy.isAccepted( &bbar ) );
    }
    {
      CategoryIdFilter<IParticle> copy = filter;
      CPPUNIT_ASSERT( !copy.isAccepted( &bbar ) );
    }
    {
      CategoryIdFilter<IParticle> copy(filter);
      CPPUNIT_ASSERT( !copy.isAccepted( &bbar ) );
    }
  }

  /// Test setters and getters
  void testSettersAndGetters()
  {
    CategoryIdFilter<IParticle> filter;
    CPPUNIT_ASSERT( filter.getCategories().empty() );

    filter.setCategory( PDGCat::BFlavour );
    CPPUNIT_ASSERT( !filter.getCategories().empty() );
    CPPUNIT_ASSERT( PDGCat::BFlavour == filter.getCategories()[0] );

    filter.addCategory( PDGCat::TFlavour );
    CPPUNIT_ASSERT( 2 == filter.getCategories().size() );
    CPPUNIT_ASSERT( PDGCat::TFlavour == filter.getCategories()[1] );
    
  }
  
  /// Test that CategoryIdFilter selects b-quarks
  void testQuarkFilter()
  {
    CategoryIdFilter<IParticle> mesonFilter;
    mesonFilter.setCategory( PDGCat::Meson );

    CategoryIdFilter<IParticle> filter;
    Jet j;

    // u
    filter.setCategory( PDGCat::UFlavour );
    
    j.set_pdgId( PDG::u );
    CPPUNIT_ASSERT( filter.isAccepted( &j ) );
    CPPUNIT_ASSERT( !mesonFilter.isAccepted( &j ) );
    j.set_pdgId( PDG::anti_u );
    CPPUNIT_ASSERT( filter.isAccepted( &j ) );
    CPPUNIT_ASSERT( !mesonFilter.isAccepted( &j ) );

    // d
    filter.setCategory( PDGCat::DFlavour );
    
    j.set_pdgId( PDG::d );
    CPPUNIT_ASSERT( filter.isAccepted( &j ) );
    CPPUNIT_ASSERT( !mesonFilter.isAccepted( &j ) );
    j.set_pdgId( PDG::anti_d );
    CPPUNIT_ASSERT( filter.isAccepted( &j ) );
    CPPUNIT_ASSERT( !mesonFilter.isAccepted( &j ) );

    // c
    filter.setCategory( PDGCat::CFlavour );
    
    j.set_pdgId( PDG::c );
    CPPUNIT_ASSERT( filter.isAccepted( &j ) );
    CPPUNIT_ASSERT( !mesonFilter.isAccepted( &j ) );
    j.set_pdgId( PDG::anti_c );
    CPPUNIT_ASSERT( filter.isAccepted( &j ) );
    CPPUNIT_ASSERT( !mesonFilter.isAccepted( &j ) );

    // s
    filter.setCategory( PDGCat::SFlavour );
    
    j.set_pdgId( PDG::s );
    CPPUNIT_ASSERT( filter.isAccepted( &j ) );
    CPPUNIT_ASSERT( !mesonFilter.isAccepted( &j ) );
    j.set_pdgId( PDG::anti_s );
    CPPUNIT_ASSERT( filter.isAccepted( &j ) );
    CPPUNIT_ASSERT( !mesonFilter.isAccepted( &j ) );

    // t
    filter.setCategory( PDGCat::TFlavour );
    
    j.set_pdgId( PDG::t );
    CPPUNIT_ASSERT( filter.isAccepted( &j ) );
    CPPUNIT_ASSERT( !mesonFilter.isAccepted( &j ) );
    j.set_pdgId( PDG::anti_t );
    CPPUNIT_ASSERT( filter.isAccepted( &j ) );
    CPPUNIT_ASSERT( !mesonFilter.isAccepted( &j ) );

    // b
    filter.setCategory( PDGCat::BFlavour );
    
    j.set_pdgId( PDG::b );
    CPPUNIT_ASSERT( filter.isAccepted( &j ) );
    CPPUNIT_ASSERT( !mesonFilter.isAccepted( &j ) );
    j.set_pdgId( PDG::anti_b );
    CPPUNIT_ASSERT( filter.isAccepted( &j ) );
    CPPUNIT_ASSERT( !mesonFilter.isAccepted( &j ) );

  }

  /// Test the filtering on light flavour
  void testLightFlavourFilter() 
  {
    //> Create a light jet filter
    CategoryIdFilter<IParticle> ljetFilter;
    ljetFilter.setCategory( PDGCat::LightFlavour );

    // Test with a std::vector
    std::vector<Jet*> lightJets;
    CPPUNIT_ASSERT( lightJets.size() == static_cast<unsigned int>(0) );
    AnalysisUtils::copy_if( m_jets->begin(),   
			    m_jets->end(),
			    std::back_inserter(lightJets), 
			    ljetFilter );
    CPPUNIT_ASSERT( lightJets.size() != m_nLightJets );

    // Now add the category for "untagged jets"
    ljetFilter.addCategory( PDGCat::Untagged );
    lightJets.clear();
    AnalysisUtils::copy_if( m_jets->begin(),   
			    m_jets->end(),
			    std::back_inserter(lightJets), 
			    ljetFilter );
    CPPUNIT_ASSERT( lightJets.size() == m_nLightJets );

    // Test with a Selection
    AnalysisUtils::Selection<CategoryIdFilter<IParticle> > ljetFilterSel;
    ljetFilterSel.setCategory( PDGCat::LightFlavour );
    ljetFilterSel.addCategory( PDGCat::Untagged );

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
    //> Create a b-jet filter
    CategoryIdFilter<IParticle> bjetFilter;
    bjetFilter.setCategory( PDGCat::BFlavour );
    
    std::vector<Jet*> bJets;
    bJets.reserve (m_nBJets);
    AnalysisUtils::copy_if( m_jets->begin(), 
			    m_jets->end(),
			    std::back_inserter(bJets), 
			    bjetFilter );

    CPPUNIT_ASSERT( bJets.size() == m_nBJets );

    return;
  }

  void testLeptonFilter()
  {
    Jet ele;

    CategoryIdFilter<IParticle> filter;
    filter.setCategory( PDGCat::Lepton );
    
    ele.set_pdgId( PDG::e_minus );
    CPPUNIT_ASSERT( filter.isAccepted( &ele ) );
    ele.set_pdgId( PDG::e_plus );
    CPPUNIT_ASSERT( filter.isAccepted( &ele ) );

    for ( JetCollection::const_iterator jet = m_jets->begin();
	  jet != m_jets->end();
	  ++jet ) {
      CPPUNIT_ASSERT( !filter.isAccepted( *jet ) );
    }

  }

  void testBaryonFilter()
  {
    Jet j;

    CategoryIdFilter<IParticle> hadron;
    hadron.setCategory( PDGCat::Hadron );

    CategoryIdFilter<IParticle> filter;
    filter.setCategory( PDGCat::Baryon );

    j.set_pdgId( PDG::Delta_minus );
    CPPUNIT_ASSERT( filter.isAccepted( &j ) );
    CPPUNIT_ASSERT( hadron.isAccepted( &j ) );

    j.set_pdgId( PDG::p_plus );
    CPPUNIT_ASSERT( filter.isAccepted( &j ) );
    CPPUNIT_ASSERT( hadron.isAccepted( &j ) );

    j.set_pdgId( PDG::Sigma_minus );
    CPPUNIT_ASSERT( filter.isAccepted( &j ) );
    CPPUNIT_ASSERT( hadron.isAccepted( &j ) );

    j.set_pdgId( PDG::Lambda0 );
    CPPUNIT_ASSERT( filter.isAccepted( &j ) );
    CPPUNIT_ASSERT( hadron.isAccepted( &j ) );

    j.set_pdgId( PDG::Xi_minus );
    CPPUNIT_ASSERT( filter.isAccepted( &j ) );
    CPPUNIT_ASSERT( hadron.isAccepted( &j ) );
		    
    j.set_pdgId( PDG::Xi0 );
    CPPUNIT_ASSERT( filter.isAccepted( &j ) );
    CPPUNIT_ASSERT( hadron.isAccepted( &j ) );
		    
    j.set_pdgId( PDG::Omega_minus );
    CPPUNIT_ASSERT( filter.isAccepted( &j ) );
    CPPUNIT_ASSERT( hadron.isAccepted( &j ) );
		    
    j.set_pdgId( PDG::Sigma_c0 );
    CPPUNIT_ASSERT( filter.isAccepted( &j ) );
    CPPUNIT_ASSERT( hadron.isAccepted( &j ) );
		    
    j.set_pdgId( PDG::Lambda_c_plus );
    CPPUNIT_ASSERT( filter.isAccepted( &j ) );
    CPPUNIT_ASSERT( hadron.isAccepted( &j ) );
		    
    j.set_pdgId( PDG::Xi_c0 );
    CPPUNIT_ASSERT( filter.isAccepted( &j ) );
    CPPUNIT_ASSERT( hadron.isAccepted( &j ) );
		    
    j.set_pdgId( PDG::Sigma_c_plus_plus );
    CPPUNIT_ASSERT( filter.isAccepted( &j ) );
    CPPUNIT_ASSERT( hadron.isAccepted( &j ) );
		    
    j.set_pdgId( PDG::Xi_c_plus );
    CPPUNIT_ASSERT( filter.isAccepted( &j ) );
    CPPUNIT_ASSERT( hadron.isAccepted( &j ) );
		    
    j.set_pdgId( PDG::Xi_prime_c0 );
    CPPUNIT_ASSERT( filter.isAccepted( &j ) );
    CPPUNIT_ASSERT( hadron.isAccepted( &j ) );
		    
    j.set_pdgId( PDG::Omega_c0 );
    CPPUNIT_ASSERT( filter.isAccepted( &j ) );
    CPPUNIT_ASSERT( hadron.isAccepted( &j ) );
		    
    j.set_pdgId( PDG::Sigma_b_minus );
    CPPUNIT_ASSERT( filter.isAccepted( &j ) );
    CPPUNIT_ASSERT( hadron.isAccepted( &j ) );
		    
    j.set_pdgId( PDG::Lambda_b0 );
    CPPUNIT_ASSERT( filter.isAccepted( &j ) );
    CPPUNIT_ASSERT( hadron.isAccepted( &j ) );
		    
    j.set_pdgId( PDG::Xi_b_minus );
    CPPUNIT_ASSERT( filter.isAccepted( &j ) );
    CPPUNIT_ASSERT( hadron.isAccepted( &j ) );
		    
    j.set_pdgId( PDG::Sigma_b0 );
    CPPUNIT_ASSERT( filter.isAccepted( &j ) );
    CPPUNIT_ASSERT( hadron.isAccepted( &j ) );
		    
    j.set_pdgId( PDG::Sigma_b_plus );
    CPPUNIT_ASSERT( filter.isAccepted( &j ) );
    CPPUNIT_ASSERT( hadron.isAccepted( &j ) );
		    
    j.set_pdgId( PDG::Xi_b0 );
    CPPUNIT_ASSERT( filter.isAccepted( &j ) );
    CPPUNIT_ASSERT( hadron.isAccepted( &j ) );
		    
    j.set_pdgId( PDG::Xi_prime_b_minus );
    CPPUNIT_ASSERT( filter.isAccepted( &j ) );
    CPPUNIT_ASSERT( hadron.isAccepted( &j ) );
		    
    j.set_pdgId( PDG::Xi_prime_b0 );
    CPPUNIT_ASSERT( filter.isAccepted( &j ) );
    CPPUNIT_ASSERT( hadron.isAccepted( &j ) );
		    
    j.set_pdgId( PDG::Omega_b_minus );
    CPPUNIT_ASSERT( filter.isAccepted( &j ) );
    CPPUNIT_ASSERT( hadron.isAccepted( &j ) );
		    
    for ( JetCollection::const_iterator jet = m_jets->begin();
	  jet != m_jets->end();
	  ++jet ) {
      CPPUNIT_ASSERT( !filter.isAccepted( *jet ) );
      CPPUNIT_ASSERT( !hadron.isAccepted( *jet ) );
   }

  }

  void testMesonFilter()
  {
    Jet j;

    CategoryIdFilter<IParticle> hadron;
    hadron.setCategory( PDGCat::Hadron );

    CategoryIdFilter<IParticle> filter;
    filter.setCategory( PDGCat::Meson );

    j.set_pdgId( PDG::pi0 );
    CPPUNIT_ASSERT( filter.isAccepted( &j ) );
    CPPUNIT_ASSERT( hadron.isAccepted( &j ) );
    
    j.set_pdgId( PDG::pi_plus );
    CPPUNIT_ASSERT( filter.isAccepted( &j ) );
    CPPUNIT_ASSERT( hadron.isAccepted( &j ) );
    
    j.set_pdgId( PDG::eta );
    CPPUNIT_ASSERT( filter.isAccepted( &j ) );
    CPPUNIT_ASSERT( hadron.isAccepted( &j ) );
    
    j.set_pdgId( PDG::rho_2S0 );
    CPPUNIT_ASSERT( filter.isAccepted( &j ) );
    CPPUNIT_ASSERT( hadron.isAccepted( &j ) );
    
    j.set_pdgId( PDG::phi );
    CPPUNIT_ASSERT( filter.isAccepted( &j ) );
    CPPUNIT_ASSERT( hadron.isAccepted( &j ) );
    
    j.set_pdgId( PDG::f_0 );
    CPPUNIT_ASSERT( filter.isAccepted( &j ) );
    CPPUNIT_ASSERT( hadron.isAccepted( &j ) );
    
    j.set_pdgId( PDG::K0 );
    CPPUNIT_ASSERT( filter.isAccepted( &j ) );
    CPPUNIT_ASSERT( hadron.isAccepted( &j ) );
    
    j.set_pdgId( PDG::K_0_star0 );
    CPPUNIT_ASSERT( filter.isAccepted( &j ) );
    CPPUNIT_ASSERT( hadron.isAccepted( &j ) );
    
    j.set_pdgId( PDG::D_plus );
    CPPUNIT_ASSERT( filter.isAccepted( &j ) );
    CPPUNIT_ASSERT( hadron.isAccepted( &j ) );
    
    j.set_pdgId( PDG::D0 );
    CPPUNIT_ASSERT( filter.isAccepted( &j ) );
    CPPUNIT_ASSERT( hadron.isAccepted( &j ) );
    
    j.set_pdgId( PDG::D_0_star_plus );
    CPPUNIT_ASSERT( filter.isAccepted( &j ) );
    CPPUNIT_ASSERT( hadron.isAccepted( &j ) );
    
    j.set_pdgId( PDG::D_prime_1_plus );
    CPPUNIT_ASSERT( filter.isAccepted( &j ) );
    CPPUNIT_ASSERT( hadron.isAccepted( &j ) );
    
    j.set_pdgId( PDG::D_s_plus );
    CPPUNIT_ASSERT( filter.isAccepted( &j ) );
    CPPUNIT_ASSERT( hadron.isAccepted( &j ) );
    
    j.set_pdgId( PDG::D_s1_plus );
    CPPUNIT_ASSERT( filter.isAccepted( &j ) );
    CPPUNIT_ASSERT( hadron.isAccepted( &j ) );
    
    j.set_pdgId( PDG::B0 );
    CPPUNIT_ASSERT( filter.isAccepted( &j ) );
    CPPUNIT_ASSERT( hadron.isAccepted( &j ) );
    
    j.set_pdgId( PDG::B_plus );
    CPPUNIT_ASSERT( filter.isAccepted( &j ) );
    CPPUNIT_ASSERT( hadron.isAccepted( &j ) );
    
    j.set_pdgId( PDG::B_0_star0 );
    CPPUNIT_ASSERT( filter.isAccepted( &j ) );
    CPPUNIT_ASSERT( hadron.isAccepted( &j ) );
    
    j.set_pdgId( PDG::B_10 );
    CPPUNIT_ASSERT( filter.isAccepted( &j ) );
    CPPUNIT_ASSERT( hadron.isAccepted( &j ) );
    
    j.set_pdgId( PDG::B_s0 );
    CPPUNIT_ASSERT( filter.isAccepted( &j ) );
    CPPUNIT_ASSERT( hadron.isAccepted( &j ) );
    
    j.set_pdgId( PDG::B_s0_star0 );
    CPPUNIT_ASSERT( filter.isAccepted( &j ) );
    CPPUNIT_ASSERT( hadron.isAccepted( &j ) );
    
    j.set_pdgId( PDG::B_c_plus );
    CPPUNIT_ASSERT( filter.isAccepted( &j ) );
    CPPUNIT_ASSERT( hadron.isAccepted( &j ) );
    
    j.set_pdgId( PDG::B_c0_star_plus );
    CPPUNIT_ASSERT( filter.isAccepted( &j ) );
    CPPUNIT_ASSERT( hadron.isAccepted( &j ) );
    
    j.set_pdgId( PDG::eta_c );
    CPPUNIT_ASSERT( filter.isAccepted( &j ) );
    CPPUNIT_ASSERT( hadron.isAccepted( &j ) );
    
    j.set_pdgId( PDG::J_psi );
    CPPUNIT_ASSERT( filter.isAccepted( &j ) );
    CPPUNIT_ASSERT( hadron.isAccepted( &j ) );
    
    j.set_pdgId( PDG::Upsilon );
    CPPUNIT_ASSERT( filter.isAccepted( &j ) );
    CPPUNIT_ASSERT( hadron.isAccepted( &j ) );
    
    for ( JetCollection::const_iterator jet = m_jets->begin();
	  jet != m_jets->end();
	  ++jet ) {
      CPPUNIT_ASSERT( !filter.isAccepted( *jet ) );
      CPPUNIT_ASSERT( !hadron.isAccepted( *jet ) );
    }

  }

  /// Test DiQuark filter
  void testDiQuarkFilter()
  {
    Jet j;

    CategoryIdFilter<IParticle> filter;
    filter.setCategory( PDGCat::DiQuark );

    CategoryIdFilter<IParticle> meson;
    meson.setCategory( PDGCat::Meson );

    CategoryIdFilter<IParticle> baryon;
    baryon.setCategory( PDGCat::Baryon );

    // stricto sensu this particle is not a diquark (not color neutral)
    // so it is NOT accepted by the filter.
    // Is it really worrying ?
    j.set_pdgId( PDG::dd_0 );
    CPPUNIT_ASSERT( !filter.isAccepted( &j ) );
    CPPUNIT_ASSERT( !meson.isAccepted( &j ) );
    CPPUNIT_ASSERT( !baryon.isAccepted( &j ) );

    j.set_pdgId( PDG::ud_0 );
    CPPUNIT_ASSERT( filter.isAccepted( &j ) );
    CPPUNIT_ASSERT( !meson.isAccepted( &j ) );
    CPPUNIT_ASSERT( !baryon.isAccepted( &j ) );

    // stricto sensu this particle is not a diquark (not color neutral)
    // so it is NOT accepted by the filter.
    // Is it really worrying ?
    j.set_pdgId( PDG::uu_0 );
    CPPUNIT_ASSERT( !filter.isAccepted( &j ) );
    CPPUNIT_ASSERT( !meson.isAccepted( &j ) );
    CPPUNIT_ASSERT( !baryon.isAccepted( &j ) );

    j.set_pdgId( PDG::sd_0 );
    CPPUNIT_ASSERT( filter.isAccepted( &j ) );
    CPPUNIT_ASSERT( !meson.isAccepted( &j ) );
    CPPUNIT_ASSERT( !baryon.isAccepted( &j ) );

    j.set_pdgId( PDG::su_0 );
    CPPUNIT_ASSERT( filter.isAccepted( &j ) );
    CPPUNIT_ASSERT( !meson.isAccepted( &j ) );
    CPPUNIT_ASSERT( !baryon.isAccepted( &j ) );

    // stricto sensu this particle is not a diquark (not color neutral)
    // so it is NOT accepted by the filter.
    // Is it really worrying ?
    j.set_pdgId( PDG::ss_0 );
    CPPUNIT_ASSERT( !filter.isAccepted( &j ) );
    CPPUNIT_ASSERT( !meson.isAccepted( &j ) );
    CPPUNIT_ASSERT( !baryon.isAccepted( &j ) );

    j.set_pdgId( PDG::cd_0 );
    CPPUNIT_ASSERT( filter.isAccepted( &j ) );
    CPPUNIT_ASSERT( !meson.isAccepted( &j ) );
    CPPUNIT_ASSERT( !baryon.isAccepted( &j ) );

    j.set_pdgId( PDG::cu_0 );
    CPPUNIT_ASSERT( filter.isAccepted( &j ) );
    CPPUNIT_ASSERT( !meson.isAccepted( &j ) );
    CPPUNIT_ASSERT( !baryon.isAccepted( &j ) );

    j.set_pdgId( PDG::cs_0 );
    CPPUNIT_ASSERT( filter.isAccepted( &j ) );
    CPPUNIT_ASSERT( !meson.isAccepted( &j ) );
    CPPUNIT_ASSERT( !baryon.isAccepted( &j ) );

    // stricto sensu this particle is not a diquark (not color neutral)
    // so it is NOT accepted by the filter.
    // Is it really worrying ?
    j.set_pdgId( PDG::cc_0 );
    CPPUNIT_ASSERT( !filter.isAccepted( &j ) );
    CPPUNIT_ASSERT( !meson.isAccepted( &j ) );
    CPPUNIT_ASSERT( !baryon.isAccepted( &j ) );

    j.set_pdgId( PDG::bd_0 );
    CPPUNIT_ASSERT( filter.isAccepted( &j ) );
    CPPUNIT_ASSERT( !meson.isAccepted( &j ) );
    CPPUNIT_ASSERT( !baryon.isAccepted( &j ) );

    j.set_pdgId( PDG::bu_0 );
    CPPUNIT_ASSERT( filter.isAccepted( &j ) );
    CPPUNIT_ASSERT( !meson.isAccepted( &j ) );
    CPPUNIT_ASSERT( !baryon.isAccepted( &j ) );

    j.set_pdgId( PDG::bs_0 );
    CPPUNIT_ASSERT( filter.isAccepted( &j ) );
    CPPUNIT_ASSERT( !meson.isAccepted( &j ) );
    CPPUNIT_ASSERT( !baryon.isAccepted( &j ) );

    j.set_pdgId( PDG::bc_0 );
    CPPUNIT_ASSERT( filter.isAccepted( &j ) );
    CPPUNIT_ASSERT( !meson.isAccepted( &j ) );
    CPPUNIT_ASSERT( !baryon.isAccepted( &j ) );

    // stricto sensu this particle is not a diquark (not color neutral)
    // so it is NOT accepted by the filter.
    // Is it really worrying ?
    j.set_pdgId( PDG::bb_0 );
    CPPUNIT_ASSERT( !filter.isAccepted( &j ) );
    CPPUNIT_ASSERT( !meson.isAccepted( &j ) );
    CPPUNIT_ASSERT( !baryon.isAccepted( &j ) );

    j.set_pdgId( PDG::dd_1 );
    CPPUNIT_ASSERT( filter.isAccepted( &j ) );
    CPPUNIT_ASSERT( !meson.isAccepted( &j ) );
    CPPUNIT_ASSERT( !baryon.isAccepted( &j ) );

    j.set_pdgId( PDG::ud_1 );
    CPPUNIT_ASSERT( filter.isAccepted( &j ) );
    CPPUNIT_ASSERT( !meson.isAccepted( &j ) );
    CPPUNIT_ASSERT( !baryon.isAccepted( &j ) );

    j.set_pdgId( PDG::uu_1 );
    CPPUNIT_ASSERT( filter.isAccepted( &j ) );
    CPPUNIT_ASSERT( !meson.isAccepted( &j ) );
    CPPUNIT_ASSERT( !baryon.isAccepted( &j ) );

    j.set_pdgId( PDG::ss_1 );
    CPPUNIT_ASSERT( filter.isAccepted( &j ) );
    CPPUNIT_ASSERT( !meson.isAccepted( &j ) );
    CPPUNIT_ASSERT( !baryon.isAccepted( &j ) );

    j.set_pdgId( PDG::cc_1 );
    CPPUNIT_ASSERT( filter.isAccepted( &j ) );
    CPPUNIT_ASSERT( !meson.isAccepted( &j ) );
    CPPUNIT_ASSERT( !baryon.isAccepted( &j ) );

    j.set_pdgId( PDG::bb_1 );
    CPPUNIT_ASSERT( filter.isAccepted( &j ) );
    CPPUNIT_ASSERT( !meson.isAccepted( &j ) );
    CPPUNIT_ASSERT( !baryon.isAccepted( &j ) );

    for ( JetCollection::const_iterator jet = m_jets->begin();
	  jet != m_jets->end();
	  ++jet ) {
      CPPUNIT_ASSERT( !filter.isAccepted( *jet ) );
    }
  }

  /// Test Boson filter
  void testBosonFilter()
  {
    Jet j;

    CategoryIdFilter<IParticle> filter;
    filter.setCategory( PDGCat::Boson );

    j.set_pdgId( static_cast<PDG::pidType>(21) ); //> gluon
    CPPUNIT_ASSERT( filter.isAccepted( &j ) );

    j.set_pdgId( PDG::gamma );
    CPPUNIT_ASSERT( filter.isAccepted( &j ) );

    j.set_pdgId( PDG::Z0 );
    CPPUNIT_ASSERT( filter.isAccepted( &j ) );

    j.set_pdgId( PDG::W_plus );
    CPPUNIT_ASSERT( filter.isAccepted( &j ) );

    j.set_pdgId( PDG::Higgs0 );
    CPPUNIT_ASSERT( filter.isAccepted( &j ) );

    j.set_pdgId( PDG::reggeon );
    CPPUNIT_ASSERT( filter.isAccepted( &j ) );

    j.set_pdgId( PDG::pomeron );
    CPPUNIT_ASSERT( filter.isAccepted( &j ) );

    j.set_pdgId( PDG::Z_prime0 );
    CPPUNIT_ASSERT( filter.isAccepted( &j ) );

    j.set_pdgId( PDG::Z_prime_prime0 );
    CPPUNIT_ASSERT( filter.isAccepted( &j ) );

    j.set_pdgId( PDG::W_prime_plus );
    CPPUNIT_ASSERT( filter.isAccepted( &j ) );

    j.set_pdgId( PDG::Higgs_prime0 );
    CPPUNIT_ASSERT( filter.isAccepted( &j ) );

    j.set_pdgId( PDG::A0 );
    CPPUNIT_ASSERT( filter.isAccepted( &j ) );

    j.set_pdgId( PDG::Higgs_plus );
    CPPUNIT_ASSERT( filter.isAccepted( &j ) );

    for ( JetCollection::const_iterator jet = m_jets->begin();
	  jet != m_jets->end();
	  ++jet ) {
      CPPUNIT_ASSERT( !filter.isAccepted( *jet ) );
    }
  }
};

/// Registration of the test suite "CategoryIdFilterTest" so it will be 
/// recognized by the main test program which drives the different tests
CPPUNIT_TEST_SUITE_REGISTRATION( CategoryIdFilterTest );

/// CppUnit test-driver common for all the cppunit test classes.
/// In ATLAS sw release it is located in TestPolicy package
#include <TestPolicy/CppUnit_SGtestdriver.cxx>
