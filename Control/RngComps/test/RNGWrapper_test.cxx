#include <iostream>
#include <cmath>
#include <limits>
#include <numeric>
#include <vector>
#include <cassert>
#include <atomic>

#include "AthenaKernel/RNGWrapper.h"
#include "TestTools/expect.h"
#include "TestTools/ParallelCallTest.h"
#include "AtlasCLHEP_RandomGenerators/dSFMTEngine.h"
#include "CLHEP/Random/Ranlux64Engine.h"
#include "CLHEP/Random/RanecuEngine.h"

  // assume large number of slots
const size_t nSlots = 20;

using namespace ATHRNG;
void reseed( RNGWrapper* wrapper, size_t slot ) {
  wrapper->setSeed("TestAlg", slot, 0xfffaaaffa34, 330000); // event and run number
}

void  pullN(RNGWrapper* wrapper, size_t ssize, std::vector<double>& sequence, size_t slot ) {
  sequence.reserve( ssize );
  EventContext ctx;
  ctx.setSlot( slot );
  for ( size_t i =0; i < ssize; ++i ) 
    sequence.push_back( wrapper->getEngine( ctx )->flat() );
}

void checkSlotInvaraianceSequential( RNGWrapper* wrapper ) {
  const size_t seqLen=  100000;
  
  double ref = 0.0;
  
  // instead of simple comparison between each value we will make sums and compare them
  // if this comparison fails we check further
  for ( size_t slot = 0; slot < nSlots; ++slot )  { 
    reseed( wrapper, slot );
    std::vector<double> sequence;
    pullN( wrapper, seqLen, sequence, slot );    
    double sum = std::accumulate( sequence.begin(), sequence.end(), 0.0 );
    if ( ref == 0.0 ) {
      ref = sum;    
    }
    
    VALUE( sum ) EXPECTED ( ref );
  }
}

/**
 * In the test the plan is to pull sequence of numbers from the slot 0 RNG
 * and then repeat the same for all other slots in parallel. It is expected that the sequence is 
 * always the same!  
 **/

class TestScenario : public ParallelCallTest {
public:
  TestScenario( RNGWrapper* wrapper, const std::string& name )
    : m_wrapper( wrapper ),
      m_name( name ){}

  // pull numbers from the first slot and make it a reference
  void firstCall() override {    
    reseed( m_wrapper, 0 );
    std::vector<double>  seq;
    pullN( m_wrapper, 10000, seq, 0);
    m_slot0SumRef = std::accumulate( seq.begin(), seq.end(), 0.0);
    //    std::cout << m_name << " Slot 0 sum of randoms " << m_slot0SumRef << std::endl;
  }
  
  bool callAndCompare() const override {
    size_t currentSlot = m_slot++ % nSlots;
    //    std::cout << "Testing slot " << currentSlot << std::endl;

    VALUE( currentSlot < 20 ) EXPECTED( true );
    reseed( m_wrapper, currentSlot );
    std::vector<double>  seq;
    pullN( m_wrapper, 10000, seq, currentSlot );
    double sum = std::accumulate( seq.begin(), seq.end(), 0.0);
    //    std::cout << m_name << " Slot " << currentSlot << " sum of randoms " << sum << std::endl;
    VALUE( sum ) EXPECTED ( m_slot0SumRef );
    return true;
  }
private:
  RNGWrapper* m_wrapper;
  std::string m_name;
  mutable std::atomic<size_t> m_slot{ 0 };
  double m_slot0SumRef;
};


int main() {
    std::function< CLHEP::HepRandomEngine*(void) > dSFMTFactory = [](void)->CLHEP::HepRandomEngine* { 
      return new CLHEP::dSFMTEngine(); 
    };
    RNGWrapper dSFMT( dSFMTFactory , nSlots );

    std::function< CLHEP::HepRandomEngine*(void) > RanluxFactory = [](void)->CLHEP::HepRandomEngine*{
      return new CLHEP::Ranlux64Engine();
    };
    RNGWrapper Ranlux( RanluxFactory , nSlots );

    std::function< CLHEP::HepRandomEngine*(void) > RanecuFactory = [](void)->CLHEP::HepRandomEngine*{
      return new CLHEP::RanecuEngine();
    };
    RNGWrapper Ranecu( RanecuFactory , nSlots );


    checkSlotInvaraianceSequential( &dSFMT );
    checkSlotInvaraianceSequential( &Ranlux );
    checkSlotInvaraianceSequential( &Ranecu );

    std::cout << "RNGWrapper works for slots if used sequentially" << std::endl;

    TestScenario dSFMTScenario( &dSFMT, "dSFMT" );
    TestScenario RanluxScenario( &Ranlux, "Ranlux" ); 
    TestScenario RanecuScenario( &Ranecu, "Ranecu" );

    for ( size_t rep = 0; rep < 100; ++rep ) {
      ParallelCallTest::launchTests( 20, { &dSFMTScenario, &RanluxScenario, &RanecuScenario } );
    }

    std::cout << "RNGWrapper works for slots when used in parallel" << std::endl;
    return 0;
}
