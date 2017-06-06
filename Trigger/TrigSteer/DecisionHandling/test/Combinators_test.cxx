/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <iostream>
#include "TestTools/expect.h"
#include "DecisionHandling/Combinators.h"

using namespace HLT;
void smallCombination () {
  CombinationGenerator g1({ 3, 2 });
  auto c1 = g1(); // the first combination
  std::cout << c1[0] <<  " " << c1[1] << std::endl;
  VALUE( size_t( c1[0] ) ) EXPECTED( 0 );
  VALUE( size_t( c1[1] ) ) EXPECTED( 0 );
  
  ++g1;
  c1 = g1();
  std::cout << c1[0] <<  " " << c1[1] << std::endl;
  VALUE( size_t( c1[0] ) ) EXPECTED( 1 );
  VALUE( size_t( c1[1] ) ) EXPECTED( 0 );

  ++g1;
  c1 = g1();
  std::cout << c1[0] <<  " " << c1[1] << std::endl;
  VALUE( size_t( c1[0] ) ) EXPECTED( 2 );
  VALUE( size_t( c1[1] ) ) EXPECTED( 0 );

  // here the start of the other index increase
  ++g1;
  c1 = g1();
  std::cout << c1[0] <<  " " << c1[1] << std::endl;
  VALUE( size_t( c1[0] ) ) EXPECTED( 0 );
  VALUE( size_t( c1[1] ) ) EXPECTED( 1 );

  // and back to the c1[0] beegin increased
  ++g1;
  c1 = g1();
  std::cout << c1[0] <<  " " << c1[1] << std::endl;
  VALUE( size_t( c1[0] ) ) EXPECTED( 1 );
  VALUE( size_t( c1[1] ) ) EXPECTED( 1 );

  ++g1;
  c1 = g1();
  std::cout << c1[0] <<  " " << c1[1] << std::endl;
  VALUE( size_t( c1[0] ) ) EXPECTED( 2 );
  VALUE( size_t( c1[1] ) ) EXPECTED( 1 );

  ++g1;
  c1 = g1();
  VALUE ( c1.size() ) EXPECTED ( 0 );
}

void endCombination() {
  CombinationGenerator g1({ 3, 2, 7, 8});
  auto c1 = g1();
  VALUE( c1.size() ) EXPECTED( 4 );
  
  while ( g1 ) {
    auto x = g1();
    ++g1;
  }
  auto endCombination = g1();
  VALUE( endCombination.size() ) EXPECTED( 0 ) ;
  
}



int main() {

  smallCombination();
  endCombination();
  
  //  CombinationGenerator g( {2, 3} );
  // this will come

  
  return 0;  
}
