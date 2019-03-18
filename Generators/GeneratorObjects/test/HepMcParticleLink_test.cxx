/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @author John Chapman
 * @date June, 2016
 */

#undef NDEBUG

#include <iostream>
#include <list>
// Framework
#include "TestTools/initGaudi.h"

// ATLAS C++
#include "CxxUtils/make_unique.h"

// Google Test
#include "gtest/gtest.h"

// HepMC includes
#include "HepMC/GenEvent.h"

// CLHEP includes
#include "CLHEP/Vector/LorentzVector.h"
#include "CLHEP/Units/SystemOfUnits.h"

// Athena headers
#include "CxxUtils/make_unique.h"
#include "StoreGate/WriteHandle.h"
#include "StoreGate/StoreGateSvc.h"
#include "GeneratorObjects/McEventCollection.h"
#include "GeneratorObjects/HepMcParticleLink.h"

namespace MCTesting {

  // needed every time an AthAlgorithm, AthAlgTool or AthService is instantiated
  ISvcLocator* g_svcLoc = nullptr;

  // global test environment takes care of setting up Gaudi
  class GaudiEnvironment : public ::testing::Environment {
  protected:
    virtual void SetUp() override {
      Athena_test::initGaudi(MCTesting::g_svcLoc);
    }
  };

  class HepMcParticleLink_test : public ::testing::Test {

  protected:
    virtual void SetUp() override {
    }

    virtual void TearDown() override {
    }
  };

  //cut-and-paste from HepMC/examples/example_BuildEventFromScratch.cxx
  HepMC::GenEvent* buildEvent() {
    //
    // In this example we will place the following event into HepMC "by hand"
    //
    //     name status pdg_id  parent Px       Py    Pz       Energy      Mass
    //  1  !p+!    3   2212    0,0    0.000    0.000 7000.000 7000.000    0.938
    //  2  !p+!    3   2212    0,0    0.000    0.000-7000.000 7000.000    0.938
    //=========================================================================
    //  3  !d!     3      1    1,1    0.750   -1.569   32.191   32.238    0.000
    //  4  !u~!    3     -2    2,2   -3.047  -19.000  -54.629   57.920    0.000
    //  5  !W-!    3    -24    1,2    1.517   -20.68  -20.605   85.925   80.799
    //  6  !gamma! 1     22    1,2   -3.813    0.113   -1.833    4.233    0.000
    //  7  !d!     1      1    5,5   -2.445   28.816    6.082   29.552    0.010
    //  8  !u~!    1     -2    5,5    3.962  -49.498  -26.687   56.373    0.006

    // // first we construct a ParticleDataTable with all the particles we need
    // ParticleDataTable pdt("my particle data table");
    // // create a particle data entry for the proton and add it to pdt at the
    // // same time
    // pdt.insert( new ParticleData( "p+", 2212,   +1, 0.938,  -1, .5 ) );
    // pdt.insert( new ParticleData( "d",  1,  -2./3., 0,      -1, .5 ) );
    // pdt.insert( new ParticleData( "u~", -2, -1./3., 0,      -1, .5 ) );
    // pdt.insert( new ParticleData( "W-", -24,    -1, 80.396,
    //                    clifetime_from_width(2.06), 1 )    );
    // pdt.insert( new ParticleData( "gamma", 22,   0, 0,      -1, 1  ) );

    // // print out the GenParticle Data to the screen
    // //    pdt.print();

    // First create the event container, with Signal Process 20, event number 1
    HepMC::GenEvent* evt = new HepMC::GenEvent( 20, 1 );
    //
    // create vertex 1 and vertex 2, together with their inparticles
    HepMC::GenVertex* v1 = new HepMC::GenVertex();
    evt->add_vertex( v1 );
    v1->add_particle_in( new HepMC::GenParticle( CLHEP::HepLorentzVector(0,0,7000,7000),
                                                 2212, 3 ) );
    HepMC::GenVertex* v2 = new HepMC::GenVertex();
    evt->add_vertex( v2 );
    v2->add_particle_in( new HepMC::GenParticle( CLHEP::HepLorentzVector(0,0,-7000,7000),
                                                 2212, 3 ) );
    //
    // create the outgoing particles of v1 and v2
    HepMC::GenParticle* p3 =
      new HepMC::GenParticle( CLHEP::HepLorentzVector(.750,-1.569,32.191,32.238), 1, 3 );
    v1->add_particle_out( p3 );
    HepMC::GenParticle* p4 =
      new HepMC::GenParticle( CLHEP::HepLorentzVector(-3.047,-19.,-54.629,57.920), -2, 3 );
    v2->add_particle_out( p4 );
    //
    // create v3
    HepMC::GenVertex* v3 = new HepMC::GenVertex();
    evt->add_vertex( v3 );
    v3->add_particle_in( p3 );
    v3->add_particle_in( p4 );
    v3->add_particle_out(
                         new HepMC::GenParticle( CLHEP::HepLorentzVector(-3.813,0.113,-1.833,4.233 ), 22, 1 )
                         );
    HepMC::GenParticle* p5 =
      new HepMC::GenParticle( CLHEP::HepLorentzVector(1.517,-20.68,-20.605,85.925), -24,3);
    v3->add_particle_out( p5 );
    //
    // create v4
    HepMC::GenVertex* v4 = new HepMC::GenVertex();
    evt->add_vertex( v4 );
    v4->add_particle_in( p5 );
    v4->add_particle_out(
                         new HepMC::GenParticle( CLHEP::HepLorentzVector(-2.445,28.816,6.082,29.552), 1,1 )
                         );
    v4->add_particle_out(
                         new HepMC::GenParticle( CLHEP::HepLorentzVector(3.962,-49.498,-26.687,56.373), -2,1 )
                         );
    //
    // tell the event which vertex is the signal process vertex
    evt->set_signal_process_vertex( v3 );
    // the event is complete, we now print it out to the screen
#ifdef GENP_DEBUG
    evt->print();
#endif

    // delete all particle data objects in the particle data table pdt
    //pdt.delete_all();
    return evt;
  }

