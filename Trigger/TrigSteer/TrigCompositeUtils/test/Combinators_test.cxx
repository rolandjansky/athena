/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <iostream>
#include "TestTools/expect.h"
#include "TrigCompositeUtils/Combinators.h"

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


std::ostream& operator<<( std::ostream& o, const std::vector<size_t>& v ) {
  for ( auto el: v)
    o << el << " ";
  return o;
}

void trivialUniqueCombination() {
  std::cout << "\nunique combinations trivial case\n";

  UniqueCombinationGenerator g1(4, 1);  // 1 element combination out of numbers 0-3)
  auto c1 = g1(); // the first combination
  VALUE( g1.size() ) EXPECTED( 1 );
  std::cout << "trivialUniqueCombinations start: " << c1 << std::endl;
  VALUE(c1) EXPECTED({0});

  ++g1;
  c1 = g1();
  std::cout << "trivialUniqueCombinations: " << c1 << std::endl;
  VALUE(c1) EXPECTED({1});

  ++g1;
  c1 = g1();
  std::cout << "trivialUniqueCombinations: " << c1 << std::endl;
  VALUE(c1) EXPECTED({2});

  ++g1;
  c1 = g1();
  std::cout << "trivialUniqueCombinations: " << c1 << std::endl;
  VALUE(c1) EXPECTED({3});

  ++g1;
  c1 = g1();
  std::cout << "trivialUniqueCombinations end: " << c1 << std::endl;
  VALUE(c1) EXPECTED({});


  UniqueCombinationGenerator g2(2, 2); 
  c1 = g2(); // the first combination
  VALUE( g2.size() ) EXPECTED( 2 );

  std::cout << "trivialUniqueCombinations start: " << c1 << std::endl;
  VALUE(c1) EXPECTED({0, 1});

  ++g2;
  c1 = g2(); // the first combination
  std::cout << "trivialUniqueCombinations end: " << c1 << std::endl;
  VALUE(c1) EXPECTED({});


}
void smallUniqueCombination() {
  std::cout << "\nunique combinations small case\n";

  UniqueCombinationGenerator g1(4, 3);  // 3 element combination out of numbers 0-3)
  VALUE( g1.size() ) EXPECTED( 3 );

  auto c1 = g1(); // the first combination
  std::cout << "smallUniqueCobinations start: " << c1 << std::endl;
  VALUE(c1) EXPECTED({0, 1, 2});

  ++g1;
  c1 = g1();
  std::cout << "smallUniqueCobinations: " << c1 << std::endl;

  VALUE(c1) EXPECTED({0, 1, 3});

  ++g1;
  c1 = g1();
  std::cout << "smallUniqueCobinations: " << c1 << std::endl;

  VALUE(c1) EXPECTED({0, 2, 3});

  ++g1;
  c1 = g1();
  std::cout << "smallUniqueCobinations: " << c1 << std::endl;

  VALUE(c1) EXPECTED({1, 2, 3});

  ++g1;
  c1 = g1();
  std::cout << "smallUniqueCobinations: " << c1 << std::endl;
  VALUE(bool(g1)) EXPECTED(false);
  VALUE(c1) EXPECTED({});
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

void largerUniqueCombination() {
  UniqueCombinationGenerator g(5, 2);
  auto c1 = g(); // the first combination
  std::cout << "largerUniqueCombinations: " << c1 << std::endl;
  VALUE(c1) EXPECTED({0, 1});
  ++g; c1 = g();
  std::cout << "largerUniqueCombinations: " << c1 << std::endl;

  ++g; c1 = g();
  std::cout << "largerUniqueCombinations: " << c1 << std::endl;

  ++g; c1 = g();
  std::cout << "largerUniqueCombinations: " << c1 << std::endl;

  VALUE(c1) EXPECTED({0, 4});
  ++g; c1 = g();
  VALUE(c1) EXPECTED({1, 2});
}

void _count( size_t from, size_t choose) {
  UniqueCombinationGenerator g(from, choose);
  int counter = 0;
  while( g ) {
    ++g;
    ++counter;
  };
  std::cout << from << " choose " << choose << " " << counter << std::endl;
  VALUE ( counter) EXPECTED ( std::tgamma(from+1)/( std::tgamma(from-choose+1)* std::tgamma(choose+1)) );
}

void countUniqueCombinations() {
  _count(4,2);
  _count(10,2);
  _count(10,3);
  _count(10,4);
  _count(10,5);

  _count(11,11);

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


void nestedComb() {
  std::cout << "\nnested unique combiantions";

  // want all combinations of 2 from 3 el., 1 from 2 el and 2 from 2 (5 indices)
  // first two cycle through 0,1 -> 0,2 -> 1,2 
  // middle one cycles though 0 -> 1
  // last two indices never change
  NestedUniqueCombinationGenerator n;
  n.add({3,2});
  n.add({2,1});
  n.add({2,2});

//  ({{3, 2}, {2, 1}, {2,2}}); 
  auto c1 = n();
  VALUE( c1.size() ) EXPECTED ( 5 );
  std::cout << "unique: " << c1 << std::endl;
  VALUE(c1) EXPECTED({0,1,0,0,1});
  ++n;
  c1 = n();
  std::cout << "unique: " << c1 << std::endl;
  VALUE(c1) EXPECTED({0,2,0,0,1});

  ++n;
  c1 = n();
  std::cout << "unique: " << c1 << std::endl;
  VALUE(c1) EXPECTED({1,2,0,0,1});

  ++n;
  c1 = n();
  std::cout << "unique: " << c1 << std::endl;
  VALUE(c1) EXPECTED({0,1,1,0,1});

  ++n;
  c1 = n();
  std::cout << "unique: " << c1 << std::endl;
  VALUE(c1) EXPECTED({0,2,1,0,1});

  ++n;
  c1 = n();
  std::cout << "unique: " << c1 << std::endl;
  VALUE(c1) EXPECTED({1,2,1,0,1});

  ++n;
  c1 = n();
  VALUE(c1) EXPECTED({});
  VALUE(bool(n)) EXPECTED(false);

}


int main() {

  smallCombination();
  smallUniqueCombination();
  largerUniqueCombination();
  trivialUniqueCombination();
  countUniqueCombinations();
  endCombination();

  uniqueCombElements();
  uniqueComb();
  
  nestedComb();
  
  return 0;  
}
