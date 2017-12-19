// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: ConstDataVector.h 777302 2016-10-08 21:24:41Z ssnyder $
/**
 * @file AthContainers/ConstDataVector.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Sep, 2011
 * @brief @c DataVector adapter that acts like it holds const pointers.
 *
 * A @c DataVector\<T> acts as a container of @c T*.
 * This means, though, that one cannot put a <code>const T*</code>
 * into a @c DataVector\<T>.  However, one sometimes wants
 * to do that.  A typical case is that one retrieves
 * a const @c DataVector from StoreGate, filters the contents,
 * and then stores them in a new @c DataVector.  Recall that
 * a const @c DataVector will return <code>const T*</code>.
 * So the pointers one gets from a const @c DataVector
 * cannot be inserted into another @c DataVector.
 * (The root cause of this is that we don't want to have
 * to deal with distinct <code>DataVector\<T></code> and
 * <code>DataVector\<const T></code> types, and thus
 * @c DataVector [and StoreGate] don't have standard const semantics.)
 *
 * To solve this, we introduce the template class @c ConstDataVector\<DV>.
 * The template argument should be a @c DataVector class or something
 * that derives from one.  (The reason the template argument is the
 * @c DataVector class rather than the element type @c T is to allow
 * for types that derive from @c DataVector.)  @c ConstDataVector\<DV>
 * derives from @c DV, but privately --- so it is a @c DV, but clients
 * cannot use it as a @c DV.  Instead, we provide only methods that
 * retrieve const pointers.  Further, the insertion methods will
 * take const rather than non-const pointers.
 *
 * There are two ways (short of casting) to convert a @c ConstDataVector\<DV>
 * to a <code>const DV</code>.  The @c asDataVector method will directly
 * do this conversion.  Also, if the object is recorded in StoreGate,
 * it will automatically be made const, so a retrieval will get
 * a <code>const DV</code>.  A @c ConstDataVector should not convert
 * to a non-const @c DV.
 *
 * So, for example, filtering might look something like this:
 *
 *@code
 *  const DataVector<T>* v_in = 0;
 *  CHECK( sg->retrieve (v_in) );
 *  ConstDataVector<DataVector<T> >* v_out =
 *    new ConstDataVector<DataVector<T> > (SG::VIEW_ELEMENTS);
 *  CHECK( sg->record (v_out, "key") );
 *  for (const T* t : *v_in) {
 *    if (filter (t))
 *      v_out->push_back (t);
 *  }
 @endcode
 *
 * Note that if you are not recording the result in StoreGate,
 * it may well be preferable to just use a <code>std::vector<const T*></code>
 * rather than @c ConstDataVector.
 */


#ifndef ATHCONTAINERS_CONSTDATAVECTOR_H
#define ATHCONTAINERS_CONSTDATAVECTOR_H

#include "AthContainers/DataVector.h"
#include "AthLinks/ElementLink.h"
#if __cplusplus > 201100
#include <initializer_list>
#endif


/**
 * @brief @c DataVector adapter that acts like it holds const pointers.
 */
