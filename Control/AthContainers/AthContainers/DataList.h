// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: DataList.h 772789 2016-09-12 15:55:30Z ssnyder $

/**
 * @file AthContainers/DataList.h
 * @author Srini Rajagopalan, Sebastien Binet - ATLAS Collaboration
 * @date February 2006; rewritten from earlier version.
 * @brief An STL list of pointers that by default owns its pointed-to
 *        elements
 *
 * For further information, see
 *    <https://twiki.cern.ch/twiki/bin/view/Atlas/DataVector>
 *
 * A @c DataList<T> acts like a @c std::list<T*>, except that it can
 * optionally manage the memory that it contains. The constructors take
 * an (optional) extra argument, which can be either @c SG::OWN_ELEMENTS
 * or @c SG::VIEW_ELEMENTS (defaulting to @c SG::OWN_ELEMENTS except for a
 * copy constructor). This tells whether the @c DataList owns its contained
 * elements or not.
 *
 * If a @c DataList owns its elements, then they are deleted when the
 * container itself is. Further, they are deleted by actions which 
 * erase elements from the container (i.e.: @c erase(), @c pop_back() ).
 * A replacement (such as <code> (*l.front()) = new T; </code>) will result
 * in the old element being deleted and the container taking ownership of
 * the new element. It is an error to assign directly between two
 * owning containers (<code> (*list1.front()) = (*list2.front());</code>).
 *
 * Beware of ownership issues when modify a @c DataList .
 * Obviously you should not delete explicitly a @c DataList element
 * (because this is already taken care of by this object).
 * A @c DataList should never have two elements pointing to the same object.
 * This may seem obvious but certain STL algorithms (eg: @c remove_if ) may
 * leave a @c DataList with two copies of the same element in the "left-over"
 * range. To avoid a crash when clearing the list (eg: in the destructor we 
 * have introduced a @f$ n\log n @f$ helper function that searches and removes
 * duplicates in the @c DataList . This is used by the destructor by @c clear()
 * and by <code>erase(first, last)</code>. As this may change in the future to
 * improve performance, do not rely on this functionality and do avoid 
 * introducing duplicated elements in a @c DataList .
 * 
 * All these cautions do not apply when a @c DataList it is created with 
 * the flag @c SG::VIEW_ELEMENTS (see <code>enum OwnershipPolicy</code>)
 * and hence does not own its elements. This is typically used
 * to have @c DataList elements allocated by @c DataPool.
 * Otherwise consider the cleaner alternative of using a @c list<T*>.
 *
 * The interface for @c DataList should be mostly compatible with that
 * of @c std::list.  There are a few differences which should not make
 * much difference in practice.  For example, methods which would
 * return a reference return a proxy object instead.  Also @c value_type
 * is used instead of @c const_reference; this is justified by the fact
 * that the elements are always pointers.
 *
 * Note that algorithms which modify their range may not work
 * correctly if the container owns its contents.  Specializations
 * that work properly for @c DataList are available for some algorithms.
 * These include:
 *  - @c std::remove
 *  - @c std::remove_if
 *  - @c std::unique
 *  - @c std::reverse
 *  - @c std::rotate
 *  - @c std::partition
 *  - @c std::stable_partition
 *
 * There are a few other additions to the standard @c std::list interface.
 *  - The method @c stdcont may be used to get access to the underlying
 *    @c std::list representation.
 *  - The type @c PtrList is the type of the underlying @c std::List.
 *    @c BaseContainer is a synonym for this.
 *  - The method @c ownPolicy returns the ownership policy of the container.
 *  - An additional overload of @c clear() is provided that takes as
 *    an argument a new ownership policy for the container.  This is
 *    the only way to change the ownership policy.
 *
 * Note that since @c DataList\<T> has an element type of @c T*,
 * it is not possible to directly insert a <code>const T*</code>.
 * If you want to do that, see @c ConstDataList.  (In some cases,
 * such as if the destination container is not being recorded in StoreGate,
 * it may be more appropriate to simply use a
 * <code>std::list<const T*></code>.)  Don't just use a @c const_cast!
 *
 * Finally, @c DataList's may inherit from one another.
 * If you have class @c D which derives from class @c B, you can set things
 * up so that @c DataList\<D> derives from @c DataList\<B>.  This allows
 * you do to the same sort of conversions on the @c DataList's as on the
 * element pointers themselves.  The key to doing this is to add the
 * declaration
 *
 *@code
 *   DATALIST_BASE (D, B);
 @endcode
 *
 * before using @c DataList\<D>.  A few caveats about doing this.
 * The pointers are actually stored in the base @c DataList instance,
 * and the type that @c stdcont returns will reflect this.
 * For example, in the example given above, @c DataList<D>::stdcont()
 * will return a reference to std::list\<B*>.  Second, in order
 * to preserve the invariant
 * that a @c DataList\<D> contains only elements that actually derive
 * from @c D, while at the same time not requiring that the contained
 * objects be polymorphic, there is a restriction that you cannot
 * insert into a @c DataList if you're not referring to it as the
 * most derived type (even if such an insertion would not actually
 * break the invariant).  This is implemented as a runtime check.
 *
 * Example:
 *
 *@code
 *   DataList<D> ld;
 *   ld.push_back (new D);  // This is ok.
 *   ld.push_back (new B);  // This will give a compilation error
 *                             (it would break the invariant).
 *   DataList<B>& lb = ld;
 *   lb.push_back (new B);  // This will give a run-time error
 *                             (it breaks the invariant).
 *   lb.push_back (new D);  // This will also give a run-time error.
 *                             (It's actually ok, but there's no good way
 *                             to distinguish it from the previous case.)
 @endcode
 *
 * Note also this (related to a common atlas idiom).  If we have the above,
 * and also:
 *
 *@code
 *   class B_List : public DataList<B> { ... };
 *   class D_List : public DataList<D> { ... };
 @endcode
 *
 * Then a @c D_List will be convertible to a DataList\<B>, but _not_
 * to a @c B_List.
 *
 * Multiple and virtual inheritance are also supported.  In this case,
 * use <code>DATALIST_VIRTBASES</code><em>n</em> (where @e n is 1, 2, or 3)
 * instead of @c DATALIST_BASE.  Example: Given:
 *
 *@code
 *   class M { ... };
 *   class N : virtual public M { ... };
 *   class O : virtual public M { ... };
 *   class P : virtual public N, virtual public O { ... };
 @endcode
 *
 * declare this with
 *
 *@code
 *   DATALIST_VIRTBASES1(N, M);
 *   DATALIST_VIRTBASES1(O, M);
 *   DATALIST_VIRTBASES2(P, N, O);
 @endcode
 *
 * There is a restriction that there must be a unique base class that
 * does not derive from anything else.  For example, the diamond configuration
 * above is ok, but this would not be:
 *
 *@code
 *   class L { ... };
 *   class M { ... };
 *   class N : virtual public M, virtual public L { ... };
 *   class O : virtual public M { ... };
 *   class P : virtual public N, virtual public O { ... };
 *
 *   DATALIST_VIRTBASES2(N, M, L);
 *   DATALIST_VIRTBASES1(O, M);
 *   DATALIST_VIRTBASES2(P, N, O);
 @endcode
 *
 * Note, however, that you don't have to tell @c DataList about the complete
 * hierarchy; leaving the @c L out of @c DATALIST_VIRTBASES would work
 * (you just wouldn't be able to convert to @c DataList\<L> ).
 *
 * If you use @c DATALIST_VIRTBASES, there is an additional time penalty
 * to retrieve elements from the collection.  This does not apply
 * for @c DATALIST_BASES.
 *
 * All applicable @c DATALIST_* macros must be visible at the point at which
 * a @c DataList is instantiated.  A confusing compilation error is
 * likely to result otherwise.  Note that this means that if you have
 * the @c DATALIST_* macros within a container header file, then
 * the header for the derived container must include the header
 * for the base container.   Be alert to this when converting existing
 * code to use the inheritance scheme.  For example, if class D2 derives
 * from D which derives from B:
 *
 * BList.h:
 *@code
 *  #include "B.h"
 *  #include "DataList.h"
 *  typedef DataList<B> BVec;
 @endcode
 *
 * DList.h:
 *@code
 *  #include "D.h"
 *  #include "DataList.h"
 *  DATALIST_BASE(D,B);
 *  typedef DataList<D> DVec;
 @endcode
 *
 * D2List.h:
 *@code
 *  #include "D2.h"
 *  #include "DataList.h"
 *  #include "DList.h" // This is required
 *  DATALIST_BASE(D2,D);
 *  typedef DataList<D2> DVec;
 @endcode
 *
 * Using @c DATALIST_BASE will also set up the corresponding @c SG::BaseInfo
 * definitions, both for the vectors themselves and for the contained objects.
 */
 
