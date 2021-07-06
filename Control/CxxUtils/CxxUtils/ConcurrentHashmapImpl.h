// This file's extension implies that it's C, but it's really -*- C++ -*-.
/*
 * Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration.
 */
/**
 * @file CxxUtils/ConcurrentHashmapImpl.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Dec, 2020
 * @brief Hash table allowing concurrent, lockless reads.
 */



#ifndef CXXUTILS_CONCURRENTHASHMAPIMPL_H
#define CXXUTILS_CONCURRENTHASHMAPIMPL_H


#include "CxxUtils/bitscan.h"
#include "CxxUtils/atomic_fetch_minmax.h"
#include <functional>
#include <cstdint>
#include <cstdlib>
#include <atomic>
#include <mutex>
#include <memory>
#include <new>


class ConcurrentHashmapImplTest;


namespace CxxUtils {
namespace detail {


/// Type used for keys and values --- an unsigned big enough to hold a pointer.
/// Need to have this defined outside of ConcurrentHashmapImpl itself
/// in order to avoid instantiation circularities, as the HASHER_ and MATCHER_
/// classes will probably want to use it.
using ConcurrentHashmapVal_t = uintptr_t;


/**
 * @brief Helper to generate hash probes.
 *
 * To search for an entry with hash code @c hash in the table pointed
 * at by @c entries:
 *@code
 *  CHMTableInterator<ENTRIES_PER_CACHELINE> it (hash, mask, maskBits, probleLimit);
 *  do {
 *    entry_t* ent = entries + it.offset();
 *    <<test if ent is the desired entry and handle if so>>
 *  } while (it.next());
 *  // Too many probes --- failed.
 @endcode
 *
 * The template argument is the number of table entries per cache line.
 * We try to be cache friendly by first searching all entries in a cache line,
 * then moving to another line.
 */
template <unsigned ENTRIES_PER_CACHELINE>
struct CHMTableIterator
{
  /// Mask for the within-cachline part of indices.
  static  constexpr size_t ENTRIES_PER_CACHELINE_MASK = ENTRIES_PER_CACHELINE-1;

  /**
   * @brief Constructor.
   * @param hash The hash of the entry we're looking for.
   * @param mask Table mask; i.e., the table capacity - 1.
   * @param maskBits Number of 1 bits in mask.
   * @param probeLimit Maximum number of probes to try before failing.
   */
  CHMTableIterator (size_t hash, size_t mask, size_t maskBits, size_t probeLimit);


  /**
   * @brief Offset of the element currently being probed.
   */
  size_t offset() const;


  /**
   * @brief Return the number of probes performed so far.
   */
  size_t nprobes() const;


