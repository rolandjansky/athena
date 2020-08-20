// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ATHLINKS_ELEMENTLINKVECTOR_H
#define ATHLINKS_ELEMENTLINKVECTOR_H

#include <algorithm>
#include <exception>
#include <functional>
#include <vector>
#include <boost/iterator/transform_iterator.hpp>
#include <boost/iterator_adaptors.hpp> 

#include "AthLinks/ElementLinkVectorBase.h"
#include "AthLinks/DataLink.h"
#include "AthLinks/ElementLink.h"
#include "AthLinks/tools/SGELVRef.h"
#include "AthLinks/tools/selection_ns.h"


// Forward declaration(s):
ENTER_ROOT_SELECTION_NS
template< class STORABLE >
struct ElementLinkVector;
EXIT_ROOT_SELECTION_NS

// forward declarations of our friends
template <typename DOBJ>
bool
operator <(const ElementLinkVector<DOBJ>& lhs,
	   const ElementLinkVector<DOBJ>& rhs);
template <typename DOBJ>
bool
operator ==(const ElementLinkVector<DOBJ>& lhs,
	    const ElementLinkVector<DOBJ>& rhs);

/** @class ElementLinkVector
 * @brief a vector of "compact" element links. It turns the host data object
 * key into an index. The memory size of a compact link is three
 * words/link, and only 2 words/link needs to be persistified.
 * It also maintains a vector of hosts used to generate the "short ref"
 *
 * @param DOBJ        host object type (e,g. vector<Elem>, NOT Elem),
 * All host data objects must have the same type.
 * xxx
 * @param IndexingPolicy  policy to find the element in the host DOBJ \n
 * IndexingPolicy is generated automatically for STL sequences
 * (e.g. vector, DataList etc). For other types of containers
 * (e.g. maps), the container author must define the container type
 * using the macros in tools/DeclareIndexingPolicies.h \n
 * Advanced developers may have to define an ad-hoc indexing policy
 * (e.g. GenParticleIndexing in GeneratorObjects/McEventIndexingPolicy.h)
 *
 * @author ATLAS Collaboration
 * $Id: ElementLinkVector.h 702198 2015-10-21 20:58:43Z ssnyder $
 **/

template <typename DOBJ>
class ElementLinkVector : public ElementLinkVectorBase
{
private:
  typedef ElementLinkVector<DOBJ> ElemLinkVec;
  typedef SG::ELVRef<DOBJ> ElemLinkRef;


public:
  typedef typename std::vector< DataLink<DOBJ> > DataLinkVector;
  typedef ElementLink<DOBJ> ElemLink;

  /** @class Short2LongRef
   *  @brief a functor turning an ElemLinkRef into an ElementLink
   */
  struct Short2LongRef : public std::unary_function<ElemLinkRef, ElemLink> {
    // i.e.    friend class ElemLinkRef;
    typedef ElementLinkVector<DOBJ> ElemLinkVec;
    ElemLink operator()(ElemLinkRef& shortRef) const { 
      return shortRef.elementLink();
    }
    const ElemLink operator()(const ElemLinkRef& shortRef) const {
      return shortRef.elementLink();
    }
  };

private:
  ///the element links stored as ElemLinkRefs for compactness
  typedef typename std::vector<ElemLinkRef> RefVector;
  RefVector m_shortRefs;    

  ///the dobjs hosting our elements. They are all of type DOBJ 
  DataLinkVector m_hostDObjs;

public:  
  typedef typename ElemLink::ElementConstPointer ElementConstPointer;
  typedef typename ElemLink::index_type index_type;
  typedef typename ElemLink::ID_type ID_type;
  typedef typename ElemLink::sgkey_t sgkey_t;


  // We used to just use transform_iterator directly.
  // However, postincrement for transform_iterator is implemented as a free
  // function, which pyroot doesn't see.  This causes python iteration
  // over ElementLinkVector to fail.
  template <class Iterator>
  class ELVIterator
    : public boost::transform_iterator<Short2LongRef, Iterator>
  {
  public:
    typedef boost::transform_iterator<Short2LongRef, Iterator> Base;
    using Base::Base;
    ELVIterator (const Base& b) : Base (b) {}
    using Base::operator++;
    using Base::operator--;
    ELVIterator operator++(int) { ELVIterator tmp=*this; ++(*this); return tmp; }
    ELVIterator operator--(int) { ELVIterator tmp=*this; --(*this); return tmp; }
  };