#ifndef ATHCONTAINERS_DATALIST_H
#define ATHCONTAINERS_DATALIST_H

// Make sure that this header is not used outside of Athena:
#ifdef XAOD_STANDALONE
#error "DataList is not supported outside of Athena!"
#endif // XAOD_STANDALONE


// For feature tests.
#define HAVE_CONSTDATAVECTOR


// STL includes
#include <list>
#include <typeinfo>
#include <functional>
#include <iostream>
#include <algorithm> // for std::swap, find, find_if
#include <iterator>
#include <type_traits>

// Boost includes
#include <boost/static_assert.hpp>
#include <boost/type_traits.hpp>
#include <boost/iterator/iterator_adaptor.hpp>

// Kernel and StoreGate includes
#include "SGTools/BaseInfo.h"
#include "SGTools/DataBucketTraitFwd.h"
#include "SGTools/ClassName.h"

#include "AthContainers/DataVector.h"
#include "AthContainers/OwnershipPolicy.h"
#include "AthContainers/exceptions.h"
#include "AthContainers/tools/DVLNoBase.h"
#include "AthContainers/tools/DVLInfo.h"
#include "AthContainers/tools/DVLDataBucket.h"
#include "AthContainers/tools/DVLCast.h"
#include "AthContainers/tools/DVLIterator.h"
#include "AthContainers/tools/DVL_iter_swap.h"
#include "AthContainers/tools/DVL_algorithms.h"
#include "AthContainers/tools/ElementProxy.h"
#include "AthContainers/tools/IsMostDerivedFlag.h"
#include "AthLinks/tools/selection_ns.h"


ENTER_ROOT_SELECTION_NS
template <class T, class BASE> class DataList;
EXIT_ROOT_SELECTION_NS


namespace DataList_detail {


// These are the intermediate classes from which @c DataList derives
// in the case of multiple or virtual derivation.  The actual
// definitions are in the @c icc file.  They have the following
// properties:
//   - They derive from those of @c B1, @c B2, and @c B3 that are not
//     @c NoBase.
//   - They perform (static) checking to ensure that there's a unique
//     base class in the hierarchy.
//   - They make the following types available from the base classes:
//     - @c PtrList
//     - @c BaseContainer
//     - @c size_type
//     - @c difference_type
//     - @c allocator_type
//   - @c has_virtual is defined as @c true.
//   - An override of @c dl_typeid is defined, to prevent ambiguities.
template <class B1,
          class B2=DataModel_detail::NoBase,
	  class B3=DataModel_detail::NoBase> struct VirtBases;

} // end namespace DataList_detail

/**
 * @brief Derivation information for @c DataList.
 *
 * Specializations of this class represent the derivation information that
 * @c DataList needs. They have the property that 
 * <code>DataListBase<T>::Base</code> is either @c NoBase or it is the class 
 * from which @c DataList\<T> should derive.
 */
template <class T>
struct DataListBase
{
  typedef DataModel_detail::NoBase Base;
};

/**
 * @brief Declare base class info to @c DataList.
 *        Single, non-virtual derivation.
 *
 * <code>DATALIST_BASE(D, B)</code> says that @c D derives
 * non-virtually from @c B.
 *
 * This macro creates an appropriate specialization of @c DataListBase.
 */
#define DATALIST_BASE(T, BASE)          \
template <> struct DataListBase<T>      \
{ typedef DataList<BASE> Base; };       \
SG_BASE(DataList<T>, DataList<BASE>); \
template struct DataList_detail::DVLEltBaseInit<T>


/**
 * @brief Declare base class info to @c DataList.
 *        Single, virtual derivation.
 *
 * <code>DATALIST_VIRTBASES(D, B1)</code> says that @c D derives
 * virtually from @c B1.
 *
 * This macro creates an appropriate specialization of @c DataListBase.
 */
#define DATALIST_VIRTBASES1(T, B1)                  \
template <> struct DataListBase<T>                  \
{ typedef DataList_detail::VirtBases<B1> Base; };   \
SG_BASES1(DataList<T>, SG_VIRTUAL(DataList<B1>)); \
template struct DataList_detail::DVLEltBaseInit<T>



/**
 * @brief Declare base class info to @c DataList.
 *        Multiple derivation.
 *
 * <code>DATALIST_VIRTBASES(D, B1)</code> says that @c D derives
 * from both @c B1 and @c B2.
 *
 * This macro creates an appropriate specialization of @c DataListBase.
 */
#define DATALIST_VIRTBASES2(T, B1, B2)                \
template <> struct DataListBase<T>                    \
{ typedef DataList_detail::VirtBases<B1, B2> Base; }; \
SG_BASES2(DataList<T>, SG_VIRTUAL(DataList<B1>),      \
                       SG_VIRTUAL(DataList<B2>)); \
template struct DataList_detail::DVLEltBaseInit<T>


/**
 * @brief Declare base class info to @c DataList.
 *        Multiple derivation.
 *
 * <code>DATALIST_VIRTBASES(D, B1)</code> says that @c D derives
 * from all of @c B1, @c B2, and @c B3.
 *
 * This macro creates an appropriate specialization of @c DataListBase.
 */
#define DATALIST_VIRTBASES3(T, B1, B2, B3)            \
template <> struct DataListBase<T>                    \
{ typedef DataList_detail::VirtBases<B1, B2, B3> Base; }; \
SG_BASES3(DataList<T>, SG_VIRTUAL(DataList<B1>),          \
		       SG_VIRTUAL(DataList<B2>),          \
		       SG_VIRTUAL(DataList<B3>)); \
template struct DataList_detail::DVLEltBaseInit<T>


template <class DL> class ConstDataList;
template <class DL> void test2_assignelement1();


/**
 * @brief Derived @c DataList\<T>.
 *
 * This is used for the case where @c T derives
 * from other classes.  The list of pointers is actually held
 * in the (unique) base class of the hierarchy.
 *
 * See the file comments for full details.
 */
template <class T, class BASE = typename DataListBase<T>::Base>
class DataList : public BASE
{
public:
  typedef BASE DataList_BASE;
  typedef BASE DVL_BASE;

  /// This is true for any @c DataList class if we need to use virtual
  /// derivation to get to the base @c DataList class.
  static const bool has_virtual = BASE::has_virtual;

  /// This is the type of the underlying @c std::list
  /// (what @c stdcont returns).
  typedef typename BASE::PtrList PtrList;
  typedef typename BASE::PtrList BaseContainer;


public:
  // Standard types required for the container interface.
  typedef T*& reference;
  typedef T* const & const_reference;
  typedef typename BASE::size_type size_type;
  typedef typename BASE::difference_type difference_type;
  typedef T* value_type;
  typedef typename BASE::allocator_type allocator_type;
  typedef T** pointer;
  typedef T* const * const_pointer;

  typedef const T* const_value_type;

  /// The @c T value used as the template parameter.
  /// Note that this is different from @c value_type (that's @c T*).
  typedef T base_value_type;

  /// This type is used to proxy lvalue accesses to @c DataList
  /// elements, in order to handle ownership.
  typedef DataModel_detail::ElementProxy<DataList> ElementProxy;


  /// Standard @c const_iterator.
  typedef typename DataModel_detail::const_iterator<DataList>
    const_iterator;

  /// Standard @c iterator.  Note that lvalue references here will yield
  /// an @c ElementProxy, not a @c reference.
  typedef typename DataModel_detail::iterator<DataList> iterator;


  /// Standard @c const_reverse_iterator.
  typedef typename std::reverse_iterator<const_iterator>
    const_reverse_iterator;

  /// Standard @c reverse_iterator.  Note that lvalue references here will
  /// yield an @c ElementProxy, not a @c reference.
  typedef typename std::reverse_iterator<iterator>
    reverse_iterator;


  //========================================================================
  /** @name Constructors, destructors, assignment. */
  //@{