  /**
   * @brief Move to the next probe.
   * Returns true if we should continue, or false if we've hit the maximum
   * number of probes.
   */
  bool next();


private:
  /// Mask for the table; i.e., capacity-1, but with the low bits
  /// corresponding to ENTRIES_PER_CACHELINE also masked off.
  const size_t m_mask;
  /// Offset of the first entry we probe within its cacheline.
  const size_t m_boffs;
  /// Base increment between probes.
  const size_t m_stride;
  /// Maximum number of probes to try.
  const size_t m_probeLimit;
  /// Index of the start of the cacheline currently being probed.
  size_t m_bucket;
  /// Number of probes tried so far.
  size_t m_nprobes;
};


/**
 * @brief Hash table allowing concurrent, lockless reads.
 *
 * This class implements a simple hash map from uintptr_t to uintptr_t.
 * This class isn't meant to be used directly; rather, a more user-friendly
 * interface can be built on top of this.  Although here we deal only
 * with uintptr_t values, the hash and comparison operations are templated.
 * This allows for handling more complex types, where the values
 * stored in the map are pointers to the actual objects.  We support
 * inserting new items and modifying existing ones, but not deletion.
 * One value of the key must be reserved to indicate null; no keys with
 * that value may be inserted.
 *
 * There can be only one writer at a time; this is enforced with internal locks.
 * However, there can be any number of concurrent readers at any time.
 * The reads are lockless.  So this is appropriate when reads
 * are much more frequent than writes.
 *
 * Template arguments:
 *  UPDATER_ - Object used for memory management; see below.
 *             This has the same requirements as for ConcurrentRangeMap;
 *             see there for further details.
 *  HASHER_  - Functional to compute a hash value from a key.
 *             Defaults to std::hash.
 *  MATCHER_ - Functional to compare two keys for equality.
 *             Defaults to std::equal_to.
 *  NULLVAL_ -  Value of the key to be considered null.
 *              A key with this value may not be inserted.
 *
 * Implementation notes:
 *  We use open addressing (see, eg, knuth AOCP 6.4), in which the payloads
 *  are kept in the hash table itself.  We try to be cache friendly
 *  by probing all entries within a cache line before trying anything
 *  in a different cache line.  If the table gets full, we make a new, larger,
 *  instance copying the data from the old one.  The old table instances
 *  are then given to the Updater object to handle.
 *
 *  The implementation here is inspired by the hash maps from ConcurrencyKit
 *  (http://concurrencykit.org), though the code is all new.
 *
 * nb. Can't use plain `MATCHER' as a template argument because it collides
 * with gtest.
 */
template <template <class> class UPDATER_,
          typename HASHER_ = std::hash<uintptr_t>,
          typename MATCHER_ = std::equal_to<uintptr_t>,
          uintptr_t NULLVAL_ = 0>
class ConcurrentHashmapImpl
{
public:
  /// Type used for keys and values --- an unsigned big enough to hold a pointer.
  using val_t = ConcurrentHashmapVal_t;
  /// Hash object.
  using Hasher_t = HASHER_;
  /// Key match object.
  using Matcher_t = MATCHER_;
  /// Null key value.
  static constexpr uintptr_t nullval = NULLVAL_;
  /// Used to represent an invalid table index.
  static constexpr size_t INVALID = static_cast<size_t>(-1);


private:
  /// One entry in the hash table.
  struct entry_t {
    std::atomic<val_t> m_key;
    std::atomic<val_t> m_val;
  };

  /// Assumed length in bytes of one cache line.
  static constexpr size_t CACHELINE = 64;

  // Ensure that entries will evenly pack a cache line.
  // If CACHELINE is a power of two, this implies that sizeof(entry_t)
  // is as well.
  static_assert (CACHELINE >= sizeof (entry_t) &&
                 CACHELINE % sizeof(entry_t) == 0);

  /// Number of entries in one cache line.
  static constexpr size_t ENTRIES_PER_CACHELINE = CACHELINE / sizeof(entry_t);
  /// Mask for the cache line part of an index.
  static constexpr size_t ENTRIES_PER_CACHELINE_MASK = (ENTRIES_PER_CACHELINE-1);

  /// For unit testing.
  friend class ::ConcurrentHashmapImplTest;


  /**
   * @brief Table of hash entries.
   *
   * This is the actual table of hash entries.  It consists of a fixed-size
   * header, followed by the actual array of entries.  We override new
   * in order to be able to properly allocate the space for the array.
   * The start of the array of entries need to be aligned on a cache line.
   * We make a new instance of this if the table needs to be grown.
   */
  class alignas(CACHELINE) Table
  {
  public:
    using TableIterator = CHMTableIterator<ENTRIES_PER_CACHELINE>;


    /**
     * @brief Constructor.
     * @param capacity Number of entries in the table.  Must be a power of 2.
     * @param hasher Hash object to use.
     * @param matcher Key match object to use.
     */
    Table (size_t capacity,
           const Hasher_t& hasher = Hasher_t(),
           const Matcher_t& matcher = Matcher_t());


    /**
     * @brief Allocator for table objects.
     * @param capacity Size of the table (must be a power of 2).
     *
     * Allocate with enough space for the table of entries.
     * Also align on a cache line.
     */
    static void* operator new (size_t, size_t capacity);