  ///\name vector typedefs: it behaves like a vector<ElemLink>
  //@{
  typedef ElemLink& reference;
  //FIXME  typedef const ElemLink& const_reference;
  typedef ElemLink const_reference;
  typedef ELVIterator<typename RefVector::iterator> iterator;
  typedef ELVIterator<typename RefVector::const_iterator> const_iterator;
  //1.30  typedef typename boost::transform_iterator_generator<Short2LongRef, typename RefVector::iterator>::type iterator;
  //1.30  typedef typename boost::transform_iterator_generator<Short2LongRef, typename RefVector::const_iterator>::type const_iterator;
  typedef typename RefVector::size_type size_type;
  typedef typename RefVector::difference_type difference_type;
  typedef ElemLink value_type;
  typedef typename RefVector::allocator_type allocator_type;
  typedef ElemLink* pointer;
  typedef const ElemLink* const_pointer;
  typedef ELVIterator<typename RefVector::reverse_iterator> reverse_iterator;
  typedef ELVIterator<typename RefVector::const_reverse_iterator> const_reverse_iterator;
  //1.30  typedef typename boost::transform_iterator_generator<Short2LongRef, typename RefVector::reverse_iterator>::type reverse_iterator;
  //1.30  typedef typename boost::transform_iterator_generator<Short2LongRef, typename RefVector::const_reverse_iterator>::type const_reverse_iterator;
  //@}


  /// \name recommended (fast) access to contents of ElemLinks
  /// by-pass the expensive creation of ElemLinks
  //@{
  /// pointer to an element, given its ElementLinkVector index. O(1)
  ElementConstPointer elementCPtr(size_type index) const {
    return m_shortRefs[index].cptr();
  }
  /// host index of an element, given its ElementLinkVector index. O(1)
  index_type elementIndex(size_type index) const {
    return m_shortRefs[index].elementIndex();
  }
  /// dataID (long ref) of an element, given its ElementLinkVector index. O(1)
  ID_type elementDataID(size_type index) const {
    return m_shortRefs[index].dataID();
  }

  //@}

  /// \name iterators to DataLinkVector of host dobjs. Use e.g. for persistency
  //@{
  const DataLinkVector& hostDObjs() const { return m_hostDObjs; }
  typename DataLinkVector::iterator beginHostDObjs() {
    return m_hostDObjs.begin();
  }
  typename DataLinkVector::iterator endHostDObjs() {
    return m_hostDObjs.end();
  }
  typename DataLinkVector::const_iterator beginHostDObjs() const {
    return m_hostDObjs.begin();
  } 
  typename DataLinkVector::const_iterator endHostDObjs() const {
    return m_hostDObjs.end();
  }
  ///find the host of an element. Returns endHostDObjs() if not found
  typename DataLinkVector::const_iterator findHostDObj(const ElemLink& link) const;
  ///find the host of an element. Returns endHostDObjs() if not found
  typename DataLinkVector::iterator findHostDObj(const ElemLink& link);

  // Set the vector of host data objects from @a dobjs.
  // @a dobjs is destroyed.
  // This is an error if the vector is not empty.
  void moveHostDObjs (DataLinkVector& dobjs);

  //@}


  /// \name vector constructors (no Allocators)
  //@{
  ElementLinkVector();
 
  ElementLinkVector(size_type n, const ElemLink& link) :
    ElementLinkVectorBase(), m_shortRefs(n, ElemLinkRef(link))
  { 
    if (n > 0)
      addHostDObj(link);
  }

  ElementLinkVector(int n, const ElemLink& link = ElemLink()) :
    ElementLinkVectorBase(), m_shortRefs(n, ElemLinkRef(link))
  {
    if (n > 0)
      addHostDObj(link);
  }

  ElementLinkVector(long n, const ElemLink& link = ElemLink()) :
    ElementLinkVectorBase(), m_shortRefs(n, ElemLinkRef(link))
  {
    if (n > 0)
      addHostDObj(link);
  }

  explicit ElementLinkVector(size_type n) :
    ElementLinkVectorBase(), m_shortRefs(n, ElemLinkRef()) { }
  
  ElementLinkVector(const ElemLinkVec& vec) :
    ElementLinkVectorBase( vec ),
    m_shortRefs(vec.m_shortRefs), m_hostDObjs(vec.m_hostDObjs) { }

  ElementLinkVector(ElemLinkVec&& vec) noexcept :
    ElementLinkVectorBase( std::move(vec) ),
    m_shortRefs(std::move(vec.m_shortRefs)),
    m_hostDObjs(std::move(vec.m_hostDObjs)) { }

  ElementLinkVector& operator= (const ElemLinkVec& vec)
  {
    if (this != &vec) {
      m_persKeys    = vec.m_persKeys;
      m_persIndices = vec.m_persIndices;
      m_shortRefs   = vec.m_shortRefs;
      m_hostDObjs   = vec.m_hostDObjs;
    }
    return *this;
  }