  /**
   * @brief Default constructor.
   * @param ownPolicy The ownership mode for the container.
   *
   * By default, a @c DataList will own its elements.
   * To avoid this, pass @c SG::VIEW_ELEMENTS for @a ownPolicy.
   */
  explicit DataList( SG::OwnershipPolicy ownPolicy = SG::OWN_ELEMENTS );


  /**
   * @brief Sized constructor.
   * @param n The size of the container.
   * @param ownPolicy The ownership mode for the container.
   *
   * Note that unlike the standard list constructor, you can't specify
   * an initial value here.  The container will be initialized with 0's.
   *
   * By default, a @c DataList will own its elements.
   * To avoid this, pass @c SG::VIEW_ELEMENTS for @a ownPolicy.
   */
  explicit DataList( size_type n, /* value_type pElem = 0,*/
 		     SG::OwnershipPolicy ownPolicy = SG::OWN_ELEMENTS );


  /**
   * @brief Copy constructor.
   * @param rhs The container from which to copy.
   *
   * This is a `shallow' copy; the new container will not own its elements.
   */
  // The copy constructor for derived classes is deliberately omitted,
  // as otherwise we get warnings about not calling the copy constructors
  // for base classes, which are problematic when we have virtual
  // inheritance.  Most of what needs doing is done in the base class anyway,
  // except for setting @c m_isMostDerived.  We arrange for these flags
  // to all get set to false; they'll get set correctly when
  // @c testInsert is called.


  /**
   * @brief Constructor from iterators.
   * @param first The start of the range to put in the new container.
   * @param last The end of the range to put in the new container.
   * @param ownPolicy The ownership mode for the container.
   *
   * By default, a @c DataList will own its elements (and take ownership
   * of the pointers passed to this constructor).
   * To avoid this, pass @c SG::VIEW_ELEMENTS for @a ownPolicy.
   */
  template <class InputIterator>
  DataList( InputIterator first, InputIterator last,
     	    SG::OwnershipPolicy ownPolicy = SG::VIEW_ELEMENTS );


  /**
   * @brief Assignment operator.
   * @param rhs The DataList from which to assign.
   * @return This object.
   *
   * This is a `shallow' copy; after the completion of this, the @c DataList
   * will not own its elements.  Any elements it owned prior to this call
   * will be released.
   *
   * Note: this method may only be called using the most derived
   * @c DataList in the hierarchy.
   */
  DataList& operator=( const DataList& rhs );


  /**
   * @brief Assign from iterators.
   * @param first The start of the range to put in the container.
   * @param last The end of the range to put in the container.
   *
   * Any existing owned elements will be released.
   * The @c DataList's ownership policy determines whether it will take
   * ownership of the new elements.
   */
  template <class InputIterator>
  void assign( InputIterator first, InputIterator last );


  // Destructor is inherited.


  //@}
  //========================================================================
  /** @name Size and capacity. */
  //@{


  /**
   * @fn size_type size() const
   * @brief Returns the number of elements in the collection.
   */
  // This is inherited from the base class.


  /**
   * @fn size_type max_size() const
   * @brief Returns the @c size() of the largest possible collection.
   */
  // This is inherited from the base class.


  /**
   * @fn void resize(size_type sz)
   * @brief Resizes the collection to the specified number of elements.
   * @param sz The new size of the collection.
   *
   * Note that this function differs from the standard in that it does
   * not allow specifying the value of any inserted elements.
   * They will always be 0.
   *
   * If the container is shrunk, elements will be deleted as with @c erase().
   */
  // This is inherited from the base class.
  

  /**
   * @fn bool empty() const
   * @brief Returns @c true if the collection is empty.
   */
  // This is inherited from the base class.


  //@}
  //========================================================================
  /** @name Element access. */
  //@{

  /**
   * @brief Access the first element in the collection as an rvalue.
   * @return The first element in the collection.
   *
   * No checking is done to ensure that the container is not empty.
   * Note that we return a @c const @c T* rather than a reference.
   */
  const T* front() const;


  /**
   * @brief Access the last element in the collection as an rvalue.
   * @return The last element in the collection.
   *
   * No checking is done to ensure that the container is not empty.
   * Note that we return a @c const @c T* rather than a reference.
   */
  const T* back() const;


  /**
   * @brief Access the first element in the collection as an lvalue.
   * @return Proxy to the first element in the collection.
   *
   * No checking is done to ensure that the container is not empty.
   * Note that we return a proxy object rather than a reference;
   * the proxy will handle deleting an owned element if it's assigned to.
   */
  ElementProxy front ();


  /**
   * @brief Access the last element in the collection as an lvalue.
   * @return Proxy to the last element in the collection.
   *
   * No checking is done to ensure that the container is not empty.
   * Note that we return a proxy object rather than a reference;
   * the proxy will handle deleting an owned element if it's assigned to.
   */
  ElementProxy back ();


  //@}
  //========================================================================
  /** @name Iterator creation. */
  //@{


  /**
   * @brief Return a @c const_iterator pointing at the beginning
   *        of the collection.
   * @return A @c const_iterator.
   *
   * Note that dereferencing the iterator will yield a @c const @c T* rather
   * than a reference.
   */
  const_iterator begin() const;


  /**
   * @brief Return a @c const_iterator pointing past the end
   *        of the collection.
   * @return A @c const_iterator.
   *
   * Note that dereferencing the iterator will yield a @c const @c T* rather
   * than a reference.
   */
  const_iterator end() const;


  /**
   * @brief Return an @c iterator pointing at the beginning
   *        of the collection.
   * @return An @c iterator.
   *
   * Note that dereferencing the iterator will yield a proxy rather
   * than a reference; the proxy will handle deleting an owned element
   * if it's assigned to.
   */
  iterator begin();


  /**
   * @brief Return an @c iterator pointing past the end
   *        of the collection.
   * @return An @c iterator.
   *
   * Note that dereferencing the iterator will yield a proxy rather
   * than a reference; the proxy will handle deleting an owned element
   * if it's assigned to.
   */
  iterator end();


  /**
   * @brief Return a @c const_reverse_iterator pointing past the end
   *        of the collection.
   * @return A @c const_reverse_iterator.
   *
   * Note that dereferencing the iterator will yield a @c const @c T* rather
   * than a reference.
   */
  const_reverse_iterator rbegin() const;


  /**
   * @brief Return a @c const_reverse_iterator pointing at the beginning
   *        of the collection.
   * @return A @c const_reverse_iterator.
   *
   * Note that dereferencing the iterator will yield a @c const @c T* rather
   * than a reference.
   */
  const_reverse_iterator rend() const;


  /**
   * @brief Return a @c reverse_iterator pointing past the end
   *        of the collection.
   * @return A @c reverse_iterator.
   *
   * Note that dereferencing the iterator will yield a proxy rather
   * than a reference; the proxy will handle deleting an owned element
   * if it's assigned to.
   */
  reverse_iterator rbegin();


  /**
   * @brief Return a @c reverse_iterator pointing at the beginning
   *        of the collection.
   * @return A @c reverse_iterator.
   *
   * Note that dereferencing the iterator will yield a proxy rather
   * than a reference; the proxy will handle deleting an owned element
   * if it's assigned to.
   */
  reverse_iterator rend();


  //@}
  //========================================================================
  /** @name Insertion operations. */
  //@{


  /**
   * @brief Add an element at the beginning of the collection.
   * @param pElem The element to add to the collection.
   *
   * The container's ownership policy will determine if it takes ownership
   * of the new element.
   *
   * Note: this method may only be called using the most derived
   * @c DataList in the hierarchy.
   */
  void push_front( value_type pElem );


  /**
   * @brief Add an element to the end of the collection.
   * @param pElem The element to add to the collection.
   *
   * The container's ownership policy will determine if it takes ownership
   * of the new element.
   *
   * Note: this method may only be called using the most derived
   * @c DataList in the hierarchy.
   */
  void push_back( value_type pElem );


  /**
   * @brief Add a new element to the collection.
   * @param position Iterator before which the element will be added.
   * @param pElem The element to add to the collection.
   * @return An iterator that points to the inserted data.
   *
   * The container's ownership policy will determine if it takes ownership
   * of the new element.
   *
   * Note: this method may only be called using the most derived
   * @c DataList in the hierarchy.
   */
  iterator insert( iterator position, value_type pElem );


  /**
   * @brief Add a group of new elements to the collection.
   * @param position Iterator before which the element will be added.
   * @param first The start of the range to put in the container.
   * @param last The end of the range to put in the container.
   *
   * The container's ownership policy will determine if it takes ownership
   * of the new element.
   *
   * Note: this method may only be called using the most derived
   * @c DataList in the hierarchy.
   */
  template <class InputIterator>
  void insert( iterator position, InputIterator first, InputIterator last );


