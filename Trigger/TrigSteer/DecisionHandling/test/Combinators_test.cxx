/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <iostream>
#include "TestTools/expect.h"
#include "DecisionHandling/Combinators.h"


class ArrTest {
public:
  ArrTest( const std::vector<float>& cuts )
    : m_cuts(cuts) {}
  size_t size() { return m_cuts.size(); }
  bool operator()( size_t c, float v ) {
    return v > m_cuts[c];
  }
private:
  std::vector<float> m_cuts;
};


int main() {
  using namespace HLT;
  std::vector<float> a{1,2,3,4};
  ArrTest aCuts({1,2,3});
  //  VALUE(is_inclusive_combination_present(a, aCuts)) EXPECTED(true);


  CombinationsGenerator g( 2, 3);
  

  
  return 0;  
}
