/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/// @purpose: test the TruthParticle class
/// @author : S.Binet<binet@cern.ch>

// STL includes
#include <cmath>
#include <cassert>
#include <stdexcept>
#include <string>
#include <sstream>
#include <iomanip>
#include <iostream>
#include <vector>

// Framework includes
#ifndef NOGAUDI
#include "TestTools/initGaudi.h"
#endif //NOGAUDI

#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/IService.h"

#include "StoreGate/WriteHandle.h"

// CLHEP includes
#include "CLHEP/Units/SystemOfUnits.h"
#include "GeneratorObjects/McEventCollection.h"
#include "AtlasHepMC/GenEvent.h"
#include "AtlasHepMC/GenVertex.h"
#include "AtlasHepMC/GenParticle.h"

#include "GeneratorObjects/HepMcParticleLink.h"

// McParticleEvent includes
#include "McParticleEvent/TruthParticleContainer.h"
#include "McParticleEvent/TruthParticle.h"
#include "McParticleEvent/TruthEtIsolations.h"

class tp_test_err
  : public std::exception
{
public:
  explicit tp_test_err (const char* file,
                        int line,
                        const char* what);
  virtual ~tp_test_err() throw() {}
  virtual const char* what() const throw() { return m_what.c_str(); }
private:
  std::string m_what;
};


tp_test_err::tp_test_err (const char* file,
                          int line,
                          const char* what)
{
  std::ostringstream os;
  os << file << ":" << line << " myassertion failure: " << what;
  m_what = os.str();
}


void throw_tp_test_err (const char* file, int line, const char* what)
{
  throw (tp_test_err (file, line, what));
}

#define myassert(X) do {     \
    if (!(X)) { \
      throw_tp_test_err (__FILE__, __LINE__, #X); \
    } \
  } while (0)

#define TP_ASSERT(X) myassert(X)

typedef HepMC::FourVector HLV_t;
typedef TruthParticleContainer::Map_t Map_t;
typedef TruthEtIsolations::EtIsol_t EtIsol_t;

// helper to fill the TruthParticleContainer internal map[extended-bc -> truthpart]
//  note that it is only working for the case where there is only 1 GenEvent...
#ifdef HEPMC3
Map_t::value_type 
make_map_t_pair(const HepMC::GenParticlePtr &p,
                const TruthParticle &tp)
{
  const std::size_t genEventIdx = 0;
  HepMcParticleLink link(HepMC::barcode(p), genEventIdx);
  return Map_t::value_type(link.compress(), &tp);
}
bool operator==(TruthParticle a, HepMC::GenParticlePtr b)
{
if (!a.genParticle() && !b) return true;
if (a.genParticle() && !b) return false;
if (!a.genParticle() && b) return false;
return (a.genParticle().get() == b.get());
}
#else
Map_t::value_type 
make_map_t_pair(const HepMC::GenParticle &p,
                const TruthParticle &tp)
{
  const std::size_t genEventIdx = 0;
  HepMcParticleLink link(p.barcode(), genEventIdx, EBC_MAINEVCOLL, HepMcParticleLink::IS_POSITION);
  return Map_t::value_type(link.compress(), &tp);
}
#endif


class TruthParticleProt
  : public TruthParticle
{
public:
  using TruthParticle::TruthParticle;
  using TruthParticle::m_container;
};


class TruthParticleTest
{
public:

  HepMC::GenEvent * m_evt;
  HepMC::GenVertexPtr m_vtx;
  HepMC::GenParticlePtr m_top;
  HepMC::GenParticlePtr m_w;
  HepMC::GenParticlePtr m_b;
  HepMC::GenParticlePtr m_g1;
  HepMC::GenParticlePtr m_g2;

  unsigned int m_nPartsIn;
  unsigned int m_nPartsOut;

  unsigned int m_nCones;
  EtIsol_t m_etIsols;

  double            m_epsilon;

  TruthParticleContainer * m_mc;
};

