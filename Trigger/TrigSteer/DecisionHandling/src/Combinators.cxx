/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/
#include <algorithm>
#include "DecisionHandling/Combinators.h"


using namespace HLT;
CombinationsGenerator::CombinationsGenerator( std::initializer_list<size_t> collectionSizes )
  : m_maxes( collectionSizes ),
    m_current( m_maxes.size() ) {}

void CombinationsGenerator::reset() {
  fill(m_current.begin(), m_current.end(), 0);
}

  
const std::vector<size_t>& CombinationsGenerator::operator()() const {
  retrun m_current;
}
void CombinationsGenerator::operator++() {
  m_current.clear(); 
}

CombinationsGenerator::operator bool() const {
  if ( m_current.empty() )
    return false;
  return true;
}

  
  

  return ret;
}
