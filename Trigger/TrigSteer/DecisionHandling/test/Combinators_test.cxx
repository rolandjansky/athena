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



void checkElementsIn(const Index2DVec& inp, const Index1DVec& exp, 
		     std::function<bool(const Index1DVec&)> filter = [](const Index1DVec&){ return true; } ) {
  std::cout << "\n";
  std::set<size_t> part;
  elementsInUniqueCombinations(inp, part, filter);

  for ( auto& v: inp ) {

    for ( auto el: v ) {
      std::cout << " " << el;
    }
    std::cout << " | ";
  }
  std::cout << ".... result is: >";
  for ( auto el: part ) {
    std::cout << " " << el;
  }
  std::cout << " <";
  
  VALUE ( part == std::set<size_t>( exp.begin(), exp.end() ) ) EXPECTED( true );

}


void checkCombinations(const Index2DVec& inp, const Index2DVec& exp ) {
  std::cout << "\n";
  Index2DVec allCombs;
  findUniqueCombinations(inp, allCombs);

  for ( auto& v: inp ) {
    for ( auto el: v ) 
      std::cout << " " << el;    
    std::cout << " | ";
  }
  std::cout << ".... result is: > ";
  for ( auto comb: allCombs ) {
    for ( auto el: comb ) 
      std::cout << " " << el;
    std::cout << " | ";
  }
  std::cout << " <";
  VALUE(allCombs == exp ) EXPECTED( true );
}

void uniqueCombElements() {
  std::cout << "\nelements in unique combinations";
  checkElementsIn( {{0, 3}, {0}}, {0, 3} );
  checkElementsIn( {{1}, {1}}, {} );
  checkElementsIn( {{}, {0}}, {} );
  checkElementsIn( {{}, {1}}, {} );
  checkElementsIn( {{0,3}, {1}}, {0,1,3} );
  checkElementsIn( {{0,3}, {0, 3}}, {0,3} );

  // 3 el combinations
  checkElementsIn( { {1}, {1}, {1} }, {} );
  checkElementsIn( { {1}, {1}, {2} }, {} );
  checkElementsIn( { {0}, {1}, {2} }, {0,1,2} );
  checkElementsIn( { {2}, {0,2,3}, {3} }, {0, 2, 3} );
  checkElementsIn( { {2}, {2,3}, {3} }, {} );
  checkElementsIn( { {3}, {1,2}, {3} }, {} );

  // with filtering
  //
  std::cout << "\nelements in unique combinations with filtering";
  checkElementsIn( {{0,1},{1,2,3}}, {1,2,3}, [](const Index1DVec& comb){ return comb[0] != 0; } );
}

void uniqueComb() {
  std::cout << "\nunique combinations";
  checkCombinations( { {0, 3}, {0}}, {{3, 0} }  ); 
  checkCombinations( { {0}, {0}}, {}  ); 
  checkCombinations( { {0,3}, {1,3}}, {{0, 1}, {0,3}, {3, 1} } );
  checkCombinations( { {0}, {1}, {2} }, {{0,1,2} } );
  checkCombinations( { {2}, {0,2,3}, {3} }, { {2, 0, 3} } );
  checkCombinations( { {2}, {0,2,3}, {1,3} }, { {2, 0, 1}, {2, 0, 3}, {2, 3, 1} } );
}


int main() {

  smallCombination();
  endCombination();

  uniqueCombElements();
  uniqueComb();
  
  //  CombinationGenerator g( {2, 3} );
  // this will come

  
  return 0;  
}
