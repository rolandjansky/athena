// This file's extension implies that it's C, but it's really -*- C++ -*-.
/*
 * Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration.
 */
/**
 * @file CxxUtils/ConcurrentPtrSet.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Jul, 2022
 * @brief A set of pointers, alowing concurrent, lockless reads.
 */


#include "CxxUtils/ConcurrentHashmapImpl.h"
#include "boost/iterator/iterator_facade.hpp"
#include "boost/range/iterator_range.hpp"
#include <type_traits>


#ifndef CXXUTILS_CONCURRENTPTRSET_H
#define CXXUTILS_CONCURRENTPTRSET_H


namespace CxxUtils {


/**
 * @brief A set of pointers, allowing concurrent, lockless queries.
 *
 * This class implements a set of pointers, allowing for concurrent access
 * from many threads.
 * Queries can proceed without taking out a lock, while insertions
 * are serialized with each other.  Thus, this is appropriate for sets
 * which are read-mostly.
 *
 * This is based on ConcurrentHashmapImpl.
 *
 * Besides the pointer target type,
 * this class is templated on an UPDATER class, which is used to manage
 * the underlying memory.  The requirements are the same as for the 
 * UPDATER template argument of ConcurrentRangeMap; see there for
 * further details.  (AthenaKernel/RCUUpdater is a concrete version
 * that should work in the context of Athena.)
 *
 * This mostly supports the interface of std::unordered_set, with a few
 * differences / omissions:
 *
 *  - No try_emplace.
 *  - No insert methods with hints.
 *  - No swap().  Could probably be implemented if really needed, but would
 *    require support in ConcurrentHashmapImpl and in the Updater classes.
 *  - No erase() methods.
 *  - No operator==.
 *  - Nothing dealing with the bucket/node interface or merge().
 *
 * Performance:
 *  This is the result from running the test with --perf on my machine,
 *  with gcc 12.1.1:
 *
 *  ConcurrentPtrSet
 *  lookup:   1.225s wall, 1.220s user + 0.000s system = 1.220s CPU (99.6%)
 *  iterate:  0.646s wall, 0.640s user + 0.000s system = 0.640s CPU (99.0%)
 *  UnorderedSet
 *  lookup:   1.894s wall, 1.880s user + 0.000s system = 1.880s CPU (99.3%)
 *  iterate:  1.064s wall, 1.060s user + 0.000s system = 1.060s CPU (99.6%)
 *  concurrent_unordered_set
 *  lookup:   5.369s wall, 5.320s user + 0.000s system = 5.320s CPU (99.1%)
 *  iterate:  4.151s wall, 4.130s user + 0.000s system = 4.130s CPU (99.5%)
 *  ck_ht
 *  lookup:   2.034s wall, 2.020s user + 0.000s system = 2.020s CPU (99.3%)
 *  iterate:  1.500s wall, 1.500s user + 0.000s system = 1.500s CPU (100.0%)
 *
 * The timing for the lookup test of UnorderedSet is probably overly-optimistic,
 * since the test doesn't do any locking in that case.
 */
template <class VALUE, template <class> class UPDATER>
// FIXME: Check UPDATER.
class ConcurrentPtrSet
{
private:
  /// The underlying uint->uint hash table.
  struct Hasher;
  struct Matcher;
  using Impl_t = typename detail::ConcurrentHashmapImpl<UPDATER, Hasher, Matcher>;
  /// Representation type in the underlying map.
  using val_t = CxxUtils::detail::ConcurrentHashmapVal_t;


public:
  /// Standard STL types.
  using key_type = VALUE*;
  using const_key_type = const VALUE*;
  using size_type = size_t;
  /// Updater object.
  using Updater_t = typename Impl_t::Updater_t;
  /// Context type.
  using Context_t = typename Updater_t::Context_t;


  /**
   * @brief Constructor.
   * @param updater Object used to manage memory
   *                (see comments at the start of the class).
   * @param capacity The initial table capacity.
   *                 (Will be rounded up to a power of two.)
   * @param ctx Execution context.
   */
  ConcurrentPtrSet (Updater_t&& updater,
                    size_type capacity = 64,
                    const Context_t& ctx = Updater_t::defaultContext());