  class is_photon {
  public:
    bool operator() ( const HepMC::GenParticle* p ) {
      return ( p && p->pdg_id() == 22 );
    }
  };


  HepMC::GenParticle* populateGenEvent(HepMC::GenEvent & ge)
  {
    CLHEP::HepLorentzVector myPos( 0.0, 0.0, 0.0, 0.0);
    HepMC::GenVertex *myVertex = new HepMC::GenVertex( myPos, -1 );
    HepMC::FourVector fourMomentum1( 0.0, 0.0, 1.0, 1.0*CLHEP::TeV);
    HepMC::GenParticle* inParticle1 = new HepMC::GenParticle(fourMomentum1, 2, 10);
    myVertex->add_particle_in(inParticle1);
    HepMC::FourVector fourMomentum2( 0.0, 0.0, -1.0, 1.0*CLHEP::TeV);
    HepMC::GenParticle* inParticle2 = new HepMC::GenParticle(fourMomentum2, -2, 10);
    myVertex->add_particle_in(inParticle2);
    HepMC::FourVector fourMomentum3( 0.0, 1.0, 0.0, 1.0*CLHEP::TeV);
    HepMC::GenParticle* inParticle3 = new HepMC::GenParticle(fourMomentum3, 2, 10);
    myVertex->add_particle_out(inParticle3);
    HepMC::FourVector fourMomentum4( 0.0, -1.0, 0.0, 1.0*CLHEP::TeV);
    HepMC::GenParticle* inParticle4 = new HepMC::GenParticle(fourMomentum4, -2, 10);
    myVertex->add_particle_out(inParticle4);
    ge.add_vertex( myVertex );
    ge.set_signal_process_vertex( myVertex );
    ge.set_beam_particles(inParticle1,inParticle2);
    return inParticle3;
  }

  HepMC::GenParticle* populateGenEvent2(HepMC::GenEvent & ge)
  {
    CLHEP::HepLorentzVector myPos( 0.0, 0.0, 0.0, 0.0);
    HepMC::GenVertex *myVertex = new HepMC::GenVertex( myPos, -1 );
    HepMC::FourVector fourMomentum1( 0.0, 0.0, 1.0, 1.0*CLHEP::TeV);
    HepMC::GenParticle* inParticle1 = new HepMC::GenParticle(fourMomentum1, 2, 10);
    myVertex->add_particle_in(inParticle1);
    HepMC::FourVector fourMomentum2( 0.0, 0.0, -1.0, 1.0*CLHEP::TeV);
    HepMC::GenParticle* inParticle2 = new HepMC::GenParticle(fourMomentum2, -2, 10);
    myVertex->add_particle_in(inParticle2);
    HepMC::FourVector fourMomentum3( 0.0, 1.0, 0.0, 1.0*CLHEP::TeV);
    HepMC::GenParticle* inParticle3 = new HepMC::GenParticle(fourMomentum3, 2, 10);
    myVertex->add_particle_out(inParticle3);
    HepMC::FourVector fourMomentum4( 0.0, -1.0, 0.0, 1.0*CLHEP::TeV);
    HepMC::GenParticle* inParticle4 = new HepMC::GenParticle(fourMomentum4, -2, 10);
    myVertex->add_particle_out(inParticle4);
    ge.add_vertex( myVertex );
    ge.set_signal_process_vertex( myVertex );
    ge.set_beam_particles(inParticle1,inParticle2);
    return inParticle4;
  }