  //@}
  //========================================================================
  /** @name Erasure operations. */
  //@{


  /**
   * @brief Remove element at a given position.
   * @param position Iterator pointing to the element to be removed.
   * @return An iterator pointing to the next element (or @c end()).
   *
   * If the container owns its elements, then the pointed-to element
   * will be deleted.
   */
  iterator erase(iterator position);


  /**
   * @brief Remove a range of elements.
   * @param first Iterator pointing to the first element to be removed.
   * @param last Iterator pointing one past the last element to be removed.
   * @return An iterator pointing to the element pointed to by @a last
   *         prior to erasing (or @c end()).
   *
   * If the container owns its elements, then the removed elements
   * will be deleted.  Any duplicates will be removed in this process,
   * but don't rely on this.
   */
  iterator erase(iterator first, iterator last);


  /**
   * @fn void pop_back()
   * @brief Remove the last element from the collection.
   *
   * If the container owns its elements, then the removed element
   * will be deleted.
   */
  // This is inherited from the base class.


  /**
   * @fn void pop_front()
   * @brief Remove the first element from the collection.
   *
   * If the container owns its elements, then the removed element
   * will be deleted.
   */
  // This is inherited from the base class.


  /**
   * @fn void clear()
   * @brief Erase all the elements in the collection.
   *
   * If the container owns its elements, then the removed elements
   * will be deleted.  Any duplicates will be removed in this process,
   * but don't rely on this.
   */
  // This is inherited from the base class.


  //@}
  //========================================================================
  /** @name Swap and sort. */
  //@{


  /**
   * @brief Swap this collection with another.
   * @param rhs The collection with which to swap.
   *
   * Ownership is swapped along with the collection content.
   *
   * Note: this method may only be called using the most-derived
   * @c DataList in the hierarchy.  The @a rhs must also be
   * referenced using the most-derived @c DataList.
   */
  void swap( DataList& rhs );


  /**
   * @brief Swap the referents of two @c DataVector iterators.
   * @param a The first iterator for the swap.
   * @param b The second iterator for the swap/
   */
  static void iter_swap (iterator a, iterator b);


  /**
   * @brief Sort the container.
   *
   * This just sorts by pointer value, so it's probably not very useful.
   */
  void sort();


  /**
   * @brief Sort the container with a user-specified comparison operator.
   * @param comp Functional to compare two values.
   */
  template <class COMPARE>
  void sort(COMPARE comp);
  
  
  //@}
  //========================================================================
  /** @name List operations. */
  //@{


  /**
   *  @brief Insert contents of another list.
   *  @param position Iterator referencing the element to insert before.
   *  @param l  Source list.
   *
   *  The elements of @a l are inserted in constant time in front of
   *  the element referenced by @a position.  @a l becomes an empty
   *  list.
   */
  void splice( iterator position, DataList& l );

  /**
   *  @brief Insert element from another list.
   *  @param position  Iterator referencing the element to insert before.
   *  @param l  Source list.
   *  @param i  Iterator referencing the element to move.
   *
   *  Removes the element in list @a l referenced by @a i and
   *  inserts it into the current list before @a position.
   */
  void splice( iterator position, DataList& l, iterator i );


  /**
   *  @brief  Insert range from another list.
   *  @param  position  Iterator referencing the element to insert before.
   *  @param  l  Source list.
   *  @param  first  Iterator referencing the start of range in @a l.
   *  @param  last  Iterator referencing the end of range in @a l.
   *
   *  Removes elements in the range [@a first, @a last ) and inserts them
   *  before @a position in constant time.
   *
   *  Undefined if @a position is in [@a first, @a last ).
   */
  void splice( iterator position, DataList& l,
               iterator first,
               iterator last );


  /**
   *  @brief  Remove all elements equal to value.
   *  @param  value  The value to remove.
   *
   *  Removes every element in the list equal to @a value.
   *  Remaining elements stay in list order.
   */
  void remove( const value_type& value );


  /**
   *  @brief  Remove all elements satisfying a predicate.
   *  @param  p  Unary predicate function or object.
   *
   *  Removes every element in the list for which the predicate
   *  returns true.  Remaining elements stay in list order.
   */
  template<typename PREDICATE>
  void remove_if( PREDICATE p );


  /**
   *  @brief  Remove consecutive duplicate elements.
   *
   *  For each consecutive set of elements with the same value,
   *  remove all but the first one.  Remaining elements stay in
   *  list order.
   */
  void unique();


  /**
   *  @brief  Remove consecutive elements satisfying a predicate.
   *  @param  p  Binary predicate function or object.
   *
   *  For each consecutive set of elements [first,last) that
   *  satisfy predicate(first,i) where i is an iterator in
   *  [first,last), remove all but the first one.  Remaining
   *  elements stay in list order.
   */
  template<typename BINARY_PREDICATE>
  void unique( BINARY_PREDICATE p );


  /**
   *  @brief  Merge sorted lists.
   *  @param  l  Sorted list to merge.
   *
   *  Assumes that both @a l and this list are sorted according to
   *  operator<().  Merges elements of @a l into this list in
   *  sorted order, leaving @a l empty when complete.  Elements in
   *  this list precede elements in @a l that are equal.
   */
  void merge( DataList& l );


  /**
   *  @brief  Merge sorted lists according to comparison function.
   *  @param  l  Sorted list to merge.
   *  @param predicate Comparison function defining a sort order (which
   *  must be strictly weak ordering).
   *
   *  Assumes that both @a l and this @c DataList are sorted according to
   *  COMPARE predicate.  Merges elements of @a l into this list
   *  in sorted order, leaving @a l empty when complete.  Elements
   *  in this list precede elements in @a x that are equivalent
   *  according to StrictWeakOrdering().
   */
  template<typename COMPARE>
  void merge( DataList& l, COMPARE predicate );


  /**
   *  @brief  Reverse the elements in list.
   *
   *  Reverse the order of elements in the list in linear time.
   */
  void reverse();


  //@}
  //========================================================================
  /** @name Non-standard operations. */
  //@{


public:
  /**
   * @brief Swap one element out of the container.
   * @param pos The element in the container to swap.
   * @param newElem New element to put in the container.
   *                May be 0.
   * @param oldElem Reference to receive the element removed from the
   *                container.
   *
   * Reference @a oldElem is initialized with element @a pos of the
   * collection (no bounds checking).  Then element @a index is set
   * to @c newElem.  If the collection owns its elements, then it will
   * take ownership of @a newElem and release (without deleting)
   * the element returned through @a oldElem.
   *
   * Note: this method may only be called using the most derived
   * @c DataList in the hierarchy.
   */
  void swapElement(iterator pos, value_type newElem, reference oldElem);
  

  /**
   * @fn const PtrList& stdcont() const
   * @brief Return the underlying @c std::list of the container.
   * @return Reference to the @c std::list actually holding the collection.
   *
   * Note that @c DataList<T>::stdcont does not necessarily return
   * a @c std::list<T*> if @c DataList inheritance is being used.
   */
  // This is inherited from the base class.


  /**
   * @fn SG::OwnershipPolicy ownPolicy() const
   * @brief Return the ownership policy setting for this container.
   */
  // This is inherited from the base class.


  /**
   * @fn void clear (SG::OwnershipPolicy ownPolicy)
   * @brief Erase all the elements in the collection, and reset
   *        the ownership mode.
   * @param ownPolicy The new ownership policy of the container.
   *
   * If the container owns its elements, then the removed elements
   * will be deleted.  Any duplicates will be removed in this process,
   * but don't rely on this.
   */
  // This is inherited from the base class.


  /**
   * @brief Return the DV/DL info struct for this class.
   *
   * This can be used to make sure that it's instantiated.
   */
  static const DataModel_detail::DVLInfoBase& dvlinfo();


  /**
   * @brief Return the DV/DL info struct for this class.
   *
   * This can be used to make sure that it's instantiated.
   */
  virtual const DataModel_detail::DVLInfoBase& dvlinfo_v() const;


  //@}
  //========================================================================