  ElementLinkVector& operator= (ElemLinkVec&& vec) noexcept
  {
    if (this != &vec) {
      m_persKeys    = std::move(vec.m_persKeys);
      m_persIndices = std::move(vec.m_persIndices);
      m_shortRefs   = std::move(vec.m_shortRefs);
      m_hostDObjs   = std::move(vec.m_hostDObjs);
    }
    return *this;
  }

  template <class InputIterator>
  void assign(InputIterator first, InputIterator last) {
    clear(); 
    insert(begin(), first, last);
  }
  void assign(size_type n, const ElemLink& link) {
    clear(); 
    insert(begin(), n, link);
  }

  bool toPersistent();

  // Also return list of shortrefs.
  bool toPersistent(std::vector<typename DataLinkVector::size_type>& shortrefs);

  // Just the DataLink part of toPersistent().
  bool toPersistentDL();
  bool toTransient();
  void doRemap();
  //@}

  /// \name vector iterators
  //@{

  iterator begin() { return iterator(m_shortRefs.begin(), Short2LongRef()); }
  const_iterator begin() const { 
    return const_iterator(m_shortRefs.begin(), Short2LongRef()); 
  }
  iterator end() { return iterator(m_shortRefs.end(), Short2LongRef()); }
  const_iterator end() const { 
    return const_iterator(m_shortRefs.end(), Short2LongRef()); 
  }
  reverse_iterator rbegin() { return reverse_iterator(m_shortRefs.begin(), Short2LongRef()); }
  const_reverse_iterator rbegin() const { 
    return const_reverse_iterator(m_shortRefs.begin(), Short2LongRef()); 
  }
  reverse_iterator rend() { return reverse_iterator(m_shortRefs.end(), Short2LongRef()); }
  const_reverse_iterator rend() const { 
    return const_reverse_iterator(m_shortRefs.end(), Short2LongRef()); 
  }
  //@}

  /// \name vector capacity
  //@{
  size_type size() const { return m_shortRefs.size(); }
  size_type max_size() const { return m_shortRefs.max_size(); }
  void resize(size_type sz, const ElemLink& link = ElemLink());
  size_type capacity() const { return m_shortRefs.capacity(); }
  bool empty() const { return 0 == size(); }
  void reserve(size_type n) { return m_shortRefs.reserve(n); }
  //@}

  /// \name vector element accessors. NB only CONST accessors
  //@{
  // reference operator[](size_type n);
  const_reference 
  operator[](size_type n) const { return m_shortRefs[n].elementLink(); } 
  // reference at(size_type n);
  const_reference 
  at(size_type n) const { return m_shortRefs.at(n).elementLink(); } 
  // reference front();
  const_reference front() const{ return m_shortRefs.front().elementLink(); } 
  // reference back();
  const_reference back() const{ return m_shortRefs.back().elementLink(); } 
  //@}

  /// \name vector Modifiers
  //@{
  void push_back(const ElemLink& link) { 
    addHostDObj(link);
    m_shortRefs.push_back(ElemLinkRef(link));
  }
  void pop_back() { //FIXME CHECK
    removeHostObj(back());
    m_shortRefs.pop_back();
  }

  // Add an element by indices.
  // (Mostly for use from persistency.)
  void push_back (typename DataLinkVector::size_type nameIndex,
                  typename ElemLinkRef::index_type elementIndex);

  iterator insert(iterator position, const ElemLink& link);
  void insert(iterator position, size_type n, const ElemLink& link);

  iterator erase(iterator position);
  iterator erase(iterator first, iterator last);

  void swap(ElemLinkVec& vec) {
    m_hostDObjs.swap(vec.m_hostDObjs);
    m_shortRefs.swap(vec.m_shortRefs);
  }
  
  void clear() {
    m_hostDObjs.clear();
    m_shortRefs.clear();
  }
  //@}

private:
  /// Function setting up the object for forward indexing types
  void toTransient( uint64_t& dummy );
  void toTransient( uint32_t& dummy );
  /// Function taking care of all the other indexing types (no direct ROOT I/O)
  template< typename INDEX_TYPE >
  void toTransient( INDEX_TYPE& dummy );

  ///remove host of link from list. O(N) in m_hostDObjs (which is small)
  void removeHostObj(const ElemLink& 
#ifdef __ELVDEBUG
		     link
#endif    
    ) {
#ifdef __ELVDEBUG
      std::cout << "DUMMY removeHostDObj called for link  " 
		<< link.dataID() << "/" << link.index() << std::endl;
#endif    
      //FIXME this is a dummy until we find how to remove an host w/o
      //FIXME screwing up the otherElemLinkRefs
      //FIXME    m_hostDObjs.erase(findHostDObj(link));
  }

