// This file's extension implies that it's C, but it's really -*- C++ -*-.
/*
 * Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration.
 */
/**
 * @file AthContainers/DataVectorWithAlloc.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Aug, 2021
 * @brief DataVector using a custom allocator for the elements.
 *
 * This is a DataVector that uses a custom allocator to for the
 * elements of the vector.  The DataVectorWithAlloc type takes
 * the allocator type as a second template argument.  Something
 * satisfying the STL allocator interface should work, as should
 * the Arena*STLAllocator types from AthAllocators.  If the second
 * argument is defaulted, then AthenaHeapSTLAllocator is used.
 *
 * The main difference in usage from an ordinary DataVector is that
 * you need to allocate elements with the allocate() method, rather
 * than using new.  This will return a @c unique_ptr-like type
 * @c Ptr, which should then be passed to the methods adding
 * an element to the container
 *
 *@code
 *  using Vec = DataVectorWithAlloc<DataVector<T> >;
 *  Vec v ...;
 *  v.push_back (v.allocate());
 *  Vec::Ptr p = v.allocate (123);
 *  v[0] = std::move (p);
 @endcode
 *
 * Similarly to @c ConstDataVector, the @c DataVector methods that take
 * bare pointers are hidden via private inheritance, and the first
 * template argument is the base DataVector type, or something
 * deriving from it.  A @c DataVectorWithAlloc\<DV> may be converted
 * to a <code>const DV</code> using @c asDataVector, or by recording
 * it in StoreGate.  Once recorded, it may be retrieved as a
 * <code>const DV</code>, and it will also act as this for purposes of I/O.
 *
 * It may be interesting to use the protect() methods of the AthAllocators
 * classes to write-protect the contents of the vector.  If that is done,
 * the memory will automatically be unprotected when the vector is destroyed.
 * For this to work, of course, the contents must be really immutable,
 * not just const.
 */


#ifndef ATHCONTAINERS_DATAVECTORWITHALLOC_H
#define ATHCONTAINERS_DATAVECTORWITHALLOC_H


#include "AthAllocators/ArenaHeapSTLAllocator.h"
#include "AthAllocators/maybeUnprotect.h"
#include "AthContainers/DataVector.h"
#include "AthContainers/DataVectorWithAllocFwd.h"


/**
 * @brief @c DataVector using a custom allocator for the elements.
 *
 * The @c ALLOC argument defaulta to @c ArenaHeapSTLAllocator
 * (see DataVectorWithAllocFwd.h).
 */
template <class DV, class ALLOC>
class DataVectorWithAlloc
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
  typedef typename DV::value_type               value_type;
  typedef typename DV::const_value_type         const_value_type;
  typedef typename DV::reference                reference;
  typedef typename DV::const_reference          const_reference;
  typedef typename DV::pointer                  pointer;
  typedef typename DV::const_pointer            const_pointer;

  /// Type of the allocator use for vector elements.
  typedef ALLOC elt_allocator_type;

  // Not necessarily DV; DV may be an intermediate class.
  typedef DataVector<base_value_type>           base_data_vector;

  /// This needs to be forwarded from the base as well.
  // cppcheck-suppress duplInheritedMember
  static const bool has_virtual = DV::has_virtual;

  /// This type is used to proxy lvalue accesses to @c DataVector
  /// elements, in order to handle ownership.
  typedef DataModel_detail::ElementProxy<DataVectorWithAlloc> ElementProxy;

  /// The iterator for this type.
  typedef typename DataModel_detail::iterator<DataVectorWithAlloc> iterator;

  /// Standard @c reverse_iterator.  Note that lvalue references here will
  /// yield an @c ElementProxy, not a @c reference.
  typedef typename std::reverse_iterator<iterator>
    reverse_iterator;

  typedef boost::true_type isSequence;

  /// If true, then this type must own its contents.
  static constexpr bool must_own = true;

  /// Expose methods from the base that don't allow getting back
  /// non-const pointers.
  using DV::size;
  using DV::max_size;
  using DV::resize;
  using DV::capacity;
  using DV::empty;
  using DV::pop_back;
  using DV::sort;
  using DV::ownPolicy;
  using DV::trackIndices;
  using DV::testInsert;
  using DV::getConstStore;
  using DV::setStore;
  using DV::setConstStore;
  using DV::setNonConstStore;
  using DV::hasNonConstStore;
  using DV::cbegin;
  using DV::cend;
  using DV::crbegin;
  using DV::crend;
  using DV::do_cast;


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
  /** @name Smart pointer. */
  //@{

