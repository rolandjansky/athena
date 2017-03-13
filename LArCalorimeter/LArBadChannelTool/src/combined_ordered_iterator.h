/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef combined_ordered_iterator_H
#define combined_ordered_iterator_H

/**
 * An iterator that provides transparant iteration over the elements of two sorted containers,
 * presenting the elements in sorted order, as if the two containers were merged and the result
 * sorted, but without the CPU and memory overheads of merging and sorting.
 * This implementation is sufficient for forward iteration, but not much else. 
 */

namespace LArBadChanImpl {

  template <class Iter, class CMP> 
  class combined_ordered_iterator {
  public:

    typedef typename Iter::value_type value_type;

    combined_ordered_iterator( const Iter& b1, const Iter& e1,
			       const Iter& b2, const Iter& e2, const CMP& cmp) :
      m_b1(b1), m_e1(e1), m_b2(b2), m_e2(e2), m_cmp(cmp)
    {
      m_first = m_cmp( *m_b1, *m_b2);
    }

    combined_ordered_iterator() : m_b1(Iter()), m_e1(Iter()), 
                                  m_b2(Iter()), m_e2(Iter()), m_cmp(CMP()),
                                  m_first(false){}

    combined_ordered_iterator& operator++() {
      advance();
      if (at_the_end()) switch_to_other();
      else if ( !other_at_the_end()) {
	if (m_cmp(other(), current())) switch_to_other();  // we must switch to the other container
      }
      return *this;
    }

    combined_ordered_iterator operator++(int) {
      combined_ordered_iterator tmp(*this);
      operator++();
      return tmp;
    }

    const value_type& operator*() {
      return current();
    }

    operator bool() const {return m_b1 != m_e1 || m_b2 != m_e2;}

    bool operator!=( const combined_ordered_iterator& other) const {
      if (at_the_end()) return ! other.at_the_end();
      else if (other.at_the_end()) return true;
      else return current_iter() != other.current_iter();
    }

    bool operator==(const combined_ordered_iterator& other) const {
      return !operator!=(other);
    }

  private:
  
    Iter m_b1;
    Iter m_e1;
    Iter m_b2;
    Iter m_e2;
    CMP  m_cmp;
    bool m_first;

    const value_type& other() const {return m_first ? *m_b2 : *m_b1;}

    const value_type& current() const {return m_first ? *m_b1 : *m_b2;}

    const Iter& current_iter() const {return m_first ? m_b1 : m_b2;}

    const value_type& next_in_same() const {
      Iter tmp = (m_first? m_b1 : m_b2);
      return *(++tmp);
    }

    void advance() {
      if (m_first) {
	if (m_b1 != m_e1) ++m_b1;
      }
      else {
	if (m_b2 != m_e2) ++m_b2;
      }
    }

    bool at_the_end() const { 
      return m_first ? (m_b1 == m_e1) : (m_b2 == m_e2);
    }

    bool other_at_the_end() const { 
      return m_first ? (m_b2 == m_e2) : (m_b1 == m_e1);
    }

    void switch_to_other() { m_first = !m_first;}

  };

}

#endif