TruthParticleTest* makeTestData()
{
  TruthParticleTest * test = new TruthParticleTest;

  const int signalProcessId = 1000082;
  const int evtNbr = 1;
  HepMC::GenEvent * evt = HepMC::newGenEvent( signalProcessId, evtNbr );
  test->m_evt = evt;

#ifdef HEPMC3
// This is how the attribute can be set. But in HepMC3 meaningless attributes hsould be avoided.
//  evt->add_attribute("alphaQCD",std::make_shared<HepMC3::DoubleAttribute>(-1));

  std::vector<double> weights(3);
  weights[0] = 1;
  weights[1] = 1;
  weights[2] = 1;
  std::vector<long> rdmStates(2);
  rdmStates[0] = 85909879;
  rdmStates[1] = 9707499;
  evt->weights() = weights;
  evt->add_attribute("random_states",std::make_shared<HepMC3::VectorLongIntAttribute>(rdmStates));
#else
  evt->set_event_scale( -1 );
  evt->set_alphaQCD( -1 );
  evt->set_alphaQED( -1 );

  std::vector<double> weights(3);
  weights[0] = 1;
  weights[1] = 1;
  weights[2] = 1;
  std::vector<long> rdmStates(2);
  rdmStates[0] = 85909879;
  rdmStates[1] = 9707499;
  evt->weights() = weights;
  evt->set_random_states( rdmStates );
#endif
    
  // Add a t->W+bgg
  HepMC::GenVertexPtr vtx = HepMC::newGenVertexPtr();

  evt->add_vertex( vtx );
  // top
  HepMC::GenParticlePtr top = 
  HepMC::newGenParticlePtr( HLV_t(-2.35e+05,
				      +7.34e+04,
				      +3.60e+04,
				      +3.04e+05),
				6, 3 );
  vtx->add_particle_in( top );
  

  // Wbgg
  HepMC::GenParticlePtr w = 
  HepMC::newGenParticlePtr( HLV_t(-1.09e+05,
				    +6.99e+04,
				    -3.86e+04,
				    +1.57e+05),
			      24, 2 ) ;
  vtx->add_particle_out(w);

  HepMC::GenParticlePtr b = 
  HepMC::newGenParticlePtr( HLV_t(-9.23e+04,
				    +2.54e+03,
				    +5.32e+04,
				    +1.07e+05),
			      5, 2 );
  vtx->add_particle_out(b);

  HepMC::GenParticlePtr g1 =
  HepMC::newGenParticlePtr( HLV_t(-4.76e+03,
				     +6.72e+02,
				     +2.90e+03,
				     +5.62e+03),
			       21, 2 );
  vtx->add_particle_out( g1 );

  HepMC::GenParticlePtr g2 =
  HepMC::newGenParticlePtr( HLV_t(-2.93e+04,
				     +2.13e+02,
				     +1.85e+04,
				     +3.46e+04),
			       21, 2 );
  vtx->add_particle_out( g2 );

  SG::WriteHandle<McEventCollection> inputTestDataHandle{"GEN_AOD"};
  inputTestDataHandle = std::make_unique<McEventCollection>();
  inputTestDataHandle->push_back( evt );
  McEventCollection * genEvt = &*inputTestDataHandle;

  // filling Data test members
  test->m_evt        = evt;
  test->m_vtx        = vtx;
  test->m_top        = top;
  test->m_w          = w;
  test->m_b          = b;
  test->m_g1         = g1;
  test->m_g2         = g2;
  // we subtract one because we don't account for the top
#ifdef HEPMC3
  test->m_nPartsIn   = vtx->particles_in().size() - 1; 
  test->m_nPartsOut  = vtx->particles_out().size();
#else
  test->m_nPartsIn   = vtx->particles_in_size() - 1; 
  test->m_nPartsOut  = vtx->particles_out_size();
#endif

  test->m_nCones = TruthParticleParameters::NbrOfCones;
  for ( std::size_t i = 0; 
	i != static_cast<std::size_t>(test->m_nCones); 
	++i ) {
    test->m_etIsols[i] = 0.*CLHEP::GeV;
  }

  TruthParticleContainer * mc = new TruthParticleContainer;
  ElementLink<McEventCollection> genEvtLink( *genEvt, 0 );
  mc->setGenEvent( genEvtLink );
  
  test->m_mc = mc;
  test->m_epsilon = 1e-10;

  return test;
}