template <class DV>
class ConstDataVector
  : private DV
{
public:
  /// Basic types, forwarded from the base.
  typedef typename DV::size_type                size_type;
  typedef typename DV::difference_type          difference_type;
  typedef typename DV::allocator_type           allocator_type;
  typedef typename DV::base_value_type          base_value_type;
  typedef typename DV::BaseContainer            BaseContainer;
  typedef typename DV::DVL_BASE                 DVL_BASE;
  typedef typename DV::const_iterator           const_iterator;
  typedef typename DV::const_reverse_iterator   const_reverse_iterator;

  // Not necessarily DV; DV may be an intermediate class.
  typedef DataVector<base_value_type>           base_data_vector;

  /// These types get modified so that the base's @c const_value_type
  /// becomes our @c value_type.
  typedef typename DV::const_value_type         value_type;
  typedef typename DV::const_value_type         const_value_type;
  typedef typename DV::const_value_type &       reference;
  typedef typename DV::const_value_type const & const_reference;
  typedef typename DV::const_value_type *       pointer;
  typedef typename DV::const_value_type const * const_pointer;

  /// This needs to be forwarded from the base as well.
  static const bool has_virtual = DV::has_virtual;

  /// This type is used to proxy lvalue accesses to @c DataVector
  /// elements, in order to handle ownership.
  typedef DataModel_detail::ElementProxy<ConstDataVector> ElementProxy;

  /// The iterator for this type.
  typedef typename DataModel_detail::iterator<ConstDataVector> iterator;

  /// Standard @c reverse_iterator.  Note that lvalue references here will
  /// yield an @c ElementProxy, not a @c reference.
  typedef typename std::reverse_iterator<iterator>
    reverse_iterator;

  typedef boost::true_type isSequence;

  /// Expose methods from the base that don't allow getting back
  /// non-const pointers.
  using DV::size;
  using DV::max_size;
  using DV::resize;
  using DV::capacity;
  using DV::empty;
  using DV::reserve;
  using DV::pop_back;
  using DV::sort;
  using DV::ownPolicy;
  using DV::trackIndices;
  using DV::testInsert;
  using DV::getConstStore;
  using DV::setStore;
  using DV::setConstStore;
  using DV::setNonConstStore;
  using DV::cbegin;
  using DV::cend;
  using DV::crbegin;
  using DV::crend;
#if __cplusplus > 201100
  using DV::shrink_to_fit;
#endif


  /// Expose the const versions of these methods too.
  /// We'll need to be sure to override the non-const versions of all of these
  /// below.
  using DV::begin;
  using DV::end;
  using DV::rbegin;
  using DV::rend;
  using DV::front;
  using DV::back;
  using DV::operator[];
  using DV::at;


  //========================================================================
  /** @name Constructors, destructors, assignment. */
  //@{


  /**
   * @brief Default constructor.
   * @param ownPolicy The ownership mode for the container.
   *
   * By default, a @c DataVector will own its elements.
   * To avoid this, pass @c SG::VIEW_ELEMENTS for @a ownPolicy.
   */
  explicit ConstDataVector(SG::OwnershipPolicy ownPolicy = SG::OWN_ELEMENTS);


  /**
   * @brief Constructor with argument forwarding.
   * @param ownPolicy The ownership mode for the container.
   *
   * All arguments are forwarded to the base class constructor.
   */
  template <typename... ARGS>
  explicit ConstDataVector(SG::OwnershipPolicy ownPolicy, ARGS&&... args);


  /**
   * @brief Sized constructor.
   * @param n The size of the container.
   * @param ownPolicy The ownership mode for the container.
   *
   * Note that unlike the standard vector constructor, you can't specify
   * an initial value here.  The container will be initialized with 0's.
   *
   * By default, a @c DataVector will own its elements.
   * To avoid this, pass @c SG::VIEW_ELEMENTS for @a ownPolicy.
   */
  explicit ConstDataVector(size_type n,
                           SG::OwnershipPolicy ownPolicy = SG::OWN_ELEMENTS);


  /**
   * @brief Constructor from iterators.
   * @param first The start of the range to put in the new container.
   * @param last The end of the range to put in the new container.
   * @param ownPolicy The ownership mode for the container.
   *
   * By default, a view container is made, which does not own its elements.
   * To have the container take ownership of the pointers passed
   * to this constructor, pass @c SG::OWN_ELEMENTS for @a ownPolicy.
   */
  template <class InputIterator>
  ConstDataVector(InputIterator first, InputIterator last,
                  SG::OwnershipPolicy ownPolicy = SG::VIEW_ELEMENTS);


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
#if __cplusplus > 201100
  // Need this to get the default copy ctor defined when a move
  // ctor is also present.
  ConstDataVector (const ConstDataVector&) = default;
#endif


#if __cplusplus > 201100
  /**
   * @brief Move constructor.
   * @param rhs The container from which to move.
   *
   * Any auxiliary data will be moved along with the container contents.
   */
  ConstDataVector (ConstDataVector&& rhs);


  /**
   * @brief Constructor from an initializer list.
   * @param l An initializer list.
   * @param ownPolicy The ownership mode for the container.
   *
   * By default, a view container is made, which does not own its elements.
   * To have the container take ownership of the pointers passed
   * to this constructor, pass @c SG::OWN_ELEMENTS for @a ownPolicy.
   */
  ConstDataVector(std::initializer_list<value_type> l,
                  SG::OwnershipPolicy ownPolicy = SG::VIEW_ELEMENTS);
#endif


  /**
   * @brief Constructor from a vector of ElementLinks.
   * @param v The vector from which to initialize.
   *
   * This will make a view container.
   */
  template <class CONTAINER>
  ConstDataVector (const std::vector<ElementLink<CONTAINER> >& v);


 /**
   * @brief Assignment operator.
   * @param rhs The DataVector from which to assign.
   * @return This object.
   *
   * This is a `shallow' copy; after the completion of this, the DataVector
   * will not own its elements.  Any elements it owned prior to this call
   * will be released.
   *
   * Note: this method may only be called using the most derived
   * @c DataVector in the hierarchy.
   */
  ConstDataVector& operator= (const ConstDataVector& rhs) ;


#if __cplusplus > 201100
  /**
   * @brief Move assignment.
   * @param rhs The container from which to move.
   *
   * Any auxiliary data will be moved along with the container contents.
   */
  ConstDataVector& operator= (ConstDataVector&& rhs);


  /**
   * @brief Assignment operator, from an initializer list.
   * @param l An initializer list.
   * @return This object.
   *
   * This is equivalent to @c assign.
   * Any existing owned elements will be released.
   * The @c DataVector's ownership policy determines whether it will take
   * ownership of the new elements.
   */
  ConstDataVector& operator= (std::initializer_list<value_type> l);
#endif


  /**
   * @brief Assign from iterators.
   * @param first The start of the range to put in the container.
   * @param last The end of the range to put in the container.
   *
   * Any existing owned elements will be released.
   * The @c DataVector's ownership policy determines whether it will take
   * ownership of the new elements.
   */
  template <class InputIterator>
  void assign(InputIterator first, InputIterator last);


#if __cplusplus > 201100
  /**
   * @brief Assign from an initializer list.
   * @param l An initializer list.
   *
   * Any existing owned elements will be released.
   * The @c DataVector's ownership policy determines whether it will take
   * ownership of the new elements.
   */
  void assign(std::initializer_list<value_type> l);
#endif


  /**
   * @brief Assign from a vector of ElementLinks.
   * @param v The vector from which to initialize.
   *
   * This will change the container to a view container.
   */
  template <class CONTAINER>
  void assign (const std::vector<ElementLink<CONTAINER> >& v);


  //@}
  //========================================================================
  /** @name Element access. */
  //@{


  /**
   * @brief Access an element, as an lvalue.
   * @param n Array index to access.
   * @return Proxy to the element at @a n.
   *
   * No bounds checking is done.
   * Note that we return a proxy object rather than a reference;
   * the proxy will handle deleting an owned element if it's assigned to.
   */
  ElementProxy operator[] (size_type n);


  /**
   * @brief Access an element, as an lvalue.
   * @param n Array index to access.
   * @return Proxy to the element at @a n.
   *
   * Will raise @c std::out_of_range if the index is out-of-bounds.
   * Note that we return a proxy object rather than a reference;
   * the proxy will handle deleting an owned element if it's assigned to.
   */
  ElementProxy at (size_type n);


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
   * @brief Add an element to the end of the collection.
   * @param pElem The element to add to the collection.
   *
   * The container's ownership policy will determine if it takes ownership
   * of the new element.
   *
   * Note: this method may only be called using the most derived
   * @c DataVector in the hierarchy.
   */
  void push_back(value_type pElem);


  /**
   * @brief Add an element to the end of the collection.
   * @param pElem The element to add to the collection.
   *
   * The container's ownership policy will determine if it takes ownership
   * of the new element.
   *
   * Note: this method may only be called using the most derived
   * @c DataVector in the hierarchy.
   *
   * For @c DataVector, this is just the same as @c push_back.
   * It's included just for interface compatibility with `std::vector`.
   */
  void emplace_back(value_type pElem);


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
   * @c DataVector in the hierarchy.
   */
  iterator insert(iterator position, value_type pElem);


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
   * @c DataVector in the hierarchy.
   *
   * For @c DataVector, this is just the same as @c insert.
   * It's included just for interface compatibility with `std::vector`.
   */
  iterator emplace(iterator position, value_type pElem);


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
   * @c DataVector in the hierarchy.
   */
  template <class InputIterator>
  void insert(iterator position, InputIterator first, InputIterator last);


#if __cplusplus > 201100
#ifndef __REFLEX__
  /**
   * @brief Add an element to the end of the collection.
   * @param pElem The element to add to the collection.
   *
   * The container must be an owning container.
   *
   * Note: this method may only be called using the most derived
   * @c DataVector in the hierarchy.
   */
  void push_back(std::unique_ptr<const base_value_type> pElem);


  /**
   * @brief Add a new element to the collection.
   * @param position Iterator before which the element will be added.
   * @param pElem The element to add to the collection.
   * @return An iterator that points to the inserted data.
   *
   * The container must be an owning container.
   *
   * Note: this method may only be called using the most derived
   * @c DataVector in the hierarchy.
   */
  iterator insert(iterator position, std::unique_ptr<const base_value_type> pElem);
#endif


  /**
   * @brief Add a group of new elements to the collection.
   * @param position Iterator before which the element will be added.
   * @param l An initializer list.
   *
   * The container's ownership policy will determine if it takes ownership
   * of the new element.
   *
   * Note: this method may only be called using the most derived
   * @c DataVector in the hierarchy.
   */
  void insert(iterator position, std::initializer_list<value_type> l);
#endif


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
   * @c DataVector in the hierarchy.  The @a rhs must also be
   * referenced using the most-derived @c DataVector.
   */
  void swap(ConstDataVector& rhs);


  /**
   * @brief Swap the referents of two @c DataVector iterators.
   * @param a The first iterator for the swap.
   * @param b The second iterator for the swap/
   */
  static void iter_swap (iterator a, iterator b);


  //@}
  //========================================================================
  /** @name Non-standard operations. */
  //@{



  /**
   * @brief Swap one element out of the container.
   * @param index Index of the element in the container to swap.
   * @param newElem New element to put in the container.
   *                May be 0.
   * @param oldElem Reference to receive the element removed from the
   *                container.
   *
   * Reference @a oldElem is initialized with element @a index of the
   * collection (no bounds checking).  Then element @a index is set
   * to @c newElem.  If the collection owns its elements, then it will
   * take ownership of @a newElem and release (without deleting)
   * the element returned through @a oldElem.
   *
   * Note: this method may only be called using the most derived
   * @c DataVector in the hierarchy.
   */
  void swapElement(size_type index, value_type newElem, reference oldElem);


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


#if __cplusplus > 201100
#ifndef __REFLEX__
  /**
   * @brief Swap one element out of the container.
   * @param index Index of the element in the container to swap.
   * @param newElem New element to put in the container.
   *                May be 0.
   * @param oldElem Reference to receive the element removed from the
   *                container.
   *
   * Reference @a oldElem is initialized with element @a index of the
   * collection (no bounds checking).  Then element @a index is set
   * to @c newElem.
   *
   * The collection must own its elements to use its interface.
   * The collection will take ownership of @c newElem and will return
   * ownership of @c oldElem.
   *
   * Note: this method may only be called using the most derived
   * @c DataVector in the hierarchy.
   */
  void swapElement(size_type index,
                   std::unique_ptr<const base_value_type> newElem,
                   std::unique_ptr<const base_value_type>& oldElem);


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
   * to @c newElem.
   *
   * The collection must own its elements to use its interface.
   * The collection will take ownership of @c newElem and will return
   * ownership of @c oldElem.
   *
   * Note: this method may only be called using the most derived
   * @c DataList in the hierarchy.
   */
  void swapElement(iterator pos,
                   std::unique_ptr<const base_value_type> newElem,
                   std::unique_ptr<const base_value_type>& oldElem);
#endif
#endif


public:
  /**
   * @brief Return a pointer to this object, as a const @c DataVector.
   */
  const DV* asDataVector() const;


  /**
   * @brief Cast from a @c DataVector to a @c ConstDataVector.
   * @param dv Pointer to object to cast.
   *
   * Return @c DV cast to a @c ConstDataVector.
   */
  static
  const ConstDataVector* fromDataVector (const DV* dv);


  /**
   * @brief Reset indices / reorder aux data after elements have been permuted.
   * @param beg Start of the range of elements to process.
   * @param end End of the range of elements to process.
   *
   * This is a no-op for @c ConstDataVector.
   */
  void resortAux (iterator /*beg*/, iterator /*end*/);


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
   * @fn void clear
   * @brief Erase all the elements in the collection, and reset
   *        the ownership mode.
   * @param ownPolicy The new ownership policy of the container.
   * @param trackIndices The index tracking policy.
   *
   * If the container owns its elements, then the removed elements
   * will be deleted.  Any duplicates will be removed in this process,
   * but don't rely on this.
   */
  void clear (SG::OwnershipPolicy ownPolicy,
              SG::IndexTrackingPolicy trackIndices);


  /**
   * @brief Convert to @c AuxVectorBase.
   *
   * Needed to get @x AuxVectorBase from a @c ConstDataVector.
   * Present in @c DataVector as well for consistency.
   */
  const SG::AuxVectorBase& auxbase() const;


  //@}
  //========================================================================
  /** @name Relational operators. */
  //@{


  /**
   * @brief  Vector ordering relation.
   * @param  a  A @c ConstDataVector.
   * @param  b  A @c ConstDataVector of the same type as @a x.
   * @return  True iff @a x is lexicographically less than @a y.
   *
   * This is a total ordering relation.  It is linear in the size of the
   * vectors.  Comparisons are done on the pointer values of the elements.
   *
   * See @c std::lexicographical_compare() for how the determination is made.
   */
  bool operator<  (const ConstDataVector& b) const;

  /// Based on operator<
  bool operator>  (const ConstDataVector& b) const;

  /// Based on operator<
  bool operator<= (const ConstDataVector& b) const;

  /// Based on operator<
  bool operator>= (const ConstDataVector& b) const;


  /**
   * @brief  Vector equality comparison.
   * @param  a  A @c ConstDataVector.
   * @param  b  A @c ConstDataVector of the same type as @a x.
   * @return  True iff the size and elements of the vectors are equal.
   *
   * This is an equivalence relation.  It is linear in the size of the
   * vectors.  Vectors are considered equivalent if their sizes are equal,
   * and if corresponding elements compare equal.
   */
  bool operator== (const ConstDataVector& b) const;

  /// Based on operator==
  bool operator!= (const ConstDataVector& b) const;



private:
  //@}
  //========================================================================
  /** @name Private helpers. */
  //@{


  friend class DataModel_detail::ElementProxy<ConstDataVector>;
  friend void test2_assignelement1<ConstDataVector>();


  /**
   * @brief Handle element assignment.
   * @param pos Position in the container to assign.
   * @param newElem The new element to assign.
   *
   * The old element is freed if this container owns elements.
   * Auxiliary data are copied if appropriate.
   */
  void assignElement (typename BaseContainer::iterator pos, value_type newElem);


#if __cplusplus > 201100
#ifndef __REFLEX__
  /**
   * @brief Handle element assignment.
   * @param pos Position in the container to assign.
   * @param newElem The new element to assign.
   *
   * The container must own its elements.
   * Auxiliary data are copied if appropriate.
   */
  void assignElement (typename BaseContainer::iterator pos,
                      std::unique_ptr<const base_value_type> newElem);
#endif
#endif


  /**
   * @brief Handle element assignment from a base pointer.
   * @param pos Position in the container to assign.
   * @param newElem The new element to assign.
   *
   * The old element is freed if this container owns elements.
   * Auxiliary data are copied if appropriate.
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
  typename DV::iterator to_base_iterator (iterator it);


  /**
   * @brief Convert an iterator of the base @c DataVector to
   *        a @c ConstDataVector::iterator.
   * @param it The base @c DataVector iterator to convert.
   */
  iterator to_my_iterator (typename DV::iterator it);


  /**
   * @brief Convert an iterator of the base @c vector to
   *        an @c ElementProxy for the @c ConstDataVector.
   * @param it The base @c vector iterator to convert.
   */
  ElementProxy to_element_proxy (typename BaseContainer::iterator i);


  //@}
};


