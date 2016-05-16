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
      b1_(b1), e1_(e1), b2_(b2), e2_(e2), cmp_(cmp)
    {
      first_ = cmp_( *b1_, *b2_);
    }

    combined_ordered_iterator() : b1_(Iter()), e1_(Iter()), 
                                  b2_(Iter()), e2_(Iter()), cmp_(CMP()),
                                  first_(false){}

    combined_ordered_iterator& operator++() {
      advance();
      if (at_the_end()) switch_to_other();
      else if ( !other_at_the_end()) {
	if (cmp_(other(), current())) switch_to_other();  // we must switch to the other container
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

    operator bool() const {return b1_ != e1_ || b2_ != e2_;}

    bool operator!=( const combined_ordered_iterator& other) const {
      if (at_the_end()) return ! other.at_the_end();
      else if (other.at_the_end()) return true;
      else return current_iter() != other.current_iter();
    }

    bool operator==(const combined_ordered_iterator& other) const {
      return !operator!=(other);
    }

  private:
  
    Iter b1_;
    Iter e1_;
    Iter b2_;
    Iter e2_;
    CMP  cmp_;
    bool first_;

    const value_type& other() const {return first_ ? *b2_ : *b1_;}

    const value_type& current() const {return first_ ? *b1_ : *b2_;}

    const Iter& current_iter() const {return first_ ? b1_ : b2_;}

    const value_type& next_in_same() const {
      Iter tmp = (first_? b1_ : b2_);
      return *(++tmp);
    }

    void advance() {
      if (first_) {
	if (b1_ != e1_) ++b1_;
      }
      else {
	if (b2_ != e2_) ++b2_;
      }
    }

    bool at_the_end() const { 
      return first_ ? (b1_ == e1_) : (b2_ == e2_);
    }

    bool other_at_the_end() const { 
      return first_ ? (b2_ == e2_) : (b1_ == e1_);
    }

    void switch_to_other() { first_ = !first_;}

  };

}

#endif