  TEST_F(HepMcParticleLink_test, old_test) {
    std::cout << "*** HepMcParticleLink_test starts ***" <<std::endl;

    SG::WriteHandle<McEventCollection> inputTestDataHandle{"TruthEvent"};
    inputTestDataHandle = CxxUtils::make_unique<McEventCollection>();

    HepMC::GenEvent* pEvent(buildEvent());
    inputTestDataHandle->push_back(pEvent);
    inputTestDataHandle->push_back(buildEvent());  //a copy
    std::list<HepMC::GenParticle*> theGammas;
    HepMC::copy_if( pEvent->particles_begin(), pEvent->particles_end(),
                    back_inserter(theGammas), is_photon() );
    ASSERT_EQ(1u, theGammas.size());
    const HepMC::GenParticle* pGamma(theGammas.front());
#ifdef GENP_DEBUG
    pGamma->print();
    std::cout << "gamma barcode " << hex << pGamma->barcode() << std::endl;
#endif

    HepMcParticleLink gammaLink1(pGamma);
    HepMcParticleLink gammaLink2(pGamma->barcode());
    HepMcParticleLink gammaLink11(pGamma, 1);
    HepMcParticleLink gammaLink12(pGamma->barcode(), 1);

    std::cout << "Testing HepMcParticleLink streamer "
         << gammaLink1 << " --- " << gammaLink11 <<std::endl;


#ifdef GENP_DEBUG
    std::cout << "link barcode " << hex << gammaLink1.barcode() << std::endl;
    std::cout << "link index " << hex << gammaLink1.eventIndex() << std::endl;
    std::cout << "link1 barcode " << hex << gammaLink11.barcode() << std::endl;
    std::cout << "link1 index " << hex << gammaLink11.eventIndex() << std::endl;
#endif
    ASSERT_EQ(*gammaLink1, *gammaLink2);
    ASSERT_EQ(gammaLink1, gammaLink2);
    ASSERT_EQ(gammaLink1.barcode(), pGamma->barcode());

    ASSERT_EQ(*gammaLink11, *gammaLink12);
    ASSERT_EQ(gammaLink11, gammaLink12);

    ASSERT_EQ(*gammaLink1, *gammaLink11);
    ASSERT_NE( gammaLink1, gammaLink11 ); //FIXME weird! Can't check ptr...
    ASSERT_LT( gammaLink1, gammaLink11 );  //FIXME weird! Can't check ptr...
    StoreGateSvc* pStore(nullptr);
    ASSERT_TRUE(MCTesting::g_svcLoc->service("StoreGateSvc", pStore).isSuccess());
    pStore->clearStore(true); // forceRemove=true to remove all proxies
    std::cout << "*** HepMcParticleLink_test OK ***" <<std::endl;
  }

  TEST_F(HepMcParticleLink_test, broken_event_link) {
    // create dummy input McEventCollection with a name that
    // HepMcParticleLink does not know about
    SG::WriteHandle<McEventCollection> inputTestDataHandle{"GEN_EVENT"};
    inputTestDataHandle = CxxUtils::make_unique<McEventCollection>();
    // Fill it with a dummy GenEvent
    inputTestDataHandle->push_back(new HepMC::GenEvent(20,1));
    HepMC::GenEvent& ge1 = *(inputTestDataHandle->at(0));
    const HepMC::GenParticle* particle1 = populateGenEvent(ge1);
    // A HepMcParticleLink built using a GenParticle pointer should
    // still work.
    HepMcParticleLink testLink1a(particle1,0);
    ASSERT_TRUE( testLink1a.isValid() );
    // A HepMcParticleLink built using the barcode and the position of
    // the GenEvent in the McEventCollection should still work.
    HepMcParticleLink testLink1b(particle1->barcode(),0);
    ASSERT_TRUE( testLink1b.isValid() );
    StoreGateSvc* pStore(nullptr);
    ASSERT_TRUE(MCTesting::g_svcLoc->service("StoreGateSvc", pStore).isSuccess());
    pStore->clearStore(true); // forceRemove=true to remove all proxies
  }