  /** 
   * @brief Constructor from another set.
   * @param updater Object used to manage memory
   *                (see comments at the start of the class).
   * @param capacity The initial table capacity of the new table.
   *                 (Will be rounded up to a power of two.)
   * @param ctx Execution context.
   *
   * (Not really a copy constructor since we need to pass @c updater.)
   */
  ConcurrentPtrSet (const ConcurrentPtrSet& other,
                    Updater_t&& updater,
                    size_t capacity = 64,
                    const Context_t& ctx = Updater_t::defaultContext());


  /** 
   * @brief Constructor from a range.
   * @param f Start iterator for the range.
   * @param l End iterator for the range.
   * @param updater Object used to manage memory
   *                (see comments at the start of the class).
   * @param capacity The initial table capacity of the new table.
   *                 (Will be rounded up to a power of two.)
   * @param ctx Execution context.
   */
  template <class InputIterator>
  ConcurrentPtrSet (InputIterator f,
                    InputIterator l,
                    Updater_t&& updater,
                    size_type capacity = 64,
                    const Context_t& ctx = Updater_t::defaultContext());


  /// Copy / move / assign not supported.
  ConcurrentPtrSet (const ConcurrentPtrSet& other) = delete;
  ConcurrentPtrSet (ConcurrentPtrSet&& other) = delete;
  ConcurrentPtrSet& operator= (const ConcurrentPtrSet& other) = delete;
  ConcurrentPtrSet& operator= (ConcurrentPtrSet&& other) = delete;


  /**
   * @brief Destructor.
   */
  ~ConcurrentPtrSet();


  /**
   * @brief Return the number of items currently in the set.
   */
  size_type size() const;


  /**
   * @brief Test if the set is currently empty.
   */
  bool empty() const;


  /**
   * @brief Return the current size (capacity) of the hash table.
   */
  size_t capacity() const;


  /**
   * @brief Value type for iterators.
   */
  using const_iterator_value = const key_type;


  /**
   * @brief Iterator class.
   *
   * This uses boost::iterator_facade to define the methods required
   * by an STL iterator in terms of the private methods below.
   *
   * Since dereference() will be returning a const_iterator_value by value,
   * we also need to override the reference type.
   */
  class const_iterator
    : public boost::iterator_facade<const_iterator,
                                    const const_iterator_value,
                                    std::bidirectional_iterator_tag,
                                    const const_iterator_value>
  {
  public:
   /**
     * @brief Constructor.
     * @param it Iterator of the underlying table.
     */
    const_iterator (typename Impl_t::const_iterator it);


    /**
     * @brief Test if this iterator is valid.
     *
     * This should be the same as testing for != end().
     */
    bool valid() const;


  private:
    /// Required by iterator_facade.
    friend class boost::iterator_core_access;


    /**
     * @brief iterator_facade requirement: Increment the iterator.
     */
    void increment();


    /**
     * @brief iterator_facade requirement: Decrement the iterator.
     */
    void decrement();


    /**
     * @brief iterator_facade requirement: Equality test.
     */
    bool equal (const const_iterator& other) const;


    /**
     * @brief iterator_facade requirement: Dereference the iterator.
     */
    key_type dereference() const;


    /// The iterator on the underlying table.
    typename Impl_t::const_iterator m_impl;
  };


  /// A range defined by two iterators.
  typedef boost::iterator_range<const_iterator> const_iterator_range;


  /**
   * @brief Return an iterator range covering the entire set.
   */
  const_iterator_range range() const;


  /**
   * @brief Iterator at the start of the set.
   */
  const_iterator begin() const;


  /**
   * @brief Iterator at the end of the set.
   */
  const_iterator end() const;


  /**
   * @brief Iterator at the start of the set.
   */
  const_iterator cbegin() const;


  /**
   * @brief Iterator at the end of the set.
   */
  const_iterator cend() const;


  /**
   * @brief Test if a key is in the container.
   * @param key The key to test.
   */
  bool contains (const const_key_type key) const;


  /**
   * @brief Return the number of times a given key is in the container.
   * @param key The key to test.
   *
   * Returns either 0 or 1, depending on whether or not the key is in the set.
   */
  size_type count (const const_key_type key) const;