private:
  /**
   * @brief Internal helper to serve as a deleter for our unique_ptr class.
   */
  class UPDeleter
  {
  public:
    UPDeleter() noexcept;
    UPDeleter (elt_allocator_type& heap) noexcept;
    void operator() (value_type p);
    const elt_allocator_type* heap() const noexcept;
  private:
    elt_allocator_type* m_heap;
  };

  
public:
  /**
   * @brief @c unique_ptr holding an element for this vector.
   *
   * Like @c unique_ptr, except that only the container itself
   * can build one from an arbitrary pointer or release one.
   */
  class Ptr
  {
    using base_ptr = std::unique_ptr<base_value_type, UPDeleter>;
  public:
    using pointer = typename base_ptr::pointer;
    using element_type = typename base_ptr::element_type;

    Ptr() = default;
    Ptr(std::nullptr_t) noexcept;
    Ptr(Ptr&&) = default;
    operator bool() const noexcept;
    Ptr& operator= (Ptr&&) noexcept = default;
    Ptr& operator= (std::nullptr_t) noexcept;
    void swap (Ptr& other) noexcept;
    pointer get() const noexcept;
    typename std::add_lvalue_reference<base_value_type>::type operator*() const;
    pointer operator->() const noexcept;
    bool operator== (const Ptr& other) const noexcept;
    bool operator< (const Ptr& other) const noexcept;
    bool operator== (std::nullptr_t) const noexcept;
    bool operator!= (std::nullptr_t) const noexcept;


 private:
    // Methods to build a Ptr from a bare pointer or to release a Ptr
    // to a bare pointer can only be called by the container itself.
    friend class DataVectorWithAlloc;
    Ptr (pointer p, UPDeleter&& d) noexcept;
    pointer release() noexcept;
    // We're trying to insert this pointer into a container using
    // heap other_heap.  Ensure that that's the same heap we're using;
    // raise an exception if not.
    void checkHeap (const elt_allocator_type* other_heap) const;

    /// Underlying unique_ptr.
    base_ptr m_ptr;
  };


  using unique_type = Ptr;


  /**
   * @brief Helper for unique_ptr conversions.  See make_unique below.
   */
  class ContainerUniquePtr
    : public std::unique_ptr<DataVectorWithAlloc>
  {
  public:
    using Base = std::unique_ptr<DataVectorWithAlloc>;
    using Base::Base;
    operator std::unique_ptr<const DV> () && noexcept
    {
      return std::unique_ptr<const DV>(this->release()->asDataVector());
    }
  };


  /**
   * @brief Helper for unique_ptr conversions.
   *
   * If we do
   *@code
   *  auto c = std::make_unique<DataVectorWithAlloc<DV> >();
   @endcode
   *
   * we may later want to convert this to a std::unique_ptr<const DV>.
   * We can do this via @c asDataVector, but actually doing that
   * is a bit awkward.  To help with this, we introduce the type
   * @c ContainerUniquePtr, made with the static method here,
   * that supports an implicit move conversion to std::unique_ptr<const DV>.
   */
  static ContainerUniquePtr make_unique()
  {
    return ContainerUniquePtr (new DataVectorWithAlloc);
  }


  //@}
  //========================================================================
  /** @name Constructors, destructors, assignment. */
  //@{


  /**
   * @brief Default constructor.
   * @param eltAlloc The element allocator.
   */
  explicit DataVectorWithAlloc (elt_allocator_type&& eltAlloc = elt_allocator_type());


  /**
   * @brief Sized constructor.
   * @param n The size of the container.
   * @param eltAlloc The element allocator.
   *
   * Note that unlike the standard vector constructor, you can't specify
   * an initial value here.  The container will be initialized with 0's.
   */
  explicit DataVectorWithAlloc(size_type n,
                               elt_allocator_type&& eltAlloc = elt_allocator_type());


  // Don't allow copying.
  DataVectorWithAlloc (const DataVectorWithAlloc&) = delete;
  DataVectorWithAlloc& operator= (const DataVectorWithAlloc& rhs) = delete;


  /**
   * @brief Move constructor.
   * @param rhs The container from which to move.
   *
   * Any auxiliary data will be moved along with the container contents.
   *
   * The existing allocator is moved.  @c rhs will be left with a new,
   * default-initialized, allocator.
   */
  DataVectorWithAlloc (DataVectorWithAlloc&& rhs);


  /**
   * @brief Move assignment.
   * @param rhs The container from which to move.
   *
   * Any auxiliary data will be moved along with the container contents.
   *
   * The existing allocator is moved.  @c rhs will be left with a new,
   * default-initialized, allocator.
   */
  DataVectorWithAlloc& operator= (DataVectorWithAlloc&& rhs);


  /**
   * @brief Destructor.
   */
  virtual ~DataVectorWithAlloc();


  /**
   * @brief Return the underlying allocator.
   */
  const elt_allocator_type& elt_allocator() const;


  /**
   * @brief Return the underlying allocator.
   */
  elt_allocator_type& elt_allocator();


  //@}
  //========================================================================
  /** @name Size and capacity. */
  //@{


  /**
   * @fn void reserve (size_type n)
   * @brief Attempt to preallocate enough memory for a specified number
   *        of elements.
   * @param n Number of elements required.
   */
  void reserve (size_type n);


  /**
   * @brief Change the vector capacity to match the current size.
   *
   * Note: this does not affect auxiliary data.
   */
  void shrink_to_fit();


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
  iterator begin() noexcept;


  /**
   * @brief Return an @c iterator pointing past the end
   *        of the collection.
   * @return An @c iterator.
   *
   * Note that dereferencing the iterator will yield a proxy rather
   * than a reference; the proxy will handle deleting an owned element
   * if it's assigned to.
   */
  iterator end() noexcept;


  /**
   * @brief Return a @c reverse_iterator pointing past the end
   *        of the collection.
   * @return A @c reverse_iterator.
   *
   * Note that dereferencing the iterator will yield a proxy rather
   * than a reference; the proxy will handle deleting an owned element
   * if it's assigned to.
   */
  reverse_iterator rbegin() noexcept;


  /**
   * @brief Return a @c reverse_iterator pointing at the beginning
   *        of the collection.
   * @return A @c reverse_iterator.
   *
   * Note that dereferencing the iterator will yield a proxy rather
   * than a reference; the proxy will handle deleting an owned element
   * if it's assigned to.
   */
  reverse_iterator rend() noexcept;


  //@}
  //========================================================================
  /** @name Insertion operations. */
  //@{


  /**
   * @brief Add an element to the end of the collection.
   * @param pElem The element to add to the collection.
   *
   * Note: this method may only be called using the most derived
   * @c DataVector in the hierarchy.
   *
   * Returns the pushed pointer.
   */
  value_type push_back (Ptr pElem);


  /**
   * @brief Add an element to the end of the collection.
   * @param pElem The element to add to the collection.
   *
   * Note: this method may only be called using the most derived
   * @c DataVector in the hierarchy.
   *
   * For @c DataVector, this is like the same as @c push_back, and
   * it returns the pushed element.
   * It's included just for interface compatibility with `std::vector`.
   */
  value_type emplace_back (Ptr pElem);


  /**
   * @brief Add a new element to the collection.
   * @param position Iterator before which the element will be added.
   * @param pElem The element to add to the collection.
   * @return An iterator that points to the inserted data.
   *
   * Note: this method may only be called using the most derived
   * @c DataVector in the hierarchy.
   */
  iterator insert(iterator position, Ptr pElem);


  /**
   * @brief Add a new element to the collection.
   * @param position Iterator before which the element will be added.
   * @param pElem The element to add to the collection.
   * @return An iterator that points to the inserted data.
   *
   * Note: this method may only be called using the most derived
   * @c DataVector in the hierarchy.
   *
   * For @c DataVector, this is just the same as @c insert.
   * It's included just for interface compatibility with `std::vector`.
   */
  iterator emplace(iterator position, Ptr pElem);


  //@}
  //========================================================================
  /** @name Erasure operations. */
  //@{


  /**
   * @brief Remove element at a given position.
   * @param position Iterator pointing to the element to be removed.
   * @return An iterator pointing to the next element (or @c end()).
   *
   * The pointed-to element will be deleted.
   */
  iterator erase(iterator position);


  /**
   * @brief Remove a range of elements.
   * @param first Iterator pointing to the first element to be removed.
   * @param last Iterator pointing one past the last element to be removed.
   * @return An iterator pointing to the element pointed to by @a last
   *         prior to erasing (or @c end()).
   *
   * The removed elements will be deleted.
   */
  iterator erase(iterator first, iterator last);


  /**
   * @brief Erase all the elements in the collection.
   *
   * The removed elements will be deleted.
   */
  // cppcheck-suppress duplInheritedMember
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
  void swap (DataVectorWithAlloc& rhs);


  /**
   * @brief Swap the referents of two @c DataVector iterators.
   * @param a The first iterator for the swap.
   * @param b The second iterator for the swap.
   *
   * The iterators must both refer to the same container.
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
   * to @c newElem.  
   *
   * Note: this method may only be called using the most derived
   * @c DataVector in the hierarchy.
   */
  void swapElement (size_type index, Ptr newElem, Ptr& oldElem);


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
   * Note: this method may only be called using the most derived
   * @c DataList in the hierarchy.
   */
  void swapElement (iterator pos, Ptr newElem, Ptr& oldElem);


  /**
   * @brief Return a pointer to this object, as a const @c DataVector.
   */
  const DV* asDataVector() const;


  /**
   * @brief Cast from a @c DataVector to a @c DataVectorWithAlloc.
   * @param dv Pointer to object to cast.
   *
   * Return @c DV cast to a @c DataVectorWithAlloc.
   */
  static
  const DataVectorWithAlloc* fromDataVector (const DV* dv);


  /**
   * @brief Reset indices / reorder aux data after elements have been permuted.
   * @param beg Start of the range of elements to process.
   * @param end End of the range of elements to process.
   */
  void resortAux (iterator beg, iterator end);


  /**
   * @brief Convert to @c AuxVectorBase.
   *
   * Needed to get @x AuxVectorBase from a @c ConstDataVector.
   * Present in @c DataVector as well for consistency.
   */
  const SG::AuxVectorBase& auxbase() const;


  /**
   * @brief Allocate a new vector element.
   *
   * Arguments will be forwarded to the element constructor.
   */
  template <class... Args>
  Ptr allocate (Args&&... args);


  //@}
  //========================================================================
  /** @name Relational operators. */
  //@{


  /**
   * @brief  Vector ordering relation.
   * @param  b  A @c DataVectorWithAlloc of the same type as @a *this.
   * @return  True iff @a a is lexicographically less than @a b.
   *
   * This is a total ordering relation.  It is linear in the size of the
   * vectors.  Comparisons are done on the pointer values of the elements.
   *
   * See @c std::lexicographical_compare() for how the determination is made.
   */
  bool operator<  (const DataVectorWithAlloc& b) const;

  /// Based on operator<
  bool operator>  (const DataVectorWithAlloc& b) const;

  /// Based on operator<
  bool operator<= (const DataVectorWithAlloc& b) const;

  /// Based on operator<
  bool operator>= (const DataVectorWithAlloc& b) const;


  /**
   * @brief  Vector equality comparison.
   * @param  b  A @c DataVectorWithAlloc of the same type as @a *this.
   * @return  True iff the size and elements of the vectors are equal.
   *
   * This is an equivalence relation.  It is linear in the size of the
   * vectors.  Vectors are considered equivalent if their sizes are equal,
   * and if corresponding elements compare equal.
   */
  bool operator== (const DataVectorWithAlloc& b) const;

  /// Based on operator==
  bool operator!= (const DataVectorWithAlloc& b) const;



