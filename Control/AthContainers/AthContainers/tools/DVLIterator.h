// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file AthContainers/tests/DVLIterator.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Sep, 2010
 * @brief Iterator classes for @c DataVector/@c DataList.
 */


#ifndef ATHCONTAINERS_TOOLS_DVLITERATOR_H
#define ATHCONTAINERS_TOOLS_DVLITERATOR_H


#include "AthContainers/OwnershipPolicy.h"
#include "AthContainers/tools/DVLCast.h"
#include "AthContainers/tools/ElementProxy.h"
#include <boost/iterator/iterator_adaptor.hpp>
#include <boost/version.hpp>
#include <iterator>
#include <cstdlib>


#if BOOST_VERSION < 105000
namespace boost { namespace detail {


/**
 * @brief Hack for dictionary generation.
 *
 * This is a hack to allow the operator->() defined by the boost base class
 * of iterator to compile.  It actually makes no sense to ever use operator->,
 * since value_type is always a pointer.  But if you try to generate
 * dictionary information for one of these iterators, then you'll
 * be trying to instantiate operator->, whether or not it makes sense.
 * This should be sufficient to get things to compile; we'll just stick
 * in abort in case anyone manages to actually call the thing.
 */
template <class T, class U>
struct operator_arrow_result<T*,
                             DataModel_detail::ElementProxy<U>,
                             T**>
{
public:
  typedef T* ValueType;
  typedef DataModel_detail::ElementProxy<U> Reference;
  typedef T** Pointer;
  typedef Pointer type;
  static type make (Reference /*x*/) { std::abort(); return 0; }
};


}}
#endif


namespace DataModel_detail {


/**
 * @brief Const iterator class for @c DataVector/@c DataList.
 *
 * We need to cast dereferences to the appropriate type.
 * This could be done with a @c boost::transform_iterator,
 * except that the iterator category for that will
 * be one of boost's new-style iterator categories, which
 * don't interoperate well with the standard C++ categories.
 * In particular, the container concept checks didn't work.
 *
 * This is templated on the specific @c DataVector/List class.
 *
 * This used to be done using boost::iterator_adaptor, but that caused
 * problems with root6.  So now use just write out the iterator
 * explicitly.
 */
template <class DVL>
class const_iterator
{
public:
  /// Standard iterator typedefs.
  typedef typename DVL::const_value_type value_type;
  typedef value_type reference;
  typedef value_type* pointer;
  typedef typename DVL::BaseContainer::const_iterator::difference_type difference_type;
  typedef typename DVL::BaseContainer::const_iterator::iterator_category iterator_category;

  typedef DVL Container;
  typedef typename DVL::BaseContainer BaseContainer;


  /**
   * @brief Default constructor.
   */
  const_iterator() {}


  /**
   * @brief Constructor.
   * @param it The underlying container iterator.
   */
   const_iterator (typename BaseContainer::const_iterator it)
    : m_it (it)
  {}


  /// Dereference.  Operator-> doesn't make sense here.
  reference operator*() const { return dereference(); }
  reference operator[] (difference_type n) const
  { const_iterator tmp = *this + n; return *tmp; }


  /// Increment / decrement.
  const_iterator& operator++() { ++m_it; return *this; }
  const_iterator operator++(int)
  { const_iterator tmp = *this; ++m_it; return tmp; }
  const_iterator& operator--() { --m_it; return *this; }
  const_iterator operator--(int)
  { const_iterator tmp = *this; --m_it; return tmp; }


  /// Arithmetic.
  const_iterator& operator+= (difference_type n) { m_it += n; return *this; }
  const_iterator& operator-= (difference_type n) { m_it -= n; return *this; }
  const_iterator operator+ (difference_type n) const { const_iterator tmp = *this; tmp += n; return tmp; }
  const_iterator operator- (difference_type n) const { const_iterator tmp = *this; tmp -= n; return tmp; }
  difference_type operator- (const_iterator other) const { return m_it - other.m_it; }


