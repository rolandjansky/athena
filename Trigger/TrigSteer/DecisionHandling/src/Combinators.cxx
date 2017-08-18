/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/
#include <algorithm>
#include <iostream>
#include <iterator>
#include "DecisionHandling/Combinators.h"


using namespace HLT;
CombinationGenerator::CombinationGenerator( const std::initializer_list<size_t>& collectionSizes )
  : m_maxes( collectionSizes ),
    m_current( m_maxes.size() ) {
  reset();
}

void CombinationGenerator::reset() {
  // no combinations when at least one the sizes is 0 ( empty collection )
  if ( std::any_of( m_maxes.begin(), m_maxes.end(),
		    []( size_t m ){ return m == 0; } ) )
    m_current.clear(); 
  else
    fill( m_current.begin(), m_current.end(), 0 );
}

void CombinationGenerator::operator++() {
  if ( m_current.empty() )  return;
  for ( size_t i = 0, imax = m_current.size(); i < imax; ++i ) { // iterate over current indices and find the index which can be increase
    if ( m_maxes[i] - m_current[i] == 1 ) { // reached end of the range of i-th collection, return to the beginning
      m_current[i] = 0;
    } else {
      m_current[i]++;
      return;
    }
  }
  // unsuccesfull index increase == we need to make op bool returning false and next ++ should not happen
  m_current.clear();

}

CombinationGenerator::operator bool() const {
  if ( m_current.empty() )
    return false;
  return true;
}



namespace {
  
  void combMaker( const Index2DVec& indices, std::function<void (const Index1DVec&) > handle, std::function<bool (const Index1DVec&) > filter, size_t rank=0, Index1DVec combination={} ) {

    for ( auto position: indices[rank] ) {
      // found an element matching to this combination
      if ( std::find( combination.begin(), combination.end(), position ) == combination.end() ) {
	//	      std::cout << "rank " << rank << " adding or replacing " << position << " to a combination containing so far ";
	      //	      std::copy( combination.begin(), combination.end(), std::ostream_iterator<size_t>(std::cout, " ") );
	      //	      std::cout << "\n";
	if ( combination.size() == rank ) 
	  combination.push_back( position );
	else
	  combination[rank] = position;
 
	if ( combination.size() == indices.size() ) { // end of recursion
	  if ( filter( combination ) )
	    handle( combination );	  
	} else {
	  combMaker( indices, handle, filter, rank+1, combination);
	}
      }      
    }    
  }  
}

namespace HLT {
  void elementsInUniqueCombinations( const Index2DVec& indices,  std::set<size_t>& participants, std::function<bool(const Index1DVec&)> filter ) {
    auto handle = [&](const Index1DVec& combination ) {  for ( auto el: combination ) participants.insert(participants.begin(), el); };
    combMaker( indices, handle, filter );
  }
  
  void findUniqueCombinations( const Index2DVec& indices,  std::vector<std::vector<size_t> >& combinations, std::function<bool(const Index1DVec&)> filter ) {
    auto handle = [&](const Index1DVec& combination ) {  combinations.push_back( combination ); };
    combMaker( indices, handle, filter );  
  }
}