    /**
     * @brief Deallocator for table objects.
     */
    void operator delete (void* p);

    
    /**
     * @brief Find a table entry for reading.
     * @param key The key for which to search.
     * @param hash The hash of the key.
     *
     * Returns the matching entry, or nullptr.  xxx
     */
    size_t probeRead (val_t key, size_t hash) const;

    
    /**
     * @brief Find a table entry for writing.
     * @param key The key for which to search.
     * @param hash The hash of the key.
     * @param entry[out] The entry found.
     *
     * If we find the entry, return true with @c entry pointing at it.
     * If we don't find it, and there's still room in the table, return false
     * with @c entry pointing at the next empty entry.
     * Otherwise, return false with @c entry set to nullptr.
     */
    size_t probeWrite (val_t key, size_t hash, bool& insert);


    /**
     * @brief The number of entries in the table.
     */
    size_t capacity() const;


    /**
     * @brief Return the entry for an offset.
     * @param offset The index of the desired entry.
     */
    const entry_t& entry (size_t offset) const;


    /**
     * @brief Return the entry for an offset (non-const).
     * @param offset The index of the desired entry.
     */
    entry_t& entry (size_t offset);


  private:
    /// Number of entries in the table.  Must be a power of 2.
    const size_t m_capacity;
    /// Maximum number of probes allowed before resizing the table.
    const size_t m_maxProbe;
    /// Mask for table indices (capacity-1).
    const size_t m_mask;
    /// Number of bits in the mask.
    const size_t m_maskBits;
    /// The hash object.
    const Hasher_t& m_hasher;
    /// The key match object.
    const Matcher_t& m_matcher;
    /// Longest probe needed so far.
    std::atomic<size_t> m_longestProbe;
    /// The actual table entries.
    alignas(CACHELINE) entry_t m_entries[1];
  };


public:
  /// Updater object.
  using Updater_t = UPDATER_<Table>;
  /// Context type for the updater.
  using Context_t = typename Updater_t::Context_t;



  /**
   * @brief Constructor.
   * @param updater Object used to manage memory
   *                (see comments at the start of the class).
   * @param capacity Minimum initial table size.
   * @param hasher Hash object to use.
   * @param matcher Key match object to use.
   * @param ctx Execution context.
   */
  ConcurrentHashmapImpl (Updater_t&& updater,
                         size_t capacity_in,
                         const Hasher_t& hasher,
                         const Matcher_t& matcher,
                         const typename Updater_t::Context_t& ctx);


  // Don't implment copying.
  // This should be done by derived classes, if desired.
  ConcurrentHashmapImpl (const ConcurrentHashmapImpl&) = delete;
  ConcurrentHashmapImpl& operator= (const ConcurrentHashmapImpl&) = delete;
  

  /**
   * @brief Return the number of items currently stored.
   *   (not necessarity synced)
   */
  size_t size() const;


  /**
   * @brief Return the current table size.
   */
  size_t capacity() const;


  /** 
   * @brief Return the hasher object.
   */
  const Hasher_t& hasher() const;


  /** 
   * @brief Return the matcher object.
   */
  const Matcher_t& matcher() const;


  /**
   * @brief Bidirectional iterator over occupied table entries.
   *
   * This is not itself a compliant STL iterator.
   * Derived classes are meant to build a user-facing iterator on top of this.
   */
  class const_iterator
  {
  public:
    /**
     * @brief Constructor.
     * @param table The table instance we're referencing.
     * @param end If true, initialize this to an end iterator.
     *            Otherwise, initialize it to a a begin iterator.
     */
    const_iterator (const Table& table, bool end);


    /**
     * @brief Constructor.
     * @param table The table instance we're referencing.
     * @param offset Offset of the iterator within the table.
     *               (Must point at an occupied entry.)
     */
    const_iterator (const Table& table, size_t offset);


    /**
     * @brief Advance the iterator to the next occupied entry.
     */
    void next();


    /**
     * @brief Move the iterator back to the previous occupied entry.
     */
    void prev();


    /**
     * @brief Return the key for this iterator.
     */
    val_t key() const;
    