  // Doxygen trick.  We want DataList<T> to show the complete interface,
  // including inherited methods.  Normally, doxygen would do this.
  // But it's not smart to figure out our inheritance relationships.
  // We can add a function with `fn', but doxygen won't actually
  // generate the documentation unless it also sees the declaration.
  // So here are declarations which should be visible to doxygen
  // but not to C++.
#ifndef __cplusplus
  size_type size() const;
  size_type max_size() const;
  void resize(size_type sz);
  bool empty() const;
  void pop_back();
  void pop_front();
  void clear();
  const PtrList& stdcont() const;
  SG::OwnershipPolicy ownPolicy() const;
  void clear (SG::OwnershipPolicy ownPolicy);
#endif // not __cplusplus


  /** @name Internal operations. */
  //@{


public:
  /**
   * @brief Reset indices / reorder aux data after elements have been permuted.
   * @param beg Start of the range of elements to process.
   * @param end End of the range of elements to process.
   *
   * This is a no-op for @c DataList.
   */
  void resortAux (iterator beg, iterator end);


  /**
   * @brief Test if we can insert; raise an exception if not.
   * @param op Description of the attempted operation.
   *
   * In order to maintain type-safety, we can only allow insertions
   * using the most-derived instance of @c DataList.  This checks
   * this by testing the @c m_isMostDerived, which is set by the constructors
   * to true only for the most-derived instance.
   * If the test fails, we call to potentially out-of-line code to continue.
   */
  void testInsert (const char* op);


  /**
   * @brief Test if we can insert; raise an exception if not.
   * @param op Description of the attempted operation.
   *
   * This continues the test of @c testInsert.  There is one case
   * where @c m_isMostDerived may not be set correctly.  If this container
   * was made via copy construction, then all the @c m_isMostDerived flags
   * will be false.  So we call @c setMostDerived to set the flags correctly
   * and test again.  If the test fails again, then we raise an exception.
   */
  void testInsertOol (const char* op);


protected:
  /**
   * @brief Clear @c m_isMostDerived for this instance and for all bases.
   *
   * Called from the constructor after setting @c m_isMostDerived.
   */
  void clearMostDerived();


  /**
   * @brief Set @c m_isMostDerived for this instance and clear it for all bases.
   *
   * Called from @c testInsert if the test fails.  The flag may not have
   * been set if this container was made via copy construction, so set
   * it appropriately now so we can test again.
   */
  virtual void setMostDerived();


private:
  friend class DataModel_detail::ElementProxy<DataList>;
  friend class ConstDataList<DataList>;
  friend void test2_assignelement1<DataList>();


  /**
   * @brief Find the most-derived @c DataList class in the hierarchy.
   * @return The @c type_info for the class for which this method gets run.
   *
   * In order to properly type-check insertions, we need to be able to find
   * the most-derived @c DataList class in the inheritance hierarchy.
   * That's the purpose of this function.
   * Every @c DataList defines this virtual method, so when it's
   * called, the one corresponding to the most-derived @c DataList
   * gets run.
   */
  virtual const std::type_info& dl_typeid() const;


  /**
   * @brief Handle element assignment.
   * @param pos Position in the container to assign.
   * @param newElem The new element to assign.
   *
   * The old element is freed if this container owns elements.
   */
  void assignElement (typename BaseContainer::iterator pos, value_type newElem);


  /**
   * @brief Handle element assignment from a base pointer.
   * @param pos Position in the container to assign.
   * @param newElem The new element to assign.
   *
   * The old element is freed if this container owns elements.
   */
  void assignBaseElement (typename BaseContainer::iterator pos,
                          typename BaseContainer::value_type newElem);


  /**
   * @brief Helper to shorten calls to @c DataList_detail::DVLCast.
   * @param p The value to convert.
   * @return The value as a @c const @c T*.
   *
   * The conversion will be done with @c static_cast if possible,
   * with @c dynamic_cast otherwise.
   */
  static
  const T* do_cast (const typename PtrList::value_type p);
  //@}


  /// This flag is true if this DL instance is the most-derived one.
  /// We set this to true in the top-level constructor; the constructor
  /// then calls clearMostDerived on the base classes.
  SG::IsMostDerivedFlag m_isMostDerived;


  /// The DV/DL info struct for this class.
  static DataModel_detail::DVLInfo<DataList<T> > s_info;


  typedef typename
    ROOT_SELECTION_NS::DataList<T, DataList_BASE>::self self;
};


template <class T, class BASE>
const bool DataList<T, BASE>::has_virtual;


/**
 * @brief Base specialization for @c DataList\<T>.
 *
 * This is used for the case where @c T does not derive
 * from other classes.  This is the class which actually holds
 * the list of pointers.
 *
 * See the file comments for full details.
 */
template <class T>
class DataList<T, DataModel_detail::NoBase>
{
public:
  typedef DataModel_detail::NoBase DataList_BASE;
  typedef DataModel_detail::NoBase DVL_BASE;

  /// Mark as a sequence, for DataLink / ElementLink.
  typedef std::true_type isSequence;

  /// This is the type of the underlying @c std::list
  /// (what @c stdcont returns).
  typedef std::list<T*> PtrList;
  typedef std::list<T*> BaseContainer;

  /// This is true for any @c DataList class if we need to use virtual
  /// derivation to get to the base @c DataList class.
  /// Since this @e is the base @c DataList class, set this
  /// unconditionally to @c false.
  static const bool has_virtual = false;

  // Standard types required for the container interface.
  typedef T*& reference;
  typedef T* const & const_reference;
  typedef typename PtrList::size_type size_type;
  typedef typename PtrList::difference_type difference_type;
  typedef T* value_type;
  typedef typename PtrList::allocator_type allocator_type;
  typedef T** pointer;
  typedef T* const * const_pointer;

  typedef const T* const_value_type;

  /// The @c T value used as the template parameter.
  /// Note that this is different from @c value_type (that's @c T*).
  typedef T base_value_type;

  /// This type is used to proxy lvalue accesses to @c DataList
  /// elements, in order to handle ownership.
  typedef DataModel_detail::ElementProxy<DataList> ElementProxy;


  /// Standard @c const_iterator.
  /// Here, we can just use the @c std::list iterator.
  typedef typename PtrList::const_iterator const_iterator;

  /// Standard @c iterator.  Note that lvalue references here will yield
  /// an @c ElementProxy, not a @c reference.
  typedef typename DataModel_detail::iterator<DataList> iterator;

  /// Standard @c const_reverse_iterator.
  /// Note: we can't just use the underlying PtrList::const_reverse_iterator
  /// here; otherwise, const_reverse_iterator won't be convertable to
  /// reverse_iterator.
  typedef typename std::reverse_iterator<const_iterator>
    const_reverse_iterator;

  /// Standard @c reverse_iterator.  Note that lvalue references here will
  /// yield an @c ElementProxy, not a @c reference.
  typedef typename std::reverse_iterator<iterator>
    reverse_iterator;


  //========================================================================
  /** @name Constructors, destructors, assignment. */
  //@{


  /**
   * @brief Default constructor.
   * @param ownPolicy The ownership mode for the container.
   *
   * By default, a @c DataList will own its elements.
   * To avoid this, pass @c SG::VIEW_ELEMENTS for @a ownPolicy.
   */
  explicit DataList( SG::OwnershipPolicy ownPolicy = SG::OWN_ELEMENTS );


  /**
   * @brief Sized constructor.
   * @param n The size of the container.
   * @param ownPolicy The ownership mode for the container.
   *
   * Note that unlike the standard list constructor, you can't specify
   * an initial value here.  The container will be initialized with 0's.
   *
   * By default, a @c DataList will own its elements.
   * To avoid this, pass @c SG::VIEW_ELEMENTS for @a ownPolicy.
   */
  explicit DataList( size_type n, /* value_type pElem = 0,*/
 		     SG::OwnershipPolicy ownPolicy = SG::OWN_ELEMENTS );


  /**
   * @brief Copy constructor.
   * @param rhs The container from which to copy.
   *
   * This is a `shallow' copy; the new container will not own its elements.
   */
  DataList( const DataList& rhs );


  /**
   * @brief Constructor from iterators.
   * @param first The start of the range to put in the new container.
   * @param last The end of the range to put in the new container.
   * @param ownPolicy The ownership mode for the container.
   *
   * By default, a @c DataList will own its elements (and take ownership
   * of the pointers passed to this constructor).
   * To avoid this, pass @c SG::VIEW_ELEMENTS for @a ownPolicy.
   */
  template <class InputIterator>
  DataList( InputIterator first, InputIterator last,
	    SG::OwnershipPolicy ownPolicy = SG::VIEW_ELEMENTS );


