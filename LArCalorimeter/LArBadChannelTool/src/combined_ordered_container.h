/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef combined_ordered_container_H
#define combined_ordered_container_H

#include "combined_ordered_iterator.h"

/**
 * A helper class that provides iterators over elements in two separate ordered containers
 * as if the elements were merged in a single container and sorted, but without any CPU 
 * overhead.
 */

namespace LArBadChanImpl {

  template <class Cont, class CMP=std::less<typename Cont::value_type> > 
  class combined_ordered_container {
  public:

    typedef typename Cont::value_type                 value_type;
    typedef combined_ordered_iterator< typename Cont::const_iterator, CMP>     iterator;
    typedef combined_ordered_iterator< typename Cont::const_iterator, CMP>     const_iterator;

    combined_ordered_container( const Cont& c1, const Cont& c2) : c1_(c1), c2_(c2) {}

    //iterator begin() {return iterator( c1_.begin(), c1_.end(), c2_.begin(), c2_.end(), CMP());} 
    const_iterator begin() const {return iterator( c1_.begin(), c1_.end(), 
						   c2_.begin(), c2_.end(), CMP());} 
    //iterator end() {return iterator( c1_.end(), c1_.end(), c2_.end(), c2_.end(), CMP());} 
    const_iterator end() const {return iterator( c1_.end(), c1_.end(), 
						 c2_.end(), c2_.end(), CMP());} 

  private:

    const Cont& c1_;
    const Cont& c2_;

  };

}

#endif
