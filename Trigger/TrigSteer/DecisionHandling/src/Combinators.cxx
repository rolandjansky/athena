/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/
#include <algorithm>
#include <iostream>
#include "DecisionHandling/Combinators.h"


using namespace HLT;
CombinationGenerator::CombinationGenerator( const std::initializer_list<size_t>& collectionSizes )
  : m_maxes( collectionSizes ),
    m_current( m_maxes.size() ) {
  reset();
}

void CombinationGenerator::reset() {
  // no combinations when at least one the sizes is 0 (empty collection)
  if ( std::any_of( m_maxes.begin(), m_maxes.end(),
		    []( size_t m){ return m == 0; } ) )
    m_current.clear(); 
  else
    fill(m_current.begin(), m_current.end(), 0);
}

void CombinationGenerator::operator++() {
  if ( m_current.empty() )  return;
  for ( size_t i = 0, imax = m_current.size(); i < imax; ++i ) { // iterate over current indices and find the index which can be increase
    if ( m_maxes[i] - m_current[i] == 1 ) { // reached end of the range of i-th collection, return to the beginning
      //      std::cout << ".. index " << i <<" reached the end (max, " << m_maxes[i] << " reseting it to 0 from current value " << m_current[i] << "\n";
      m_current[i] = 0;
    } else {
      //      std::cout << ".. index " << i <<" that can be incremetented " << m_maxes[i] << " current value " << m_current[i] << "\n";
      m_current[i]++;
      return;
    }
  }
  // unsuccesfull index increase == we need to make op bool returning false and next ++ not hapening
  m_current.clear();

}

CombinationGenerator::operator bool() const {
  if ( m_current.empty() )
    return false;
  return true;
}
