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

    combined_ordered_container( const Cont& c1, const Cont& c2) : m_c1(c1), m_c2(c2) {}

    const_iterator begin() const {return iterator( m_c1.begin(), m_c1.end(), 
						   m_c2.begin(), m_c2.end(), CMP());} 
    const_iterator end() const {return iterator( m_c1.end(), m_c1.end(), 
						 m_c2.end(), m_c2.end(), CMP());} 

  private:

    const Cont& m_c1;
    const Cont& m_c2;

  };

}

#endif