#ifndef XAOD_STANDALONE

#include "SGTools/ClassID_traits.h"

/**
 * @brief Specialize @c ClassID_traits for @c ConstDataVector so that
 *        they will be automatically made const when recorded in StoreGate.
 */
template <class DV>
struct ClassID_traits<ConstDataVector<DV> >
  : public ClassID_traits<DV>
{
public:
  BOOST_STATIC_CONSTANT(bool, s_isConst = true);
};


namespace SG {


/**
 * @brief @c DataBucket class for @c ConstDataVector.
 *
 * This is just the same as @c DVLDataBucket, except that we've
 * got to convert the @c ConstDataVector to @c DataVector.
 *
 * Note: Can be used to violate const-correctness.
 * Don't use this yourself!
 */
template <class DV>
class DVLConstDataVectorBucket
  : public DVLDataBucket<DV>
{
public:
  /**
   * @brief Constructor from a payload object.
   * @param data Object to hold in the bucket.
   */
  DVLConstDataVectorBucket (ConstDataVector<DV>* data);


#if __cplusplus > 201100
  /**
   * @brief Constructor from a payload object.
   * @param data Object to hold in the bucket.
   */
  DVLConstDataVectorBucket (std::unique_ptr<ConstDataVector<DV> > data);
#endif
};


/**
 * @brief Metafunction to find the proper @c DataBucket class for
 *        the first template argument.
 *
 * Specialize this for @c ConstDataVector.
 * See SGTools/StorableConversions.h for an explanation.
 */
template <class DV, class U>
struct DataBucketTrait<ConstDataVector<DV>, U>
{
  typedef SG::DVLConstDataVectorBucket<DV> type;
  static void init() { DV::dvlinfo(); }
};


/**
 * @brief Let the @c BaseInfo for @c ConstDataVector forward to that
 *        of the base @c DataVector.
 */
template <class DV>
class BaseInfo<ConstDataVector<DV> >
  : public BaseInfo<DV>
{
};


} // namespace SG


#endif // not XAOD_STANDALONE


#include "AthContainers/ConstDataVector.icc"


#endif // not ATHCONTAINERS_CONSTDATAVECTOR_H