  TEST_F(HepMcParticleLink_test, truth_event_link) {
    // create dummy input McEventCollection with a name that
    // HepMcParticleLink knows about
    SG::WriteHandle<McEventCollection> inputTestDataHandle{"TruthEvent"};
    inputTestDataHandle = CxxUtils::make_unique<McEventCollection>();

    // Add a dummy GenEvent
    const int process_id1(20);
    const int event_number1(17);
    inputTestDataHandle->push_back(new HepMC::GenEvent(process_id1, event_number1));
    HepMC::GenEvent& ge1 = *(inputTestDataHandle->at(0));
    const HepMC::GenParticle* particle1 = populateGenEvent(ge1);
    // Add a second dummy GenEvent
    const int process_id2(20);
    const int event_number2(25);
    inputTestDataHandle->push_back(new HepMC::GenEvent(process_id2, event_number2));
    HepMC::GenEvent& ge2 = *(inputTestDataHandle->at(1));
    const HepMC::GenParticle* particle2 = populateGenEvent2(ge2);
    // Add a third dummy GenEvent (identical to the first)
    const int process_id3(20);
    const int event_number3(17);
    inputTestDataHandle->push_back(new HepMC::GenEvent(process_id3, event_number3));
    HepMC::GenEvent& ge3 = *(inputTestDataHandle->at(2));
    const HepMC::GenParticle* particle3 = populateGenEvent(ge3);

    //Testing links to the first dummy GenEvent
    const IProxyDict* sg = SG::CurrentEventStore::store();

    // HepMcParticleLink built using a GenParticle pointer and the
    // position of the GenEvent.
    HepMcParticleLink testLink1a(particle1,0, EBC_MAINEVCOLL,
                                 HepMcParticleLink::IS_POSITION);
    ASSERT_TRUE( testLink1a.isValid() );
    ASSERT_EQ( particle1->barcode(), testLink1a.barcode());
    ASSERT_EQ( event_number1, testLink1a.eventIndex());
    ASSERT_EQ( 0, testLink1a.getEventPositionInCollection(sg));
    ASSERT_EQ(particle1,testLink1a.cptr());
    // A HepMcParticleLink built using the barcode and the position of
    // the GenEvent.
    HepMcParticleLink testLink1b(particle1->barcode(),0, EBC_MAINEVCOLL,
                                 HepMcParticleLink::IS_POSITION);
    ASSERT_TRUE( testLink1b.isValid() );
    ASSERT_EQ( particle1->barcode(), testLink1b.barcode());
    ASSERT_EQ( event_number1, testLink1b.eventIndex());
    ASSERT_EQ( 0, testLink1b.getEventPositionInCollection(sg));
    ASSERT_EQ(*(testLink1a.cptr()),*(testLink1b.cptr()));
    // HepMcParticleLink built using a GenParticle pointer and the
    // event_number of the GenEvent.
    HepMcParticleLink testLink1c(particle1,event_number1);
    ASSERT_TRUE( testLink1c.isValid() );
    ASSERT_EQ( particle1->barcode(), testLink1c.barcode());
    ASSERT_NE(  0, testLink1c.eventIndex());
    ASSERT_EQ( event_number1, testLink1c.eventIndex());
    ASSERT_EQ( 0, testLink1c.getEventPositionInCollection(sg));
    ASSERT_EQ(particle1,testLink1c.cptr());
    // A HepMcParticleLink built using the barcode and the event_number of
    // the GenEvent.
    HepMcParticleLink testLink1d(particle1->barcode(),event_number1);
    ASSERT_TRUE( testLink1d.isValid() );
    ASSERT_EQ( particle1->barcode(), testLink1d.barcode());
    ASSERT_NE(  0, testLink1d.eventIndex());
    ASSERT_EQ( event_number1, testLink1d.eventIndex());
    ASSERT_EQ( 0, testLink1d.getEventPositionInCollection(sg));
    ASSERT_EQ(particle1,testLink1d.cptr());

    //Testing links to the second dummy GenEvent

    // HepMcParticleLink built using a GenParticle pointer and the
    // position of the GenEvent.
    HepMcParticleLink testLink2a(particle2,1, EBC_MAINEVCOLL,
                                 HepMcParticleLink::IS_POSITION);
    ASSERT_TRUE( testLink2a.isValid() );
    ASSERT_EQ( particle2->barcode(), testLink2a.barcode());
    ASSERT_EQ( event_number2, testLink2a.eventIndex());
    ASSERT_EQ( 1, testLink2a.getEventPositionInCollection(sg));
    ASSERT_EQ(particle2,testLink2a.cptr());
    // A HepMcParticleLink built using the barcode and the position of
    // the GenEvent.
    HepMcParticleLink testLink2b(particle2->barcode(),1, EBC_MAINEVCOLL,
                                 HepMcParticleLink::IS_POSITION);
    ASSERT_TRUE( testLink2b.isValid() );
    ASSERT_EQ( particle2->barcode(), testLink2b.barcode());
    ASSERT_EQ( event_number2, testLink2b.eventIndex());
    ASSERT_EQ( 1, testLink2b.getEventPositionInCollection(sg));
    ASSERT_EQ(particle2,testLink2b.cptr());
    // HepMcParticleLink built using a GenParticle pointer and the
    // event_number of the GenEvent.
    HepMcParticleLink testLink2c(particle2,event_number2);
    ASSERT_TRUE( testLink2c.isValid() );
    ASSERT_EQ( particle2->barcode(), testLink2c.barcode());
    ASSERT_NE(  1, testLink2c.eventIndex());
    ASSERT_EQ( event_number2, testLink2c.eventIndex());
    ASSERT_EQ( 1, testLink2c.getEventPositionInCollection(sg));
    ASSERT_EQ(particle2,testLink2c.cptr());
    // A HepMcParticleLink built using the barcode and the event_number of
    // the GenEvent.
    HepMcParticleLink testLink2d(particle2->barcode(),event_number2);
    ASSERT_TRUE( testLink2d.isValid() );
    ASSERT_EQ( particle2->barcode(), testLink2d.barcode());
    ASSERT_NE(  1, testLink2d.eventIndex());
    ASSERT_EQ( event_number2, testLink2d.eventIndex());
    ASSERT_EQ( 1, testLink2d.getEventPositionInCollection(sg));
    ASSERT_EQ(particle2,testLink2d.cptr());

    //Testing links to the third dummy GenEvent

    // HepMcParticleLink built using a GenParticle pointer and the
    // position of the GenEvent.
    HepMcParticleLink testLink3a(particle3,2, EBC_MAINEVCOLL,
                                 HepMcParticleLink::IS_POSITION);
    ASSERT_TRUE( testLink3a.isValid() );
    ASSERT_EQ( particle3->barcode(), testLink3a.barcode());
    ASSERT_EQ( event_number3, testLink3a.eventIndex());
    ASSERT_EQ(particle3,testLink3a.cptr());
    // A HepMcParticleLink built using the barcode and the position of
    // the GenEvent.
    HepMcParticleLink testLink3b(particle3->barcode(),2, EBC_MAINEVCOLL,
                                 HepMcParticleLink::IS_POSITION);
    ASSERT_TRUE( testLink3b.isValid() );
    ASSERT_EQ( particle3->barcode(), testLink3b.barcode());
    ASSERT_EQ( event_number3, testLink3b.eventIndex());
    ASSERT_EQ(particle3,testLink3b.cptr());
    // HepMcParticleLink built using a GenParticle pointer and the
    // event_number of the GenEvent.
    HepMcParticleLink testLink3c(particle3,event_number3);
    ASSERT_TRUE( testLink3c.isValid() );
    ASSERT_EQ( particle3->barcode(), testLink3c.barcode());
    ASSERT_NE(  2, testLink3c.eventIndex());
    ASSERT_EQ( event_number3, testLink3c.eventIndex());
    ASSERT_EQ(particle3,testLink3c.cptr());
    // A HepMcParticleLink built using the barcode and the event_number of
    // the GenEvent. BROKEN
    HepMcParticleLink testLink3d(particle3->barcode(),event_number3);
    ASSERT_TRUE( testLink3d.isValid() );
    ASSERT_EQ( particle3->barcode(), testLink3d.barcode());
    ASSERT_NE(  2, testLink3d.eventIndex());
    ASSERT_EQ( event_number3, testLink3d.eventIndex());
    ASSERT_NE(particle3,testLink3d.cptr());
    ASSERT_EQ(particle1,testLink3d.cptr()); //POINTS AT THE IDENTICAL PARTICLE IN FIRST GenEvent!!

    StoreGateSvc* pStore(nullptr);
    ASSERT_TRUE(MCTesting::g_svcLoc->service("StoreGateSvc", pStore).isSuccess());
    pStore->clearStore(true); // forceRemove=true to remove all proxies
  }

} // <-- namespace MCTesting


int main(int argc, char *argv[])
{
  ::testing::InitGoogleTest( &argc, argv );
  ::testing::AddGlobalTestEnvironment( new MCTesting::GaudiEnvironment );
  return RUN_ALL_TESTS();
}
