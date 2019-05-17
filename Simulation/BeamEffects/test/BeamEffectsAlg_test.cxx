/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @author John Chapman
 * @date June, 2016
 * @brief Tests for BeamEffectsAlg.
 */

#undef NDEBUG

// Framework
#include "TestTools/initGaudi.h"

// ATLAS C++
#include "CxxUtils/make_unique.h"

// Google Test
#include "gtest/gtest.h"
// Google Mock
// #include "gmock/gmock.h"

// HepMC includes
#include "HepMC/GenEvent.h"

// CLHEP includes
#include "CLHEP/Vector/LorentzVector.h"
#include "CLHEP/Units/SystemOfUnits.h"

// Athena headers
#include "CxxUtils/make_unique.h"
#include "GeneratorObjects/McEventCollection.h"

// Tested AthAlgorithm
#include "../src/BeamEffectsAlg.h"


namespace SimTesting {

  // needed every time an AthAlgorithm, AthAlgTool or AthService is instantiated
  ISvcLocator* g_svcLoc = nullptr;

  // global test environment takes care of setting up Gaudi
  class GaudiEnvironment : public ::testing::Environment {
  protected:
    virtual void SetUp() override {
      Athena_test::initGaudi(SimTesting::g_svcLoc);
    }
  };

  class BeamEffectsAlg_test : public ::testing::Test {

  protected:
    virtual void SetUp() override {
      m_alg = new Simulation::BeamEffectsAlg{"BeamEffectsAlg", g_svcLoc};
      ASSERT_TRUE( m_alg->setProperties().isSuccess() );
    }

    virtual void TearDown() override {
      ASSERT_TRUE( m_alg->finalize().isSuccess() );
      delete m_alg;
    }

    //
    // accessors for private methods
    // NB: This works because BeamEffectsAlg_test is a friend of the tested
    //     BeamEffectsAlg AthAlgorithm
    //
    template<typename... Args>
    StatusCode patchSignalProcessVertex(Args&&... args) const {
      return m_alg->patchSignalProcessVertex(std::forward<Args>(args)...);
    }

    // template<typename... Args>
    // StatusCode setupReadHandleKeyVector(Args&&... args) const {
    //   return m_alg->setupReadHandleKeyVector(std::forward<Args>(args)...);
    // }

    // template<typename... Args>
    // void mergeCollections(Args&&... args) const {
    //   m_alg->mergeCollections(std::forward<Args>(args)...);
    // }
    Simulation::BeamEffectsAlg* m_alg;
  };   // BeamEffectsAlg_test fixture


  TEST_F(BeamEffectsAlg_test, empty_alg_execute) {
    // expected to fail as input collection doesn't exist
    ASSERT_TRUE( m_alg->execute().isFailure() );
  }

  TEST_F(BeamEffectsAlg_test, set_properties) {
    // ordering A, C, B is on purpose to test for unintended alphabetic ordering
    std::string  inputPropertyValue = "'TestGEN_EVENT'";
    std::string outputPropertyValue = "'TestBeamTruthEvent'";
    ASSERT_TRUE( m_alg->setProperty( "InputMcEventCollection",   inputPropertyValue).isSuccess() );
    ASSERT_TRUE( m_alg->setProperty( "OutputMcEventCollection", outputPropertyValue).isSuccess() );
    ASSERT_TRUE( m_alg->setProperty( "ISFRun", true).isSuccess()  );
  }

  TEST_F(BeamEffectsAlg_test, patchSignalProcessVertex_empty_GenEvent) {
    HepMC::GenEvent ge;
    ASSERT_TRUE( patchSignalProcessVertex(ge).isSuccess() );
    ASSERT_TRUE( ge.signal_process_vertex()==nullptr );
  }

  TEST_F(BeamEffectsAlg_test, signal_process_vertex_exists) {
    HepMC::GenEvent ge;
    CLHEP::HepLorentzVector myPos( 1.0, 1.0, 1.0, 1.0);
    HepMC::GenVertex *myVertex = new HepMC::GenVertex( myPos, -1 );
    ge.set_signal_process_vertex( myVertex );
    ASSERT_TRUE( patchSignalProcessVertex(ge).isSuccess() );
    ASSERT_TRUE( ge.signal_process_vertex()==myVertex );
  }

  TEST_F(BeamEffectsAlg_test, add_signal_process_vertex_atlasG4) {
    HepMC::GenEvent ge;
    CLHEP::HepLorentzVector myPos( 1.0, 1.0, 1.0, 1.0);
    HepMC::GenVertex *myVertex = new HepMC::GenVertex( myPos, -1 );
    ge.add_vertex( myVertex );
    ASSERT_TRUE( ge.signal_process_vertex()==nullptr );
    ASSERT_TRUE( m_alg->setProperty( "ISFRun", false).isSuccess()  );
    ASSERT_TRUE( patchSignalProcessVertex(ge).isSuccess() );
    ASSERT_TRUE( ge.signal_process_vertex()==myVertex );
    ASSERT_EQ( *ge.signal_process_vertex(), *myVertex );
  }

  TEST_F(BeamEffectsAlg_test, add_signal_process_vertex_isfG4) {
    HepMC::GenEvent ge;
    CLHEP::HepLorentzVector myPos( 1.0, 1.0, 1.0, 1.0);
    HepMC::GenVertex *myVertex = new HepMC::GenVertex( myPos, -1 );
    HepMC::GenVertex *dummyVertex = new HepMC::GenVertex();
    ge.add_vertex( myVertex );
    ASSERT_TRUE( ge.signal_process_vertex()==nullptr );
    ASSERT_TRUE( m_alg->setProperty( "ISFRun", true).isSuccess()  );
    ASSERT_TRUE( patchSignalProcessVertex(ge).isSuccess() );
    ASSERT_TRUE( ge.signal_process_vertex()!=myVertex );
    ASSERT_EQ( *ge.signal_process_vertex(), *dummyVertex );
  }

  TEST_F(BeamEffectsAlg_test, execute_pass_through) {
    // create dummy input McEventCollection containing a dummy GenEvent
    SG::WriteHandle<McEventCollection> inputTestDataHandle{"GEN_EVENT"};
    inputTestDataHandle = CxxUtils::make_unique<McEventCollection>();
    inputTestDataHandle->push_back(new HepMC::GenEvent());
    HepMC::GenEvent& ge = *(inputTestDataHandle->at(0));
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
    //
    ASSERT_TRUE( m_alg->execute().isSuccess() );
    SG::ReadHandle<McEventCollection>     outputTestDataHandle{"BeamTruthEvent"};
    ASSERT_TRUE( outputTestDataHandle.isValid() );
    ASSERT_EQ(*(outputTestDataHandle->at(0)->signal_process_vertex()), *(inputTestDataHandle->at(0)->signal_process_vertex()));
    ASSERT_EQ(**(outputTestDataHandle->at(0)->vertices_begin()), **(inputTestDataHandle->at(0)->vertices_begin()));
    ASSERT_EQ(*(outputTestDataHandle->at(0)->beam_particles().first), *(inputTestDataHandle->at(0)->beam_particles().first));
    ASSERT_EQ(*(outputTestDataHandle->at(0)->beam_particles().second), *(inputTestDataHandle->at(0)->beam_particles().second));
  }

} // <-- namespace SimTesting


int main(int argc, char *argv[])
{
  ::testing::InitGoogleTest( &argc, argv );
  ::testing::AddGlobalTestEnvironment( new SimTesting::GaudiEnvironment );
  return RUN_ALL_TESTS();
}