  ///add host of link to list. No duplicates. O(N) in m_hostDObjs
  void addHostDObj(const ElemLink& link);

  ///get a short ref iterator from an iterator
  typename RefVector::const_iterator 
  shortIterFromLong(const_iterator longIter) const {
    typename RefVector::const_iterator ret(m_shortRefs.begin());
    advance(ret, distance(begin(), longIter));
#ifdef __ELVDEBUG
    std::cout << "shortIterFromLong(const version) called for " 
	      << longIter->dataID() << "/" << longIter->index() 
	      << " 	advance by  " << distance(begin(), longIter) 
	      << " result is 	" << ret->dataID() << "/" << ret->index() << std::endl;
#endif	
    return ret;
  }

  ///get a short ref iterator from an iterator
  typename RefVector::iterator 
  shortIterFromLong(iterator longIter) {
    typename RefVector::iterator ret(m_shortRefs.begin());
    advance(ret, distance(begin(), longIter));
#ifdef __ELVDEBUG
    std::cout << "shortIterFromLong called for " 
	      << longIter->dataID() << "/" << longIter->index() 
	      << " advance by  " << distance(begin(), longIter) 
	      << " result is " << ret->dataID() << "/" << ret->index() << std::endl;
#endif
    return ret;
  }

  /// \name FIXME don't know how to implement
  //@{
  template <class InputIterator>
  ElementLinkVector(InputIterator first, InputIterator last);
  template <class InputIterator>
  void insert(iterator position, InputIterator first, InputIterator last);
  //@}
  
  ///access m_shortRefs
  friend bool operator == <>(const ElemLinkVec&, const ElemLinkVec&);
  ///access m_shortRefs
  friend bool operator < <>(const ElemLinkVec&, const ElemLinkVec&);

  /// Needed for the simplified Reflex class name
  typedef typename
  ROOT_SELECTION_NS::ElementLinkVector< DOBJ>::self DictSel;

}; // class ElementLinkVector

template <typename DOBJ>
  ElementLinkVector<DOBJ>::ElementLinkVector() : ElementLinkVectorBase() { 
}

/*
 * The following piece of code makes the Reflex dictionary think of
 * "ElementLinkVector< T, DataProxyStorage< T >,
 *                     SG::GenerateIndexingPolicy< T >::type >"
 * simply as "ElementLinkVector< T >". This is vital for tricking
 * ROOT into reading this object's payload back into a different
 * ElementLink implementation in vanilla ROOT.
 */
ENTER_ROOT_SELECTION_NS

template< class STORABLE >
struct ElementLinkVector : public SelectNoInstance
{
  typedef ElementLinkVector< STORABLE> self;
  /// Mark all transient members:
  ROOT_SELECTION_NS::MemberAttributes< kTransient > m_shortRefs;
  ROOT_SELECTION_NS::MemberAttributes< kTransient > m_hostDObjs;
};

EXIT_ROOT_SELECTION_NS

// Hide the rest from the dictionary generator:
#ifndef __GCCXML__

#include "AthLinks/ElementLinkVector.icc"
#include "AthLinks/tools/SGELVRef.icc"  /* to avoid circular deps */

/// \name vector comparison operators
//@{
template <typename DOBJ>
bool
operator <(const ElementLinkVector<DOBJ>& lhs,
	   const ElementLinkVector<DOBJ>& rhs) {
  return (lhs.m_shortRefs < rhs.m_shortRefs);
}
template <typename DOBJ>
bool
operator >(const ElementLinkVector<DOBJ>& lhs,
	   const ElementLinkVector<DOBJ>& rhs) {
  return rhs < lhs;
}
template <typename DOBJ>
bool
operator ==(const ElementLinkVector<DOBJ>& lhs,
	    const ElementLinkVector<DOBJ>& rhs) {
  return (lhs.m_shortRefs == rhs.m_shortRefs);
}
template <typename DOBJ>
bool
operator !=(const ElementLinkVector<DOBJ>& lhs,
	    const ElementLinkVector<DOBJ>& rhs) {
  return !operator==(lhs, rhs);
}
//FIXME ops <= , => etc
//@}

namespace std {
template <typename DOBJ>
void
swap(ElementLinkVector<DOBJ>& lhs,
	  ElementLinkVector<DOBJ>& rhs) {
#ifdef __ELVDEBUG
  std::cout << "std::swap called for lhs " << std::hex << &lhs 
	    << " rhs " << &rhs << std::dec << std::endl;  
#endif
  lhs.swap(rhs);
}
}

#endif // not __GCCXML__
#endif   /*ATHLINKS_ELEMENTLINKVECTOR_H*/