void testConstructor( TruthParticleTest* tp )
{
  {
    TruthParticleProt mc;

    // FourMomentum
    TP_ASSERT( mc.px () == 0 );
    TP_ASSERT( mc.py () == 0 );
    TP_ASSERT( mc.pz () == 0 );
    TP_ASSERT( mc.e  () == 0 );
    
    // TruthParticle-part
    TP_ASSERT( mc.genParticle() == 0   );
    TP_ASSERT( mc.m_container == 0   );
    TP_ASSERT( mc.nParents() == 0  );
    TP_ASSERT( mc.nDecay() == 0 );
  }

  {
    TruthParticleProt mc( tp->m_top, tp->m_mc );
    // FourMomentum
    TP_ASSERT( mc.px () == tp->m_top->momentum().px() );
    TP_ASSERT( mc.py () == tp->m_top->momentum().py() );
    TP_ASSERT( mc.pz () == tp->m_top->momentum().pz() );
    TP_ASSERT( mc.e  () == tp->m_top->momentum().e () );
    
    // TruthParticle-part
    TP_ASSERT( mc.genParticle() == tp->m_top );
    TP_ASSERT( mc.m_container == tp->m_mc  );
    TP_ASSERT( mc.nParents()  == tp->m_nPartsIn  );
    TP_ASSERT( mc.nDecay() == tp->m_nPartsOut );
  }

  return;
}

void testCopyConstructor( TruthParticleTest* tp )
{
  TruthParticleProt top( tp->m_top, tp->m_mc );
  {
    TruthParticleProt mc(top);
    // FourMomentum
    TP_ASSERT( std::abs( mc.px () - top.px() ) < tp->m_epsilon );
    TP_ASSERT( std::abs( mc.py () - top.py() ) < tp->m_epsilon );
    TP_ASSERT( std::abs( mc.pz () - top.pz() ) < tp->m_epsilon );
    TP_ASSERT( std::abs( mc.e  () - top.e()  ) < tp->m_epsilon );
    TP_ASSERT( std::abs( mc.m()   - top.m()  ) < tp->m_epsilon );
    TP_ASSERT( mc.hlv().isNear(top.hlv(), tp->m_epsilon) );

    // TruthParticle-part
    TP_ASSERT( mc.genParticle() == top.genParticle() );
    TP_ASSERT( mc.m_container == top.m_container  );
    TP_ASSERT( mc.nParents()  == top.nParents()  );
    TP_ASSERT( mc.nDecay() == top.nDecay() );
  }
  return;
}

