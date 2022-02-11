// This file's extension implies that it's C, but it's really -*- C++ -*-.
/*
 * Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration.
 */
/**
 * @file CxxUtils/ConcurrentStrMap.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Dec, 2020
 * @brief Hash map from strings allowing concurrent, lockless reads.
 */


#ifndef CXXUTILS_CONCURRENTSTRMAP_H
#define CXXUTILS_CONCURRENTSTRMAP_H


#include "CxxUtils/ConcurrentHashmapImpl.h"
#include "CxxUtils/concepts.h"
#include "boost/iterator/iterator_facade.hpp"
#include "boost/range/iterator_range.hpp"
#include <type_traits>
#include <stdexcept>


namespace CxxUtils {


/**
 * @brief Hash map from strings allowing concurrent, lockless reads.
 *
 * This class implements a hash map from strings.
 * The mapped type may be a pointer or any numeric type that can
 * fit in a uintptr_t.
 * It allows for concurrent access from many threads.
 * Reads can proceed without taking out a lock, while writes are serialized
 * with each other.  Thus, this is appropriate for maps which are read-mostly.
 *
 * This is based on ConcurrentHashmapImpl.
 *
 * Besides the mapped value type,
 * this class is templated on an UPDATER class, which is used to manage
 * the underlying memory.  The requirements are the same as for the 
 * UPDATER template argument of ConcurrentRangeMap; see there for
 * further details.  (AthenaKernel/RCUUpdater is a concrete version
 * that should work in the context of Athena.)
 *
 * This mostly supports the interface of std::unordered_map, with a few
 * differences / omissions:
 *
 *  - Dereferencing the iterator returns a structure by value, not a reference.
 *  - No try_emplace.
 *  - No insert methods with hints.
 *  - No swap().  Could probably be implemented if really needed, but would
 *    require support in ConcurrentHashmapImpl and in the Updater classes.
 *  - No clear().  Could be implemented if really needed; we would need
 *    to be able to attach the key strings to be deleted to the old
 *    table instance.
 *  - No erase() methods.  In addition to what's needed for clear(),
 *    the underlying hash table would need to support tombstones.
 *  - No operator==.
 *  - Nothing dealing with the bucket/node interface or merge().
 *
 * Possible improvements:
 *
 *  - We could speed up lookups further by storing the hash along with
 *    the string.  That way, we should almost always not have to do
 *    the full string comparison more than once.
 *
 *  - We could use a more efficient allocator for the string keys
 *    that we need to save.  That could in particular help for updates,
 *    where currently we always need to allocate a string, and then
 *    need to delete it if it turns out we're doing an update rather
 *    than in insertion.
 */
template <class VALUE, template <class> class UPDATER>
// FIXME: Check UPDATER too.
ATH_REQUIRES (std::is_standard_layout_v<VALUE> &&
              std::is_trivial_v<VALUE> &&
              (sizeof (VALUE) <= sizeof (uintptr_t)))
class ConcurrentStrMap
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
  using key_type = std::string;
  using mapped_type = VALUE;
  using size_type = size_t;
  /// Updater object.
  using Updater_t = typename Impl_t::Updater_t;
  /// Context type.
  using Context_t = typename Updater_t::Context_t;

  /// Ensure that the underlying map can store our mapped_type.
  static_assert( sizeof(typename Impl_t::val_t) >= sizeof(mapped_type) );


  /**
   * @brief Constructor.
   * @param updater Object used to manage memory
   *                (see comments at the start of the class).
   * @param capacity The initial table capacity.
   *                 (Will be rounded up to a power of two.)
   * @param ctx Execution context.
   */
  ConcurrentStrMap (Updater_t&& updater,
                    size_type capacity = 64,
                    const Context_t& ctx = Updater_t::defaultContext());

  /** 
   * @brief Constructor from another map.
   * @param updater Object used to manage memory
   *                (see comments at the start of the class).
   * @param capacity The initial table capacity of the new table.
   *                 (Will be rounded up to a power of two.)
   * @param ctx Execution context.
   *
   * (Not really a copy constructor since we need to pass @c updater.)
   */
  ConcurrentStrMap (const ConcurrentStrMap& other,
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
   *
   * Constructor from a range of pairs.
   */
  template <class InputIterator>
  ConcurrentStrMap (InputIterator f,
                    InputIterator l,
                    Updater_t&& updater,
                    size_type capacity = 64,
                    const Context_t& ctx = Updater_t::defaultContext());


  /// Copy / move / assign not supported.
  ConcurrentStrMap (const ConcurrentStrMap& other) = delete;
  ConcurrentStrMap (ConcurrentStrMap&& other) = delete;
  ConcurrentStrMap& operator= (const ConcurrentStrMap& other) = delete;
  ConcurrentStrMap& operator= (ConcurrentStrMap&& other) = delete;


  /**
   * @brief Destructor.
   */
  ~ConcurrentStrMap();


  /**
   * @brief Return the number of items currently in the map.
   */
  size_type size() const;


  /**
   * @brief Test if the map is currently empty.
   */
  bool empty() const;


  /**
   * @brief Return the current size (capacity) of the hash table.
   */
  size_t capacity() const;


  /**
   * @brief Value structure for iterators.
   *
   * For a map from K to V, a STL-style iterator should dereference
   * to a std::pair<const K, V>.  However, we don't actually store an object
   * like that anywhere.  In order to be able to have STL-like iterators,
   * we instead use a pair where the first element is a const reference
   * to the std::string key.  We will
   * return this _by value_ when we deference an iterator.
   * (The compiler should be able to optimize out the redundant
   * packing and unpacking of fields.)
   */
  using const_iterator_value = std::pair<const key_type&, mapped_type>;


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
     * @brief Test if this iterator is value.
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
    const const_iterator_value dereference() const;


    /// The iterator on the underlying table.
    typename Impl_t::const_iterator m_impl;
  };