  /// Comparisons.
  bool operator== (const const_iterator& other) const { return m_it == other.m_it; }
  bool operator!= (const const_iterator& other) const { return m_it != other.m_it; }
  bool operator< (const const_iterator& other) const { return m_it < other.m_it; }
  bool operator> (const const_iterator& other) const { return m_it > other.m_it; }
  bool operator<= (const const_iterator& other) const { return m_it <= other.m_it; }
  bool operator>= (const const_iterator& other) const { return m_it >= other.m_it; }


private:
  /// The wrapped iterator.
  typename BaseContainer::const_iterator m_it;

  /**
   * @brief Dereference the iterator.
   * @return The pointed-to element.
   */
  const typename DVL::value_type dereference() const
  {
    return DataModel_detail::DVLCast<DVL>::cast (*m_it);
  }
};


/**
 * @brief (Non-const) Iterator class for @c DataVector/@c DataList.
 *
 * Except for dereferencing, we want this to be the same as the
 * @c iterator of the underlying container type.  So, we use
 * @c boost::iterator_adaptor and override @c dereference.
 *
 * This should nearly meets underlying container's iterator requirements, except
 * that the dereferencing operations return an @c ElementProxy
 * instead of a @c reference.
 *
 * This is templated on the specific @c DataVector/List class.
 */
template <class DVL>
class iterator
  : public boost::iterator_adaptor<iterator<DVL>,
                                   typename DVL::BaseContainer::iterator,
                                   typename DVL::value_type, // Value
                                   // Note: Can't use boost::use_default here.
                                   // Otherwise, we get boost's new-fangled
                                   // iterator categories, which won't work
                                   // with libstdc++'s implementation
                                   // of sort().
                                   // CategoryOrTraversal
                                   typename DVL::BaseContainer::
                                            const_iterator::iterator_category,
                                   typename DVL::ElementProxy> // Reference
{
private:
  // Shorthand for the base class type.
  typedef boost::iterator_adaptor<iterator,
                                  typename DVL::BaseContainer::iterator,
                                  typename DVL::value_type, // Value
                                  // CategoryOrTraversal
                                  typename DVL::BaseContainer::
                                           const_iterator::iterator_category,
                                  typename DVL::ElementProxy> iterator_adaptor_;

  // Copy some other names 
  typedef typename DVL::ElementProxy ElementProxy;
  typedef typename DVL::const_iterator const_iterator;


public:
  typedef DVL Container;
  typedef typename DVL::BaseContainer BaseContainer;


  /**
   * @brief Default constructor.
   */
  iterator() :
    m_container(0)
  {}


  /**
   * @brief Constructor.
   * @param it The underlying container iterator.
   * @param container The container over which we're iterating.
   */
  iterator (typename BaseContainer::iterator it,
            DVL* container)
    : iterator_adaptor_ (it),
      m_container (container)
  {}


  /**
   * @brief Element access.
   * @param n Index relative to the iterator of the element to access.
   * @return Proxy for the element.
   *
   * We need to override this here in order to get the return type correct.
   */
  ElementProxy operator[] (int n) const
  {
    return ElementProxy (this->base()+n, m_container);
  }


  /**
   * @brief Convert to a @c const_iterator.
   * 
   * This is needed to allow @c iterator to convert to @c const_iterator.
   */
  operator const_iterator() const
  {
    return const_iterator (this->base());
  }


  /**
   * @brief Return the container holding the referenced element.
   */
  DVL* container()
  {
    return m_container;
  }


  /**
   * @brief Return the container holding the referenced element.
   */
  const DVL* container() const
  {
    return m_container;
  }


  /**
   * @brief Return the ownership policy of the container from
   *        which this iterator was created.
   */
  SG::OwnershipPolicy ownPolicy() const
  {
    return m_container->ownPolicy();
  }


  /**
   * @brief Test if we can insert; raise an exception if not.
   * @param op Description of the attempted operation.
   */
  void testInsert (const char* op)
  {
    m_container->testInsert (op);
  }


  // These are needed for iterator/const_iterator comparisons to work.
  bool operator== (const iterator& i) const { return this->base() == i.base();}
  bool operator!= (const iterator& i) const { return this->base() != i.base();}
  bool operator<  (const iterator& i) const { return this->base() <  i.base();}
  bool operator>  (const iterator& i) const { return this->base() >  i.base();}
  bool operator<= (const iterator& i) const { return this->base() <= i.base();}
  bool operator>= (const iterator& i) const { return this->base() >= i.base();}

  // These are needed for iterator/const_iterator comparisons to work.
  bool operator== (const const_iterator& i) const
  { return static_cast<const_iterator>(*this)==i; }
  bool operator!= (const const_iterator& i) const
  { return static_cast<const_iterator>(*this)!=i; }
  bool operator<  (const const_iterator& i) const
  { return static_cast<const_iterator>(*this)<i; }
  bool operator>  (const const_iterator& i) const
  { return static_cast<const_iterator>(*this)>i; }
  bool operator<= (const const_iterator& i) const
  { return static_cast<const_iterator>(*this)<=i; }
  bool operator>= (const const_iterator& i) const
  { return static_cast<const_iterator>(*this)>=i; }

  // These overloads are needed for iterator-const_iterator to work.
  typename iterator_adaptor_::difference_type
  operator- (const iterator& i) const
  { return this->base() - i.base(); }
  iterator operator- (typename iterator_adaptor_::difference_type i) const
  { return iterator (this->base() - i, m_container); }
  typename iterator_adaptor_::difference_type
  operator- (const const_iterator& i) const
  { return static_cast<const_iterator>(*this) - i; }


private:
  typename DVL::pointer operator-> ();

  // Required for iterator_adaptor to work.
  friend class boost::iterator_core_access;


  /**
   * @brief Dereference the iterator.
   * @return A proxy to the pointed-to element.
   */
  ElementProxy dereference() const
  {
    return ElementProxy (this->base(), m_container);
  }


  /// The referenced container.
  DVL* m_container;
};


} // namespace DataModel_detail