void testSettersAndGetters( TruthParticleTest* tp )
{
  const std::size_t refWIdx = 0;
  const std::size_t refBIdx = 1;
  const std::size_t refGluon1Idx = 2;
  const std::size_t refGluon2Idx = 3;
  const std::size_t refTopIdx = 0;

  TruthParticle top( tp->m_top, tp->m_mc );
  TruthParticle w  ( tp->m_w,   tp->m_mc );
  TruthParticle b  ( tp->m_b,   tp->m_mc );
  TruthParticle g1 ( tp->m_g1,  tp->m_mc );
  TruthParticle g2 ( tp->m_g2,  tp->m_mc );
  {
    TruthParticle mc( tp->m_w, tp->m_mc );
#ifdef HEPMC3
    Map_t parts;
    parts.insert( make_map_t_pair( tp->m_w,   mc  ) );
    parts.insert( make_map_t_pair( tp->m_top, top ) );
    tp->m_mc->setParticles( parts );
#else
    Map_t parts;
    parts.insert( make_map_t_pair( *tp->m_w,   mc  ) );
    parts.insert( make_map_t_pair( *tp->m_top, top ) );
    tp->m_mc->setParticles( parts );
#endif

    TP_ASSERT( mc.genMother()          == tp->m_top );
    TP_ASSERT( mc.genMother(refTopIdx) == tp->m_top );
    TP_ASSERT( mc.mother()             == &top );
    TP_ASSERT( mc.mother(refTopIdx)    == &top );

    bool caught = false;
    try {
      mc.genChild(0);
    } catch ( std::out_of_range& err ) {
      caught = true;
    }
    TP_ASSERT( caught );
    TP_ASSERT( mc.child(0) == 0 );

    TP_ASSERT( mc.hasMother( PDG::t ) );
    {
      std::vector<unsigned int> indices;
      TP_ASSERT( mc.hasMother( PDG::t, indices ) );
      TP_ASSERT( indices.size() == 1 );
      TP_ASSERT( indices[0]     == 0 );
    }
  }
  {
    TruthParticle mc( tp->m_top, tp->m_mc );
#ifdef HEPMC3
    Map_t parts;
    parts.insert( make_map_t_pair( tp->m_top, mc ) );
    parts.insert( make_map_t_pair( tp->m_w,   w  ) );
    parts.insert( make_map_t_pair( tp->m_b,   b  ) );
    parts.insert( make_map_t_pair( tp->m_g1,  g1 ) );
    parts.insert( make_map_t_pair( tp->m_g2,  g2 ) );
    tp->m_mc->setParticles( parts );
#else
    Map_t parts;
    parts.insert( make_map_t_pair( *tp->m_top, mc ) );
    parts.insert( make_map_t_pair( *tp->m_w,   w  ) );
    parts.insert( make_map_t_pair( *tp->m_b,   b  ) );
    parts.insert( make_map_t_pair( *tp->m_g1,  g1 ) );
    parts.insert( make_map_t_pair( *tp->m_g2,  g2 ) );
    tp->m_mc->setParticles( parts );
#endif

    bool caught = false;
    try {
      mc.genMother(0);
    } catch ( std::out_of_range& err ) {
      caught = true;
    }
    TP_ASSERT( caught );
    TP_ASSERT( mc.mother( ) == 0 );
    TP_ASSERT( mc.mother(0) == 0 );

#ifdef HEPMC3
    TP_ASSERT(  mc.genParticle() == tp->m_top );
#else
    TP_ASSERT( *mc.genParticle() == *tp->m_top );
#endif

    {
      // testing automatic cast to GenParticle
#ifdef HEPMC3
      TP_ASSERT( mc == tp->m_top );
#else
      const HepMC::GenParticle& hepMc = mc;
      TP_ASSERT( hepMc == *tp->m_top );
#endif
    }

    for ( unsigned int i = 0; i != mc.nDecay(); ++i ) {
      caught = false;
      try {
	TP_ASSERT( mc.genChild(i) != 0 );
      } catch ( std::out_of_range& err ) {
	caught = true;
      }
      TP_ASSERT( !caught );
    }

    for ( unsigned int i = 0; i != mc.nDecay(); ++i ) {
      caught = false;
      try {
	TP_ASSERT( mc.child(i) != 0 );
      } catch ( std::out_of_range& err ) {
	caught = true;
      }
      TP_ASSERT( !caught );
    }

    TP_ASSERT( mc.status() == tp->m_top->status() );
    TP_ASSERT( mc.hasChild( PDG::b ) );
    TP_ASSERT( mc.hasChild( PDG::W_plus ) );
    TP_ASSERT( mc.hasChild( PDG::g ) );

    {
      std::vector<unsigned int> indices;
      TP_ASSERT( mc.hasChild( PDG::W_plus, indices ) );
      TP_ASSERT( indices.size() == 1 );
      TP_ASSERT( indices[0]     == refWIdx );
      for ( unsigned int i = 0; i != indices.size(); ++i ) {
	std::cout << "indices[" << i << "]= " << indices[i] << std::endl;
      }
      for ( unsigned int i = 0; i != mc.nDecay(); ++i ) {
	TP_ASSERT( mc.genChild(i) != 0 );
	TP_ASSERT( mc.child(i)    != 0 );
	std::cout << "child[" << i << "]= " 
		  << mc.genChild(i)->pdg_id()
		  << "\t"
		  << mc.child(i)->pdgId() 
		  << std::endl;
	TP_ASSERT( mc.child(i)->pdgId() == mc.genChild(i)->pdg_id() );
      }
    }
    {
      std::vector<unsigned int> indices;
      TP_ASSERT( mc.hasChild( PDG::b, indices ) );
      TP_ASSERT( indices.size() == 1 );
      TP_ASSERT( indices[0]     == refBIdx );
    }
    {
      std::vector<unsigned int> indices;
      TP_ASSERT( mc.hasChild( PDG::g, indices ) );
      TP_ASSERT( indices.size() == 2 );
      TP_ASSERT( indices[0]     == refGluon1Idx );
      TP_ASSERT( indices[1]     == refGluon2Idx );
    }
      
    caught = false;
    try {
      double etIsol = mc.etIsol(TruthParticleParameters::NbrOfCones);
      if ( etIsol > 0. ) {
	// dummy
      }
    } catch ( std::out_of_range& err ) {
      caught = true;
    }
    TP_ASSERT( caught );
    CLHEP::HepLorentzVector mw_momentum_lv(tp->m_w->momentum().px(),
				 tp->m_w->momentum().py(),
				 tp->m_w->momentum().pz(),
				 tp->m_w->momentum().e());

    TP_ASSERT( mc.status()       == tp->m_top->status() );
#ifdef HEPMC3
//Add the comparison here?
#else
    TP_ASSERT( mc.flow()         == tp->m_top->flow() );
    TP_ASSERT( mc.polarization() == tp->m_top->polarization() );
#endif
    TP_ASSERT( mc.barcode()      == HepMC::barcode(tp->m_top) );

    TP_ASSERT( mc.nParents()        == tp->m_nPartsIn  );
    TP_ASSERT( mc.nDecay()          == tp->m_nPartsOut );
    TP_ASSERT( mc.pDecay(refWIdx)   == mw_momentum_lv );
    TP_ASSERT( mc.pdgDecay(refWIdx) == tp->m_w->pdg_id() );

    std::vector<double> etIsols;
    for ( unsigned int i = 0; 
	  i != static_cast<unsigned int>(TruthParticleParameters::NbrOfCones);
	  ++i ) {
      etIsols.push_back( i*2.*CLHEP::GeV );
    }
  }
  
  return;
}

void test( TruthParticleTest* tp )
{
  testConstructor(tp);
  testCopyConstructor(tp);
  testSettersAndGetters(tp);
  return;
}

int main()
{
  // First instantiates a stripped version of Gaudi
  ISvcLocator* pSvcLoc;
  if (!Athena_test::initGaudi("TruthParticle_test.txt", pSvcLoc)) {
    std::cerr << " This test cannot be run without init Gaudi" << std::endl;
  }
  assert(pSvcLoc);

  // Load an instance of StoreGateSvc because we'll need it to store objects
  IService * pStoreGate = 0;
  static const bool CREATE(true);
  bool sc = ( pSvcLoc->service( "StoreGateSvc", 
				 pStoreGate, CREATE) ).isSuccess();
  assert( sc );
  assert( 0 != pStoreGate );

  std::cout << " *** TruthParticle test in progress: " << std::endl;
  
  std::cout << "Build test data..." << std::endl;

  TruthParticleTest * truthParticleTest = makeTestData();
  std::cout << "Launching tests..." << std::endl;
  test(truthParticleTest);

  std::cout << "Done [OK]" << std::endl;
  return EXIT_SUCCESS;
}