private:
  //@}
  //========================================================================
  /** @name Private helpers. */
  //@{


  friend class DataModel_detail::ElementProxy<DataVectorWithAlloc>;
  friend void test2_assignelement1a<DataVectorWithAlloc>();


  /**
   * @brief Handle element assignment.
   * @param pos Position in the container to assign.
   * @param newElem The new element to assign.
   *
   * Auxiliary data are copied if appropriate.
   */
  void assignElement (typename BaseContainer::iterator pos, Ptr newElem);


  /**
   * @brief Convert a @c DataVectorWithAlloc::iterator to an iterator
   *        of the base @c DataVector.
   * @param it The @c DataVectorWithAlloc::iterator to convert.
   */
  static
  typename DV::iterator to_base_iterator (iterator it);


  /**
   * @brief Convert an iterator of the base @c DataVector to
   *        a @c DataVectorWithAlloc::iterator.
   * @param it The base @c DataVector iterator to convert.
   */
  iterator to_my_iterator (typename DV::iterator it);


  /**
   * @brief Convert an iterator of the base @c vector to
   *        an @c ElementProxy for the @c DataVectorWithAlloc.
   * @param it The base @c vector iterator to convert.
   */
  ElementProxy to_element_proxy (typename BaseContainer::iterator i);


  /**
   * @brief Internal helper to delete vector elements.
   */
  class HeapDeleter
    : public DV::Deleter
  {
  public:
    using Base = typename DV::Deleter;
    using value_type = typename Base::value_type;
    using PtrVector = typename Base::PtrVector;
    HeapDeleter (elt_allocator_type&& heap);
    elt_allocator_type& heap() noexcept;
    virtual void doDelete (value_type p) override;
    virtual void doDelete (typename PtrVector::iterator first,
                           typename PtrVector::iterator last) override;


  private:
    elt_allocator_type m_heap;
  };


  /// Non-owning pointer to the heap.
  elt_allocator_type* m_heap;
};