  /**
   * @brief Assignment operator.
   * @param rhs The @c DataList from which to assign.
   * @return This object.
   *
   * This is a `shallow' copy; after the completion of this, the @c DataList
   * will not own its elements.  Any elements it owned prior to this call
   * will be released.
   *
   * Note: this method may only be called using the most derived
   * @c DataList in the hierarchy.
   */
  DataList& operator=( const DataList& rhs );


  /**
   * @brief Assign from iterators.
   * @param first The start of the range to put in the container.
   * @param last The end of the range to put in the container.
   *
   * Any existing owned elements will be released.
   * The <code>DataList</code>'s ownership policy determines whether it will 
   * take ownership of the new elements.
   */
  template <class InputIterator>
  void assign( InputIterator first, InputIterator last );


  /**
   * @brief Destructor.
   *
   * If this container owns its elements, the contained elements will
   * be deleted as well.  Before doing this, the destructor will scan
   * for duplicate pointers (takes @f$ n\log n @f$ time); duplicates are only
   * destroyed once.  Duplicates should, however, be considered an error;
   * don't rely on this behavior.
   */
  virtual ~DataList();


  //@}
  //========================================================================
  /** @name Size and capacity. */
  //@{


  /**
   * @brief Returns the number of elements in the collection.
   */
  size_type size() const;


  /**
   * @brief Returns the @c size() of the largest possible collection.
   */
  size_type max_size() const;


  /**
   * @brief Resizes the collection to the specified number of elements.
   * @param sz The new size of the collection.
   *
   * Note that this function differs from the standard in that it does
   * not allow specifying the value of any inserted elements.
   * They will always be 0.
   *
   * If the container is shrunk, elements will be deleted as with @c erase().
   */
  void resize( size_type sz );


  /**
   * @brief Returns @c true if the collection is empty.
   */
  bool empty() const;


  //@}
  //========================================================================
  /** @name Element access. */
  //@{


  /**
   * @brief Access the first element in the collection as an rvalue.
   * @return The first element in the collection.
   *
   * No checking is done to ensure that the container is not empty.
   * Note that we return a @c const @c T* rather than a reference.
   */
  const T* front() const;


  /**
   * @brief Access the last element in the collection as an rvalue.
   * @return The last element in the collection.
   *
   * No checking is done to ensure that the container is not empty.
   * Note that we return a @c const @c T* rather than a reference.
   */
  const T* back() const;


  /**
   * @brief Access the first element in the collection as an lvalue.
   * @return Proxy to the first element in the collection.
   *
   * No checking is done to ensure that the container is not empty.
   * Note that we return a proxy object rather than a reference;
   * the proxy will handle deleting an owned element if it's assigned to.
   */
  ElementProxy front();


  /**
   * @brief Access the last element in the collection as an lvalue.
   * @return Proxy to the last element in the collection.
   *
   * No checking is done to ensure that the container is not empty.
   * Note that we return a proxy object rather than a reference;
   * the proxy will handle deleting an owned element if it's assigned to.
   */
  ElementProxy back();


  //@}
  //========================================================================
  /** @name Iterator creation. */
  //@{


  /**
   * @brief Return a @c const_iterator pointing at the beginning
   *        of the collection.
   * @return A @c const_iterator.
   *
   * Note that dereferencing the iterator will yield a @c const @c T* rather
   * than a reference.
   */
  const_iterator begin() const;


  /**
   * @brief Return a @c const_iterator pointing past the end
   *        of the collection.
   * @return A @c const_iterator.
   *
   * Note that dereferencing the iterator will yield a @c const @c T* rather
   * than a reference.
   */
  const_iterator end() const;


  /**
   * @brief Return an @c iterator pointing at the beginning
   *        of the collection.
   * @return An @c iterator.
   *
   * Note that dereferencing the iterator will yield a proxy rather
   * than a reference; the proxy will handle deleting an owned element
   * if it's assigned to.
   */
  iterator begin();


  /**
   * @brief Return an @c iterator pointing past the end
   *        of the collection.
   * @return An @c iterator.
   *
   * Note that dereferencing the iterator will yield a proxy rather
   * than a reference; the proxy will handle deleting an owned element
   * if it's assigned to.
   */
  iterator end();


  /**
   * @brief Return a @c const_reverse_iterator pointing past the end
   *        of the collection.
   * @return A @c const_reverse_iterator.
   *
   * Note that dereferencing the iterator will yield a @c const @c T* rather
   * than a reference.
   */
  const_reverse_iterator rbegin() const;


  /**
   * @brief Return a @c const_reverse_iterator pointing at the beginning
   *        of the collection.
   * @return A @c const_reverse_iterator.
   *
   * Note that dereferencing the iterator will yield a @c const @c T* rather
   * than a reference.
   */
  const_reverse_iterator rend() const;


  /**
   * @brief Return a @c reverse_iterator pointing past the end
   *        of the collection.
   * @return A @c reverse_iterator.
   *
   * Note that dereferencing the iterator will yield a proxy rather
   * than a reference; the proxy will handle deleting an owned element
   * if it's assigned to.
   */
  reverse_iterator rbegin();


  /**
   * @brief Return a @c reverse_iterator pointing at the beginning
   *        of the collection.
   * @return A @c reverse_iterator.
   *
   * Note that dereferencing the iterator will yield a proxy rather
   * than a reference; the proxy will handle deleting an owned element
   * if it's assigned to.
   */
  reverse_iterator rend();

  //@}
  //========================================================================
  /** @name Insertion operations. */
  //@{


  /**
   * @brief Add an element at the beginning of the collection.
   * @param pElem The element to add to the collection.
   *
   * The container's ownership policy will determine if it takes ownership
   * of the new element.
   *
   * Note: this method may only be called using the most derived
   * @c DataList in the hierarchy.
   */
  void push_front( value_type pElem );


  /**
   * @brief Add an element to the end of the collection.
   * @param pElem The element to add to the collection.
   *
   * The container's ownership policy will determine if it takes ownership
   * of the new element.
   *
   * Note: this method may only be called using the most derived
   * @c DataList in the hierarchy.
   */
  void push_back( value_type pElem );


  /**
   * @brief Add a new element to the collection.
   * @param position Iterator before which the element will be added.
   * @param pElem The element to add to the collection.
   * @return An iterator that points to the inserted data.
   *
   * The container's ownership policy will determine if it takes ownership
   * of the new element.
   *
   * Note: this method may only be called using the most derived
   * @c DataList in the hierarchy.
   */
  iterator insert( iterator position, value_type pElem );


  /**
   * @brief Add a group of new elements to the collection.
   * @param position Iterator before which the element will be added.
   * @param first The start of the range to put in the container.
   * @param last The end of the range to put in the container.
   *
   * The container's ownership policy will determine if it takes ownership
   * of the new element.
   *
   * Note: this method may only be called using the most derived
   * @c DataList in the hierarchy.
   */
  template <class InputIterator>
  void insert( iterator position, InputIterator first, InputIterator last );


  //@}
  //========================================================================
  /** @name Erasure operations. */
  //@{


  /**
   * @brief Remove element at a given position.
   * @param position Iterator pointing to the element to be removed.
   * @return An iterator pointing to the next element (or <code>end()</code>).
   *
   * If the container owns its elements, then the pointed-to element
   * will be deleted.
   */
  iterator erase( iterator position );


  /**
   * @brief Remove a range of elements.
   * @param first Iterator pointing to the first element to be removed.
   * @param last Iterator pointing one past the last element to be removed.
   * @return An iterator pointing to the element pointed to by @a last
   *         prior to erasing (or <code>end()</code>).
   *
   * If the container owns its elements, then the removed elements
   * will be deleted.  Any duplicates will be removed in this process,
   * but don't rely on this.
   */
  iterator erase( iterator first, iterator last );


  /**
   * @brief Remove the first element from the collection.
   *
   * If the container owns its elements, then the removed element
   * will be deleted.
   */
  void pop_front();


  /**
   * @brief Remove the last element from the collection.
   *
   * If the container owns its elements, then the removed element
   * will be deleted.
   */
  void pop_back();


  /**
   * @brief Erase all the elements in the collection.
   *
   * If the container owns its elements, then the removed elements
   * will be deleted.  Any duplicates will be removed in this process,
   * but don't rely on this.
   */
  void clear();


