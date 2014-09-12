// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: ConstDataList.h 610620 2014-08-06 21:15:52Z ssnyder $
/**
 * @file AthContainers/ConstDataList.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Sep, 2011
 * @brief @c DataList adapter that acts like it holds const pointers.
 *
 * A @c DataList\<T> acts as a container of @c T*.
 * This means, though, that one cannot put a <code>const T*</code>
 * into a @c DataList\<T>.  However, one sometimes wants
 * to do that.  A typical case is that one retrieves
 * a const @c DataList from StoreGate, filters the contents,
 * and then stores them in a new @c DataList.  Recall that
 * a const @c DataList will return <code>const T*</code>.
 * So the pointers one gets from a const @c DataList
 * cannot be inserted into another @c DataList.
 * (The root cause of this is that we don't want to have
 * to deal with distinct <code>DataList\<T></code> and
 * <code>DataList\<const T></code> types, and thus
 * @c DataList [and StoreGate] don't have standard const semantics.)
 *
 * To solve this, we introduce the template class @c ConstDataList\<DL>.
 * The template argument should be a @c DataList class or something
 * that derives from one.  (The reason the template argument is the
 * @c DataList class rather than the element type @c T is to allow
 * for types that derive from @c DataList.)  @c ConstDataList\<DL>
 * derives from @c DL, but privately --- so it is a @c DL, but clients
 * cannot use it as a @c DL.  Instead, we provide only methods that
 * retrieve const pointers.  Further, the insertion methods will
 * take const rather than non-const pointers.
 *
 * There are two ways (short of casting) to convert a @c ConstDataList\<DL>
 * to a <code>const DL</code>.  The @c asDataList method will directly
 * do this conversion.  Also, if the object is recorded in StoreGate,
 * it will automatically be made const, so a retrieval will get
 * a <code>const DL</code>.  A @c ConstDataList should not convert
 * to a non-const @c DL.
 *
 * So, for example, filtering might look something like this:
 *
 *@code
 *  const DataList<T>* v_in = 0;
 *  CHECK( sg->retrieve (v_in) );
 *  ConstDataList<DataList<T> >* v_out =
 *    new ConstDataList<DataList<T> > (SG::VIEW_ELEMENTS);
 *  CHECK( sg->record (v_out, "key") );
 *  for (const T* t : *v_in) {
 *    if (filter (*it))
 *      v_out->push_back (*it);
 *  }
 @endcode
 *
 * Note that if you are not recording the result in StoreGate,
 * it may well be preferable to just use a <code>std::list<const T*></code>
 * rather than @c ConstDataList.
 */


#ifndef ATHCONTAINERS_CONSTDATALIST_H
#define ATHCONTAINERS_CONSTDATALIST_H

#include "AthContainers/DataList.h"
#include "SGTools/ClassID_traits.h"


/**
 * @brief @c DataList adapter that acts like it holds const pointers.
 */
