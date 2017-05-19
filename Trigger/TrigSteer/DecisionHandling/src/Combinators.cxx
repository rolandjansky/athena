/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/
#include <algorithm>
#include "DecisionHandling/Combinators.h"


using namespace HLT;
CombinationsGenerator::CombinationsGenerator( const std::initializer_list<size_t>& collectionSizes )
  : m_maxes( collectionSizes ),
    m_current( m_maxes.size() ) {}

void CombinationsGenerator::reset() {
  fill(m_current.begin(), m_current.end(), 0);
}

  
const std::vector<size_t>& CombinationsGenerator::operator()() const {
  retrun m_current;
}
void CombinationsGenerator::operator++() {
  for ( size_t i = 0; i < m_maxes.size(); ++i )  { // find first index that can be increased
    if ( m_current[i] < m_maxes[i] ) {
      m_current[i]++;
      return;
    }
  }
  m_current.clear(); // if we do not mamange to increase anu index, it is the end
}

CombinationsGenerator::operator bool() const {
  if ( m_current.empty() )
    return false;
  return true;
}

  
  

  return ret;
}