  //@}
  //========================================================================
  /** @name Swap and sort. */
  //@{


  /**
   * @brief Swap this collection with another.
   * @param rhs The collection with which to swap.
   *
   * Ownership is swapped along with the collection content.
   *
   * Note: this method may only be called using the most-derived
   * @c DataList in the hierarchy.  The @a rhs must also be
   * referenced using the most-derived <code>DataList</code>.
   */
  void swap( DataList& rhs );


  /**
   * @brief Swap the referents of two @c DataVector iterators.
   * @param a The first iterator for the swap.
   * @param b The second iterator for the swap/
   */
  static void iter_swap (iterator a, iterator b);


  /**
   * @brief Sort the container.
   *
   * This just sorts by pointer value, so it's probably not very useful.
   */
  void sort();


  /**
   * @brief Sort the container with a user-specified comparison operator.
   * @param comp Functional to compare two values.
   *
   */
  template <class COMPARE>
  void sort( COMPARE comp );


  //@}
  //========================================================================
  /** @name List operations. */
  //@{


  /**
   *  @brief Insert contents of another list.
   *  @param position Iterator referencing the element to insert before.
   *  @param l  Source list.
   *
   *  The elements of @a l are inserted in constant time in front of
   *  the element referenced by @a position.  @a l becomes an empty
   *  list.
   */
  void splice( iterator position, DataList& l );

  /**
   *  @brief Insert element from another list.
   *  @param position  Iterator referencing the element to insert before.
   *  @param l  Source list.
   *  @param i  Iterator referencing the element to move.
   *
   *  Removes the element in list @a l referenced by @a i and
   *  inserts it into the current list before @a position.
   */
  void splice( iterator position, DataList& l, iterator i );


  /**
   *  @brief  Insert range from another list.
   *  @param  position  Iterator referencing the element to insert before.
   *  @param  l  Source list.
   *  @param  first  Iterator referencing the start of range in @a l.
   *  @param  last  Iterator referencing the end of range in @a l.
   *
   *  Removes elements in the range [@a first, @a last ) and inserts them
   *  before @a position in constant time.
   *
   *  Undefined if @a position is in [@a first, @a last ).
   */
  void splice( iterator position, DataList& l,
               iterator first,
               iterator last );


  /**
   *  @brief  Remove all elements equal to value.
   *  @param  value  The value to remove.
   *
   *  Removes every element in the list equal to @a value.
   *  Remaining elements stay in list order.  Note that this
   *  function only erases the elements, and that if the elements
   *  themselves are pointers, the pointed-to memory is not
   *  touched in any way.  Managing the pointer is the user's
   *  responsibility.
   */
  void remove( const value_type& value );


  /**
   *  @brief  Remove all elements satisfying a predicate.
   *  @param  p  Unary predicate function or object.
   *
   *  Removes every element in the list for which the predicate
   *  returns true.  Remaining elements stay in list order.  Note
   *  that this function only erases the elements, and that if the
   *  elements themselves are pointers, the pointed-to memory is
   *  not touched in any way.  Managing the pointer is the user's
   *  responsibility.
   */
  template<typename PREDICATE>
  void remove_if( PREDICATE p );

  /**
   *  @brief  Remove consecutive duplicate elements.
   *
   *  For each consecutive set of elements with the same value,
   *  remove all but the first one.  Remaining elements stay in
   *  list order.
   */
  void unique();

  /**
   *  @brief  Remove consecutive elements satisfying a predicate.
   *  @param  pred  Binary predicate function or object.
   *
   *  For each consecutive set of elements [first,last) that
   *  satisfy predicate(first,i) where i is an iterator in
   *  [first,last), remove all but the first one.  Remaining
   *  elements stay in list order.
   */
  template<typename BinaryPredicate>
  void unique( BinaryPredicate pred );

  /**
   *  @brief  Merge sorted lists.
   *  @param  l  Sorted list to merge.
   *
   *  Assumes that both @a l and this list are sorted according to
   *  operator<().  Merges elements of @a l into this list in
   *  sorted order, leaving @a l empty when complete.  Elements in
   *  this list precede elements in @a l that are equal.
   */
  void merge( DataList& l );

  /**
   *  @brief  Merge sorted lists according to comparison function.
   *  @param  l  Sorted list to merge.
   *  @param predicate Comparison function defining a sort order (which
   *  must be strictly weak ordering).
   *
   *  Assumes that both @a l and this @c DataList are sorted according to
   *  COMPARE predicate.  Merges elements of @a l into this list
   *  in sorted order, leaving @a l empty when complete.  Elements
   *  in this list precede elements in @a x that are equivalent
   *  according to StrictWeakOrdering().
   */
  template<typename COMPARE>
  void merge( DataList& l, COMPARE predicate );

  /**
   *  @brief  Reverse the elements in list.
   *
   *  Reverse the order of elements in the list in linear time.
   */
  void reverse();



  //@}
  //========================================================================
  /** @name Non-standard operations. */
  //@{


public:
  /**
   * @brief Swap one element out of the container.
   * @param pos The element in the container to swap.
   * @param newElem New element to put in the container.
   *                May be 0.
   * @param oldElem Reference to receive the element removed from the
   *                container.
   *
   * Reference @a oldElem is initialized with element @a pos of the
   * collection (no bounds checking).  Then element @a index is set
   * to @c newElem.  If the collection owns its elements, then it will
   * take ownership of @a newElem and release (without deleting)
   * the element returned through @a oldElem.
   *
   * Note: this method may only be called using the most derived
   * @c DataList in the hierarchy.
   */
  void swapElement(iterator pos, value_type newElem, reference oldElem);


  /**
   * @brief Return the underlying @c std::list of the container.
   * @return Reference to the @c std::list actually holding the collection.
   *
   * Note that @c DataList<T>::stdcont does not necessarily return
   * a @c std::list<T*> if @c DataList inheritance is being used.
   */
  const PtrList& stdcont() const;


  /**
   * @brief Return the ownership policy setting for this container.
   */
  SG::OwnershipPolicy ownPolicy() const;


  /**
   * @brief Erase all the elements in the collection, and reset
   *        the ownership mode.
   * @param ownPolicy The new ownership policy of the container.
   *
   * If the container owns its elements, then the removed elements
   * will be deleted.  Any duplicates will be removed in this process,
   * but don't rely on this.
   */
  void clear (SG::OwnershipPolicy ownPolicy);


  /**
   * @brief Return the DV/DL info struct for this class.
   *
   * This can be used to make sure that it's instantiated.
   */
  static const DataModel_detail::DVLInfoBase& dvlinfo();


  /**
   * @brief Return the DV/DL info struct for this class.
   *
   * This can be used to make sure that it's instantiated.
   */
  virtual const DataModel_detail::DVLInfoBase& dvlinfo_v() const;


  //@}
  //========================================================================
  /** @name Internal operations. */
  //@{


public:
  /**
   * @brief Reset indices / reorder aux data after elements have been permuted.
   * @param beg Start of the range of elements to process.
   * @param end End of the range of elements to process.
   *
   * This is a no-op for @c DataList.
   */
  void resortAux (iterator beg, iterator end);


  /**
   * @brief Test if we can insert; raise an exception if not.
   * @param op Description of the attempted operation.
   *
   * In order to maintain type-safety, we can only allow insertions
   * using the most-derived instance of @c DataList.  This checks
   * this by testing the @c m_isMostDerived, which is set by the constructors
   * to true only for the most-derived instance.
   * If the test fails, we call to potentially out-of-line code to continue.
   */
  void testInsert (const char* op);


  /**
   * @brief Test if we can insert; raise an exception if not.
   * @param op Description of the attempted operation.
   *
   * This continues the test of @c testInsert.  There is one case
   * where @c m_isMostDerived may not be set correctly.  If this container
   * was made via copy construction, then all the @c m_isMostDerived flags
   * will be false.  So we call @c setMostDerived to set the flags correctly
   * and test again.  If the test fails again, then we raise an exception.
   */
  void testInsertOol (const char* op);


private:
  friend class DataModel_detail::ElementProxy<DataList>;
  friend class ConstDataList<DataList>;
  friend void test2_assignelement1<DataList>();


  /**
   * @brief Handle element assignment.
   * @param pos Position in the container to assign.
   * @param newElem The new element to assign.
   *
   * The old element is freed if this container owns elements.
   * Auxiliary data are copied if appropriate.
   */
  void assignElement (typename BaseContainer::iterator pos, value_type newElem);