  /**
   * @brief Look up an element in the set.
   * @param key The element to find.
   *
   * Returns either an iterator referencing the found element or end().
   */
  const_iterator find (const const_key_type key) const;


  /**
   * @brief Return a range of iterators with entries matching @c key.
   * @param key The element to find.
   *
   * As keys are unique in this container, this is either a single-element
   * range, or both iterators are equal to end().
   */
  std::pair<const_iterator, const_iterator>
  equal_range (const const_key_type key) const;


  /**
   * @brief Add an element to the set.
   * @param key The key of the new item to add.
   * @param ctx Execution context.
   *
   * This will not overwrite an existing entry.
   * The first element in the returned pair is an iterator referencing
   * the added item.  The second is a flag that is true if a new element
   * was added.
   */
  std::pair<const_iterator, bool>
  emplace (const key_type key,
           const Context_t& ctx = Updater_t::defaultContext());


  /**
   * @brief Add an element to the set.
   * @param p The item to add.
   *
   * This will not overwrite an existing entry.
   * The first element in the returned pair is an iterator referencing
   * the added item.  The second is a flag that is true if a new element
   * was added.
   */
  std::pair<const_iterator, bool> insert (const key_type p);


  /**
   * @brief Insert a range of elements to the set.
   * @param first Start of the range.
   * @param last End of the range.
   */
  template <class InputIterator>
  void insert (InputIterator first, InputIterator last);


  /**
   * @brief Increase the table capacity.
   * @param capacity The new table capacity.
   * @param ctx Execution context.
   *
   * No action will be taken in @c capacity is smaller
   * than the current capacity.
   */
  void reserve (size_type capacity,
                const Context_t& ctx = Updater_t::defaultContext());


  /**
   * @brief Increase the table capacity.
   * @param capacity The new table capacity.
   *
   * No action will be taken in @c capacity is smaller
   * than the current capacity.
   */
  void rehash (size_type capacity);


  /**
   * @brief Erase the table and change the capacity.
   * @param capacity The new table capacity.
   * @param ctx Execution context.
   */
  void clear (size_t capacity,
              const Context_t& ctx = Updater_t::defaultContext());


  /**
   * @brief Erase the table (don't change the capacity).
   * @param ctx Execution context.
   */
  void clear (const Context_t& ctx = Updater_t::defaultContext());


  /**
   * @brief Called when this thread is no longer referencing anything
   *        from this container.
   * @param ctx Execution context.
   */
  void quiescent (const Context_t& ctx);


private:
  /**
   * @brief Convert an underlying key value to a pointer.
   * @param val The underlying key value.
   */
  static key_type keyAsPtr (val_t val);


  /**
   * @brief Convert a pointer to an underlying key value.
   * @param p The pointer.
   */
  static val_t keyAsVal (const const_key_type p);


  /**
   * @brief Do a lookup in the table.
   * @param key The key to look up.
   *
   * Returns an iterator of the underlying map pointing at the found
   * entry or end();
   */
  typename Impl_t::const_iterator get (const const_key_type key) const;


  /**
   * @brief Insert an entry in the table.
   * @param key The new item to add.
   * @param ctx Execution context.
   *
   * The first element in the returned pair is an iterator referencing
   * the added item.  The second is a flag that is true if a new element
   * was added.
   */
  std::pair<const_iterator, bool>
  put (const key_type key,
       const Context_t& ctx = Updater_t::defaultContext());


  /**
   * @brief Hash functional for keys.
   *
   * The key can be either a pointer or the representation type
   * used by the underlying map.
   */
  struct Hasher
  {
    /// Hash function from the underlying representation type.
    size_t operator() (const val_t p) const;
    /// Hash function from a pointer.
    size_t operator() (const const_key_type p) const;
    /// Hash functional.
    std::hash<const_key_type> m_hash;
  };


  /**
   * @brief Matching functional for keys.
   */
  struct Matcher
  {
    /// Compare two keys (as the underlying representation type) for equality.
    bool operator() (const val_t a, const val_t b) const;
  };


  /// The underlying hash table.
  Impl_t m_impl;
};


} // namespace CxxUtils


#include "CxxUtils/ConcurrentPtrSet.icc"


#endif // not CXXUTILS_CONCURRENTPTRSET_H