  /// A range defined by two iterators.
  typedef boost::iterator_range<const_iterator> const_iterator_range;


  /**
   * @brief Return an iterator range covering the entire map.
   */
  const_iterator_range range() const;


  /**
   * @brief Iterator at the start of the map.
   */
  const_iterator begin() const;


  /**
   * @brief Iterator at the end of the map.
   */
  const_iterator end() const;


  /**
   * @brief Iterator at the start of the map.
   */
  const_iterator cbegin() const;


  /**
   * @brief Iterator at the end of the map.
   */
  const_iterator cend() const;


  /**
   * @brief Test if a key is in the container.
   * @param key The key to test.
   */
  bool contains (const key_type& key) const;


  /**
   * @brief Return the number of times a given key is in the container.
   * @param key The key to test.
   *
   * Returns either 0 or 1, depending on whether or not the key is in the map.
   */
  size_type count (const key_type& key) const;


  /**
   * @brief Look up an element in the map.
   * @param key The element to find.
   *
   * Returns either an iterator referencing the found element or end().
   */
  const_iterator find (const key_type& key) const;


  /**
   * @brief Look up an element in the map.
   * @param key The element to find.
   *
   * Returns the value associated with the key.
   * Throws @c std::out_of_range if the key does not exist in the map.
   */
  mapped_type at (const std::string& key) const;


  /**
   * @brief Return a range of iterators with entries matching @c key.
   * @param key The element to find.
   *
   * As keys are unique in this container, this is either a single-element
   * range, or both iterators are equal to end().
   */
  std::pair<const_iterator, const_iterator>
  equal_range (const key_type& key) const;


  /**
   * @brief Add an element to the map.
   * @param key The key of the new item to add.
   * @param val The value of the new item to add.
   * @param ctx Execution context.
   *
   * This will not overwrite an existing entry.
   * The first element in the returned pair is an iterator referencing
   * the added item.  The second is a flag that is true if a new element
   * was added.
   */
  std::pair<const_iterator, bool>
  emplace (const key_type& key, mapped_type val,
           const Context_t& ctx = Updater_t::defaultContext());


  /**
   * @brief Add an element to the map, or overwrite an existing one.
   * @param key The key of the new item to add.
   * @param val The value of the new item to add.
   * @param ctx Execution context.
   *
   * This will overwrite an existing entry.
   * The first element in the returned pair is an iterator referencing
   * the added item.  The second is a flag that is true if a new element
   * was added.
   */
  std::pair<const_iterator, bool>
  insert_or_assign (const key_type& key, mapped_type val,
                    const Context_t& ctx = Updater_t::defaultContext());


  /**
   * @break Add an element to the map.
   * @param p The item to add.
   *          Should be a pair where first is the string key
   *          and second is the integer value.
   *
   * This will not overwrite an existing entry.
   * The first element in the returned pair is an iterator referencing
   * the added item.  The second is a flag that is true if a new element
   * was added.
   */
  template <class PAIR>
  std::pair<const_iterator, bool> insert (const PAIR& p);


  /**
   * @brief Insert a range of elements to the map.
   * @param first Start of the range.
   * @param last End of the range.
   *
   * The range should be a sequence of pairs where first is the string key
   *  and second is the integer value.
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
   * @brief Called when this thread is no longer referencing anything
   *        from this container.
   * @param ctx Execution context.
   */
  void quiescent (const Context_t& ctx);


private:
  /**
   * @brief Convert an underlying key value to a string pointer.
   * @param val The underlying key value.
   */
  static const std::string* keyAsString (val_t val);


  /**
   * @brief Convert a string pointer to an underlying key value.
   * @param val The string pointer.
   */
  static val_t keyAsVal (const std::string* s);


  /**
   * @brief Convert an underlying mapped value to this type's mapped value.
   * @param val The underlying mapped value.
   */
  static mapped_type mappedAsMapped (val_t val);


  /**
   * @brief Convert this type's mapped value to an underlying mapped value.
   * @param val The mapped value.
   */
  static val_t mappedAsVal (mapped_type val);


  /**
   * @brief Do a lookup in the table.
   * @param key The key to look up.
   *
   * Returns an iterator of the underlying map pointing at the found
   * entry or end();
   */
  typename Impl_t::const_iterator get (const key_type& key) const;


  /**
   * @brief Insert / overwrite an entry in the table.
   * @param key The key of the new item to add.
   * @param val The value of the new item to add.
   * @param overwrite If true, allow overwriting an existing entry.
   * @param ctx Execution context.
   *
   * The first element in the returned pair is an iterator referencing
   * the added item.  The second is a flag that is true if a new element
   * was added.
   */
  std::pair<const_iterator, bool>
  put (const key_type& key,
       mapped_type val,
       bool overwrite = true,
       const Context_t& ctx = Updater_t::defaultContext());


  /**
   * @brief Hash functional for keys.
   *
   * The key can be either a std::string or the representation type
   * used by the underlying map.
   */
  struct Hasher
  {
    /// Hash function from the underlying representation type.
    size_t operator() (const val_t p) const;
    /// Hash function from a std::string.
    size_t operator() (const std::string& s) const;
    /// Hash functional.
    std::hash<std::string> m_hash;
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


#include "CxxUtils/ConcurrentStrMap.icc"


#endif // not CXXUTILS_CONCURRENTSTRMAP_H