template <class DL>
class ConstDataList
  : private DL
{
public:
  /// Basic types, forwarded from the base.
  typedef typename DL::size_type                size_type;
  typedef typename DL::difference_type          difference_type;
  typedef typename DL::allocator_type           allocator_type;
  typedef typename DL::base_value_type          base_value_type;
  typedef typename DL::BaseContainer            BaseContainer;
  typedef typename DL::DVL_BASE                 DVL_BASE;
  typedef typename DL::const_iterator           const_iterator;
  typedef typename DL::const_reverse_iterator   const_reverse_iterator;

  /// These types get modified so that the base's @c const_value_type
  /// becomes our @c value_type.
  typedef typename DL::const_value_type         value_type;
  typedef typename DL::const_value_type         const_value_type;
  typedef typename DL::const_value_type &       reference;
  typedef typename DL::const_value_type const & const_reference;
  typedef typename DL::const_value_type *       pointer;
  typedef typename DL::const_value_type const * const_pointer;

  /// This needs to be forwarded from the base as well.
  static const bool has_virtual = DL::has_virtual;

  /// This type is used to proxy lvalue accesses to @c DataList
  /// elements, in order to handle ownership.
  typedef DataModel_detail::ElementProxy<ConstDataList> ElementProxy;

  /// The iterator for this type.
  typedef typename DataModel_detail::iterator<ConstDataList> iterator;

  /// Standard @c reverse_iterator.  Note that lvalue references here will
  /// yield an @c ElementProxy, not a @c reference.
  typedef typename std::reverse_iterator<iterator>
    reverse_iterator;

  /// Expose methods from the base that don't allow getting back
  /// non-const pointers.
  using DL::size;
  using DL::max_size;
  using DL::resize;
  using DL::empty;
  using DL::pop_front;
  using DL::pop_back;
  using DL::clear;
  using DL::sort;
  using DL::ownPolicy;
  using DL::reverse;
  using DL::remove_if;
  using DL::unique;
  using DL::testInsert;


  /// Expose the const versions of these methods too.
  /// We'll need to be sure to override the non-const versions of all of these
  /// below.
  using DL::begin;
  using DL::end;
  using DL::rbegin;
  using DL::rend;
  using DL::front;
  using DL::back;


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
  explicit ConstDataList(SG::OwnershipPolicy ownPolicy = SG::OWN_ELEMENTS);


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
  explicit ConstDataList(size_type n,
                         SG::OwnershipPolicy ownPolicy = SG::OWN_ELEMENTS);


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
  ConstDataList(InputIterator first, InputIterator last,
                SG::OwnershipPolicy ownPolicy = SG::VIEW_ELEMENTS);


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
  ConstDataList& operator= (const ConstDataList& rhs);


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
  void assign(InputIterator first, InputIterator last);


  //@}
  //========================================================================
  /** @name Element access. */
  //@{


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
  void push_front(value_type pElem);


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
  void push_back(value_type pElem);


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
  iterator insert(iterator position, value_type pElem);


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
  void insert(iterator position, InputIterator first, InputIterator last);


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
  void swap(ConstDataList& rhs);


  /**
   * @brief Swap the referents of two @c DataVector iterators.
   * @param a The first iterator for the swap.
   * @param b The second iterator for the swap/
   */
  static void iter_swap (iterator a, iterator b);


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
  void splice( iterator position, ConstDataList& l );


  /**
   *  @brief Insert element from another list.
   *  @param position  Iterator referencing the element to insert before.
   *  @param l  Source list.
   *  @param i  Iterator referencing the element to move.
   *
   *  Removes the element in list @a l referenced by @a i and
   *  inserts it into the current list before @a position.
   */
  void splice( iterator position, ConstDataList& l, iterator i );


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
  void splice( iterator position, ConstDataList& l,
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
   *  @brief  Merge sorted lists.
   *  @param  l  Sorted list to merge.
   *
   *  Assumes that both @a l and this list are sorted according to
   *  operator<().  Merges elements of @a l into this list in
   *  sorted order, leaving @a l empty when complete.  Elements in
   *  this list precede elements in @a l that are equal.
   */
  void merge( ConstDataList& l );


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
  void merge( ConstDataList& l, COMPARE predicate );


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
   * @brief Return a pointer to this object, as a const @c DataList.
   */
  const DL* asDataList() const;


  /**
   * @brief Reset indices / reorder aux data after elements have been permuted.
   * @param beg Start of the range of elements to process.
   * @param end End of the range of elements to process.
   *
   * This is a no-op for @c ConstDataList.
   */
  void resortAux (iterator beg, iterator end);


  //@}
  //========================================================================
  /** @name Relational operators. */
  //@{


  /**
   * @brief  List ordering relation.
   * @param  a  A @c ConstDataList.
   * @param  b  A @c ConstDataList of the same type as @a x.
   * @return  True iff @a x is lexicographically less than @a y.
   *
   * This is a total ordering relation.  It is linear in the size of the
   * lists.  Comparisons are done on the pointer values of the elements.
   *
   * See @c std::lexicographical_compare() for how the determination is made.
   */
  bool operator< (const ConstDataList& b) const;

  /// Based on operator<
  bool operator> (const ConstDataList& b) const;

  /// Based on operator<
  bool operator<= (const ConstDataList& b) const;

  /// Based on operator<
  bool operator>= (const ConstDataList& b) const;

  /**
   * @brief  List equality comparison.
   * @param  a  A @c ConstDataList.
   * @param  b  A @c ConstDataList of the same type as @a x.
   * @return  True iff the size and elements of the lists are equal.
   *
   * This is an equivalence relation.  It is linear in the size of the
   * lists.  Lists are considered equivalent if their sizes are equal,
   * and if corresponding elements compare equal.
   */
  bool operator== (const ConstDataList& b) const;

  /// Based on operator==
  bool operator!= (const ConstDataList& b) const;


private:
  //@}
  //========================================================================
  /** @name Private helpers. */
  //@{


  friend class DataModel_detail::ElementProxy<ConstDataList>;
  friend void test2_assignelement1<ConstDataList>();


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
  void
  assignBaseElement (typename BaseContainer::iterator pos,
                     typename BaseContainer::value_type newElem);


  /**
   * @brief Convert a @c ConstDataVector::iterator to an iterator
   *        of the base @c DataVector.
   * @param it The @c ConstDataVector::iterator to convert.
   */
  static
  typename DL::iterator to_base_iterator (iterator it);


  /**
   * @brief Convert an iterator of the base @c DataList to
   *        a @c ConstDataList::iterator.
   * @param it The base @c DataList iterator to convert.
   */
  iterator to_my_iterator (typename DL::iterator it);


  /**
   * @brief Convert an iterator of the base @c vector
   *        an @c ElementProxy for the @c ConstDataVector.
   * @param it The base @c vector iterator to convert.
   */
  ElementProxy to_element_proxy (typename BaseContainer::iterator i);


  //@}
};


/// See <code>DataList<T, BASE>::swap()</code>.
template <class T>
void swap( ConstDataList<T>& a, ConstDataList<T>& b );


/**
 * @brief Specialize @c ClassID_traits for @c ConstDataList so that
 *        they will be automatically made const when recorded in StoreGate.
 */
template <class DL>
struct ClassID_traits<ConstDataList<DL> >
  : public ClassID_traits<DL>
{
public:
  BOOST_STATIC_CONSTANT(bool, s_isConst = true);
};


namespace SG {


/**
 * @brief @c DataBucket class for @c ConstDataList.
 *
 * This is just the same as @c DVLDataBucket, except that we've
 * got to convert the @c ConstDataList to @c DataList.
 *
 * Note: Can be used to violate const-correctness.
 * Don't use this yourself!
 */
template <class T>
class DVLConstDataListBucket
  : public DVLDataBucket<T>
{
public:
  /**
   * @brief Constructor from a payload object.
   * @param data Object to hold in the bucket.
   */
  DVLConstDataListBucket (ConstDataList<T>* data);


#if __cplusplus > 201100
  /**
   * @brief Constructor from a payload object.
   * @param data Object to hold in the bucket.
   */
  DVLConstDataListBucket (std::unique_ptr<ConstDataList<T> > data);
#endif
};


/**
 * @brief Metafunction to find the proper @c DataBucket class for
 *        the first template argument.
 *
 * Specialize this for @c ConstDataVector.
 * See SGTools/StorableConversions.h for an explanation.
 */
template <class T, class U>
struct DataBucketTrait<ConstDataList<T>, U>
{
  typedef SG::DVLConstDataListBucket<T> type;
  static void init() { DataList<T>::dvlinfo(); }
};


/**
 * @brief Let the @c BaseInfo for @c ConstDataList forward to that
 *        of the base @c DataList.
 */
template <class T>
class BaseInfo<ConstDataList<T> >
  : public BaseInfo<T>
{
};


} // namespace SG


#include "AthContainers/ConstDataList.icc"


#endif // not ATHCONTAINERS_CONSTDATALIST_H