/// See @c DataVector<T, BASE>::swap().
template <class DV, class ALLOC>
void swap (DataVectorWithAlloc<DV, ALLOC>& a, DataVectorWithAlloc<DV, ALLOC>& b)
{
  a.swap (b);
}



#ifndef XAOD_STANDALONE

#include "AthenaKernel/ClassID_traits.h"

/**
 * @brief Specialize @c ClassID_traits for @c DataVectorWithAlloc so that
 *        they will be automatically made const when recorded in StoreGate.
 */
template <class DV, class ALLOC>
struct ClassID_traits<DataVectorWithAlloc<DV, ALLOC> >
  : public ClassID_traits<DV>
{
public:
  BOOST_STATIC_CONSTANT(bool, s_isConst = true);
};


namespace SG {


/**
 * @brief @c DataBucket class for @c DataVectorWithAlloc.
 *
 * This is just the same as @c DVLDataBucket, except that we've
 * got to convert the @c DataVectorWithAlloc to @c DataVector.
 *
 * Note: Can be used to violate const-correctness.
 * Don't use this yourself!
 */
template <class DV, class ALLOC>
class DVLDataVectorWithAllocBucket
  : public DVLDataBucket<DV>
{
public:
  /**
   * @brief Constructor from a payload object.
   * @param data Object to hold in the bucket.
   */
  DVLDataVectorWithAllocBucket (DataVectorWithAlloc<DV, ALLOC>* data);


  /**
   * @brief Constructor from a payload object.
   * @param data Object to hold in the bucket.
   */
  DVLDataVectorWithAllocBucket (std::unique_ptr<DataVectorWithAlloc<DV, ALLOC> > data);
};


/**
 * @brief Metafunction to find the proper @c DataBucket class for
 *        the first template argument.
 *
 * Specialize this for @c DataVectorWithAlloc.
 * See AthenaKernel/StorableConversions.h for an explanation.
 */
template <class DV, class ALLOC, class U>
struct DataBucketTrait<DataVectorWithAlloc<DV, ALLOC>, U>
{
  typedef SG::DVLDataVectorWithAllocBucket<DV, ALLOC> type;
  static void init() { DV::dvlinfo(); }
};


/**
 * @brief Let the @c BaseInfo for @c DataVectorWithAlloc forward to that
 *        of the base @c DataVector.
 */
template <class DV, class ALLOC>
class BaseInfo<DataVectorWithAlloc<DV, ALLOC> >
  : public BaseInfo<DV>
{
};


} // namespace SG


#endif // not XAOD_STANDALONE


#include "AthContainers/DataVectorWithAlloc.icc"


#endif // not ATHCONTAINERS_DATAVECTORWITHALLOC_H