// We need to define these to allow comparisons of
// iterator and const_iterator.
template <class DVL>
inline
bool operator== (const typename DVL::const_iterator& i1,
                 const DataModel_detail::iterator<DVL>& i2)
{
  return i1 == typename DVL::const_iterator (i2);
}
template <class DVL>
inline
bool operator!= (typename DVL::const_iterator i1,
                 DataModel_detail::iterator<DVL> i2)
{
  return i1 != typename DVL::const_iterator (i2);
}
template <class DVL>
inline
bool operator< (typename DVL::const_iterator i1,
                DataModel_detail::iterator<DVL> i2)
{
  return i1 < typename DVL::const_iterator (i2);
}
template <class DVL>
inline
bool operator> (typename DVL::const_iterator i1,
                DataModel_detail::iterator<DVL> i2)
{
  return i1 > typename DVL::const_iterator (i2);
}
template <class DVL>
inline
bool operator<= (typename DVL::const_iterator i1,
                 DataModel_detail::iterator<DVL> i2)
{
  return i1 <= typename DVL::const_iterator (i2);
}
template <class DVL>
inline
bool operator>= (typename DVL::const_iterator i1,
                 DataModel_detail::iterator<DVL> i2)
{
  return i1 >= typename DVL::const_iterator (i2);
}

// This overloads is needed for const_iterator-iterator to work.
template <class DVL>
inline
typename DVL::const_iterator::difference_type
operator- (typename DVL::const_iterator i1,
           DataModel_detail::iterator<DVL> i2)
{
  return i1 - typename DVL::const_iterator (i2);
}


#endif // not ATHCONTAINERS_TOOLS_DVLITERATOR_H