  /**
   * @brief Handle element assignment from a base pointer.
   * @param pos Position in the container to assign.
   * @param newElem The new element to assign.
   *
   * The old element is freed if this container owns elements.
   * Auxiliary data are copied if appropriate.
   */
  void assignBaseElement (typename BaseContainer::iterator pos,
                          typename BaseContainer::value_type newElem);


protected:
  /**
   * @brief Helper for @c erase().  Remove element at a given position.
   * @param position Iterator pointing to the element to be removed.
   * @return An iterator pointing to the next element (or <code>end()</code>).
   *
   * This function factors out common code between @c erase() in the
   * base and derived @c DataList classes.  It deals with the
   * @c std::list iterators directly.
   */
  typename PtrList::iterator erase_base( typename PtrList::iterator position );


  /**
   * @brief Helper for @c erase().  Remove a range of elements.
   * @param first Iterator pointing to the first element to be removed.
   * @param last Iterator pointing one past the last element to be removed.
   * @return An iterator pointing to the element pointed to by @a last
   *         prior to erasing (or <code> end()</code>).
   *
   * This function factors out common code between @c erase() in the
   * base and derived @c DataList classes.  It deals with the
   * @c std::list iterators directly.
   */
  typename PtrList::iterator
  erase_base( typename PtrList::iterator first, 
	      typename PtrList::iterator last );


protected:
  /// The ownership policy of this container ---
  /// either SG::OWNS_ELEMENTS or SG::VIEW_ELEMENTS.
  SG::OwnershipPolicy m_ownPolicy;

  /// This actually holds the elements.
  PtrList m_pCont;


  /**
   * @brief Clear @c m_isMostDerived for this instance and for all bases.
   *
   * Called from the constructor after setting @c m_isMostDerived.
   */
  void clearMostDerived();


  /**
   * @brief Set @c m_isMostDerived for this instance and clear it for all bases.
   *
   * Called from @c testInsert if the test fails.  The flag may not have
   * been set if this container was made via copy construction, so set
   * it appropriately now so we can test again.
   */
  virtual void setMostDerived();


private:
  /**
   * @brief Find the most-derived @c DataList class in the hierarchy.
   * @return The @c type_info for the class for which this method gets run.
   *
   * In order to properly type-check insertions, we need to be able to find
   * the most-derived @c DataList class in the inheritance hierarchy.
   * That's the purpose of this function.
   * Every @c DataList defines this virtual method, so when it's
   * called, the one corresponding to the most-derived @c DataList
   * gets run.
   */
  virtual const std::type_info& dl_typeid() const;
  //@}


  /// This flag is true if this DL instance is the most-derived one.
  /// We set this to true in the top-level constructor; the constructor
  /// then calls clearMostDerived on the base classes.
  SG::IsMostDerivedFlag m_isMostDerived;


  /// The DV/DL info struct for this class.
  static DataModel_detail::DVLInfo<DataList<T> > s_info;


  typedef typename
    ROOT_SELECTION_NS::DataList<T, DataList_BASE>::self self;
};

template <class T>
const bool DataList<T, DataModel_detail::NoBase>::has_virtual;


/**
 * @brief  List equality comparison.
 * @param  a  A @c DataList.
 * @param  b  A @c DataList of the same type as @a a.
 * @return  True if the size and elements of the lists are equal.
 *
 * This is an equivalence relation.  It is linear in the size of the
 * lists.  Lists are considered equivalent if their sizes are equal,
 * and if corresponding elements compare equal.
 */
template <class T>
bool operator==( const DataList<T>& a, const DataList<T>& b );


/// Based on operator==
template <class T>
bool operator!=( const DataList<T>& a, const DataList<T>& b );


/**
 * @brief  List ordering relation.
 * @param  a  A @c DataList.
 * @param  b  A @c DataList of the same type as @a a.
 * @return  True if @a a is lexicographically less than @a b.
 *
 * This is a total ordering relation.  It is linear in the size of the
 * lists.  Comparisons are done on the pointer values of the elements.
 *
 * See @c std::lexicographical_compare() for how the determination is made.
 */
template <class T>
bool operator< ( const DataList<T>& a, const DataList<T>& b );


/// Based on operator<
template <class T>
bool operator> ( const DataList<T>& a, const DataList<T>& b );


/// Based on operator<
template <class T>
bool operator<=( const DataList<T>& a, const DataList<T>& b );


/// Based on operator<
template <class T>
bool operator>=( const DataList<T>& a, const DataList<T>& b );


/// See <code>DataList<T, BASE>::swap()</code>.
template <class T>
void swap( DataList<T>& a, DataList<T>& b );


/**
 * @brief Specialization of @c ClassName for @c DataList.
 *
 * This overrides the default implementation of @c ClassName
 * to hide <code>DataList</code>'s second template parameter.
 */
template <class T>
class ClassName<DataList<T> >
{
public:
  static std::string name();
};


ENTER_ROOT_SELECTION_NS

#if ROOT_VERSION_CODE < ROOT_VERSION( 5, 99, 0 )

template <class T, class BASE>
class DataList
{
public:
  typedef DataList<T, BASE> self;

  ROOT_SELECTION_NS::TEMPLATE_DEFAULTS<
   ROOT_SELECTION_NS::NODEFAULT,
    typename ::DataList<T>::DataList_BASE> dum1;
  ROOT_SELECTION_NS::NO_SELF_AUTOSELECT dum2;
  ROOT_SELECTION_NS::TRANSIENT m_isMostDerived;
};

#else

template< class T, class BASE >
class DataList : KeepFirstTemplateArguments< 1 >, SelectNoInstance {

public:
   /// A helper typedef
   typedef DataList< T, BASE > self;

   /// Declare the transient variable(s):
   ROOT_SELECTION_NS::MemberAttributes< kTransient > m_isMostDerived;

};

#endif // ROOT_VERSION

EXIT_ROOT_SELECTION_NS


namespace SG {


/**
 * @brief Metafunction to find the proper @c DataBucket class for @c T.
 *
 * Specialize this for @c DataVector.
 * See SGTools/StorableConversions.h for an explanation.
 */
template <class T, class U>
struct DataBucketTrait<DataList<T>, U>
{
  typedef SG::DVLDataBucket<U> type;
  static void init() { DataList<T>::dvlinfo(); }
};


} // namespace SG
#include "AthContainers/DataList.icc"


#if 0
//===================================================================
// Code to try to make the DATALIST_BASES definitions automatically
// based on SG_BASES.  Still have to use one macro, DATALIST_BASES_FROM_SG.
// Currently missing the part that would declare the DataList relations
// to SG.  Not sure how useful this will be; don't bother trying to
// finish it now.
#include "boost/mpl/if.hpp"
#include "boost/mpl/and.hpp"
#include "boost/mpl/equal_to.hpp"
#define if_  boost::mpl::if_
#define and_ boost::mpl::and_
#define equal_to boost::mpl::equal_to
#define true_  boost::mpl::true_
#define false_ boost::mpl::false_
template <class U> struct not_virtual { typedef true_ type; };
template <class U> struct not_virtual<SG::Virtual<U> > { typedef false_ type; };
template <class U> struct clean_type { typedef U type; };
template <> struct clean_type<SG::NoBase>
{ typedef DataModel_detail::NoBase type; };
template <class U> struct clean_type<SG::Virtual<U> >
{ typedef typename clean_type<U>::type type; };
template <class T>
struct bases_from_sg
{
  typedef typename
          if_<equal_to<typename SG::Bases<T>::Base1, SG::NoBase>,
              DataModel_detail::NoBase,
              if_<and_<equal_to<typename SG::Bases<T>::Base1, SG::NoBase>,
                       not_virtual<typename SG::Bases<T>::Base2> >,
                  typename SG::Bases<T>::Base1,
                  DataList_detail::VirtBases
                    <clean_type<typename SG::Bases<T>::Base1>,
                     clean_type<typename SG::Bases<T>::Base2>,
                     clean_type<typename SG::Bases<T>::Base3> >
                 > >::type type;
};
#undef if_
#undef and_
#undef equal_to
#undef true_
#undef false_

#define DATALIST_BASES_FROM_SG(T) \
template <> struct DataListBase<T>                    \
{ typedef bases_from_sg<T>::type Base; };
//===================================================================
#endif



#endif // ATHCONTAINERS_DATALIST_H
