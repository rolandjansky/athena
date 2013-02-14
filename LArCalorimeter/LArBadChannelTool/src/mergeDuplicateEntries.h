/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef mergeDuplicateEntries_H
#define mergeDuplicateEntries_H

#include <vector>

/**
 * Merges elements in the container that are equivalent according to the ordering defined by 
 * the binary predicate CMP. The merge operation is provided by the template argument MERGER.
 * The result is returned in a std::vector< C::value_type> where C is the type of the input 
 * container.
 * PRECONDITION: the input container is sorted according to CMP. 
 * This function is a generalization of the algorithm std::unique
 */
namespace LArBadChanImpl {

  template <class C1, class CMP, class MERGER> 
  std::vector<typename C1::value_type> 
  mergeDuplicateEntries( const C1& c1, const CMP& cmp, const MERGER& merger)
  {
    typename C1::const_iterator first = c1.begin();
    typename C1::const_iterator current = first;
    typename C1::const_iterator last = c1.end();
    std::vector<typename C1::value_type> result;

    // protection against empty container
    if (first == last) return result;

    while (++first != last) {
      if (cmp( *current, *first)) { // elements differ, no need to merge
	result.push_back(*current);
	++current;
      }
      else { // elemens are equivalent, need to merge
	typename C1::value_type merged = merger( *current, *first);
	while (++first != last && !cmp( *current, *first)) {
	  merged = merger( merged, *first); // keep merging while elements don't differ
	}
	result.push_back(merged);
	if (first == last) return result;
	current = first; // keep track of the element after the last merged
      }
    }
    result.push_back( *current);
    return result;
  }

  struct DummyMerger {
    int operator()(const int & a, const int& /*b*/) const { return a;}
  };

}

#endif