    /**
     * @brief Return the value for this iterator.
     */
    val_t value() const;


    /**
     * @brief Compare two iterators.
     */
    bool operator!= (const const_iterator& other) const;


    /**
     * @brief Check that the iterator is valid (not pointing at the end).
     */
    bool valid() const;
    
    
  private:
    /// The table over which we're iterating.
    const Table& m_table;
    /// The current position in the table.
    /// Set to -1 for an end iterator.
    size_t m_offset;
  };


  /**
   * @brief Add an entry to the table.
   * @param key The key to insert.
   * @param hash The hash of the key.
   * @param val The value to insert.
   * @param overwrite If true, then overwrite an existing entry.
   *                  If false, an existing entry will not be changed.
   * @param ctx Execution context.
   *
   * If the key already exists, then its value will be updated.
   * Returns an iterator pointing at the entry and a flag which is
   * true if a new element was added.
   */
  std::pair<const_iterator, bool>
  put (val_t key, size_t hash, val_t val,
       bool overwrite,
       const typename Updater_t::Context_t& ctx);


  /**
   * @brief Look up an entry in the table.
   * @param key The key to find.
   * @param hash The hash of the key.
   *
   * Returns an iterator pointing at the found entry, or end().
   */
  const_iterator get (val_t key, size_t hash) const;


  /// Two iterators defining a range.
  using const_iterator_range = std::pair<const_iterator, const_iterator>;


  /**
   * @brief Return a range that can be used to iterate over the container.
   */
  const_iterator_range range() const;


  /** 
   * @brief A begin iterator for the container.
   */
  const_iterator begin() const;


  /** 
   * @brief An end iterator for the container.
   */
  const_iterator end() const;


  /**
   * @brief Erase the table and change the capacity.
   * @param capacity The new table capacity.
   * @param ctx Execution context.
   *
   * Returns an iterator pointing at the start of the old table.
   */
  const_iterator clear (size_t capacity,
                        const typename Updater_t::Context_t& ctx);


  /**
   * @brief Erase the table (don't change the capacity).
   * @param ctx Execution context.
   *
   * Returns an iterator pointing at the start of the old table.
   */
  const_iterator clear (const typename Updater_t::Context_t& ctx);


  /**
   * @brief Increase the table capacity.
   * @param capacity The new table capacity.
   * @param ctx Execution context.
   *
   * No action will be taken in @c capacity is smaller
   * than the current capacity.
   */
  void reserve (size_t capacity,
                const typename Updater_t::Context_t& ctx);


  /**
   * @brief Called when this thread is no longer referencing anything
   *        from this container.
   * @param ctx Execution context.
   */
  void quiescent (const typename Updater_t::Context_t& ctx);


private:
  using mutex_t = std::mutex;
  using lock_t  = std::lock_guard<mutex_t>;


  /**
   * @brief Make the table larger.
   * @param ctx Execution context.
   *
   * Must be holding a lock on the mutex to call this.
   */
  bool grow (lock_t& /*lock*/, const typename Updater_t::Context_t& ctx);


  /**
   * @brief Make the table larger.
   * @param new_capacity The new table capacity (must be a power of 2).
   * @param ctx Execution context.
   *
   * Must be holding a lock on the mutex to call this.
   */
  bool grow (lock_t& /*lock*/, size_t new_capacity, const typename Updater_t::Context_t& ctx);


  static uint64_t round_up (uint64_t);


  /// Updater object managing memory.  See above.
  Updater_t m_updater;
  /// The hash object.
  const Hasher_t m_hasher;
  /// The key match object.
  const Matcher_t m_matcher;
  /// The current table instance.  Must be holding the mutex to access this.
  Table* m_table;
  /// Number of entries in the map.
  std::atomic<size_t> m_size;
  /// Mutex to serialize changes to the map.
  std::mutex m_mutex;
};


} // namespace detail


} // namespace CxxUtils


#include "CxxUtils/ConcurrentHashmapImpl.icc"


#endif // not CXXUTILS_CONCURRENTHASHMAPIMPL_H
