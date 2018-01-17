// This file's extension implies that it's C, but it's really -*- C++ -*-.
/*
 * Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration.
 */
// $Id$
/**
 * @file CxxUtils/ConcurrentRangeMap.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Nov, 2017
 * @brief Map from range to payload object, allowing concurrent, lockless reads.
 */


#ifndef CXXUTILS_CONCURRENTRANGEMAP_H
#define CXXUTILS_CONCURRENTRANGEMAP_H


#include "CxxUtils/stall.h"
#include "boost/range/iterator_range.hpp"
#include <atomic>
#include <mutex>
#include <utility>
#include <vector>
#include <memory>
#include <algorithm>


namespace CxxUtils {


/**
 * @brief Map from range to payload object, allowing concurrent, lockless reads.
 *
 * This class implements a map of sorted `range' objects (though only
 * a single value is actually used) to allocated payload objects.
 * Values can be looked up by a key; the value returned will be the
 * first for which the range is not less than the key.  We also
 * support insertions, erasures, and iteration.
 *
 * There can be only one writer at a time; this is enforced with internal locks.
 * However, there can be any number of concurrent readers at any time.
 * The reads are lockless (but not necessarily waitless, though this should
 * not be significant in practice).  So this is appropriate when
 * reads are much more frequent than writes.
 *
 * This implementation also assumes that a lookup is most likely to be
 * for the last element in the map, that insertions are most likely to be
 * at the end, and that erasures are most likely to be from the beginning.
 * This class will still work if these assumptions are violated, but it
 * may be much slower.  (The use case for which this was targeted
 * was that of conditions containers.)
 *
 * Template arguments:
 *  RANGE - The type of the range object stored with each element.
 *  KEY - The type used to look up objects in the container.  This may be
 *        same as RANGE, but not necessarily.  (For example, RANGE may
 *        contain start and end times, while KEY may be a single time.)
 *  COMPARE - Object used for key comparisons; see below.
 *  UPDATER - Object used for memory management; see below.
 *
 * COMPARE should implement a less-than relation, like a typical STL
 * comparison object, with these signatures:
 *
 *@code
 *  bool operator() (const KEY& k1,   const RANGE& r2) const;
 *  bool operator() (const RANGE& r1, const RANGE& r2) const;
 @endcode
 *
 * In order to implement updating concurrently with reading, we need to
 * defer deletion of objects until no thread can be referencing them any more.
 * The policy for this is set by the template UPDATER<T>.  An object
 * of this type owns an object of type T.  It should provide a typedef
 * Context_t, giving a type for an event context, identifying which
 * thread/slot is currently executing.  It should implement these operations:
 *
 *   - const T& get() const
 *     Return the current object.
 *   - void update (std::unique_ptr<T> p, const Context_t ctx);
 *     Atomically update the current object to be p.
 *     Deletion of the previous version should be deferred until
 *     no thread can be referencing it.
 *   - void quiescent (const Context_t& ctx);
 *     Declare that the thread described by ctx is no longer accessing
 *     the object.
 *   - static const Context_t& defaultContext();
 *     Return a context object for the currently-executing thread.
 *
 * For an example, see AthenaKernel/RCUUpdater.h.
 *
 * Implementation notes:
 *   The values we store are pairs of RANGE, const T*.
 *   The data are stored in a vector of such values.
 *   We maintain atomic pointers to the first and last valid elements
 *   in the map.  We can quickly add an element to the end or delete
 *   an element from the beginning by adjusting these pointers.
 *   Any other changes will require making a new copy of the data vector.
 */
template <class RANGE, class KEY, class T, class COMPARE,
          template <class> class UPDATER>
class ConcurrentRangeMap
{
public:
  typedef RANGE key_type;
  typedef const T* mapped_type;
  typedef std::pair<RANGE, const T*> value_type;
  typedef const value_type& const_reference;
  typedef const value_type* const_pointer;
  typedef size_t size_type;
  typedef int difference_type;
  typedef COMPARE key_compare;
  typedef KEY key_query_type;

  typedef const value_type* const_iterator;
  typedef boost::iterator_range<const_iterator> const_iterator_range;


  /**
   * @brief Holds one version of the map.
   *
   * This object holds one version of the map.  Within a single version,
   * we can add elements to the end or remove elements from the beginning.
   * If we need to make other changes, or to expand the map, we must make
   * a new version and copy the contents.  The UPDATER object is then used
   * to install the new version; old versions should be kept until they
   * are no longer referenced by any thread.
   *
   * This object is also used to hold on to erased payloads until it is
   * safe to delete them.
   */
  class Impl
  {
  public:
    /**
     * @brief Constructor.
     * @param capacity Size of the data vector to allocate.
     */
    Impl (size_t capacity = 10);


    /**
     * @brief Destructor.
     *
     * This also deletes payload objects that were passed to discard().
     */
    ~Impl();


    /**
     * @brief Return a pointer to the start of the data vector.
     */
    value_type* data();


    /**
     * @brief Return the size of the current data vector.
     */
    size_t capacity() const;


    /***
     * @brief 
     */
    void discard (const T* p);


  private:
    /// Vector holding the map data.
    std::vector<value_type> m_data;

    /// Payload objects with pending deletion requests.
    /// They'll be deleted when this object is deleted.
    std::vector<const T*> m_garbage;
  };

  typedef UPDATER<Impl> Updater_t;


  /**
   * @brief Constructor.
   * @param updater Object used to manage memory
   *                (see comments at the start of the class).
   * @param capacity Initial capacity of the map.
   * @param compare Comparison object.
   */
  ConcurrentRangeMap (Updater_t&& updater,
                      size_t capacity = 16,
                      const COMPARE& compare = COMPARE());


  /**
   * @brief Destructor.
   *
   * Clean up any remaining payload objects.
   */
  ~ConcurrentRangeMap();


  /**
   * @brief Search for the first item less than or equal to KEY.
   * @param key The key to search for.
   * @returns The value, or nullptr if not found.
   */
  const_iterator find (const key_query_type& key) const;


  /**
   * @brief Add a new element to the map.
   * @param range Validity range for this element.
   * @param ptr Payload for this element.
   * @param ctx Execution context.
   */
  void emplace (const RANGE& range,
                std::unique_ptr<T> ptr,
                const typename Updater_t::Context_t& ctx =
                  Updater_t::defaultContext());


  /**
   * @brief Erase the first item less than or equal to KEY.
   * @param key The key to search erase.
   * @param ctx Execution context.
   */
  void erase (const key_query_type& key,
              const typename Updater_t::Context_t& ctx =
                Updater_t::defaultContext());


  /**
   * @brief Return the current number of elements in the map.
   */
  size_t size() const;


  /**
   * @brief Test if the map is empty.
   */
  bool empty() const;


  /**
   * @brief Return the current capacity of the map.
   */
  size_t capacity() const;


  /**
   * @brief Return a range that can be used to iterate over the container.
   */
  const_iterator_range range() const;


  /**
   * @brief Called when this thread is no longer referencing anything
   *        from this container.
   * @param ctx Execution context.
   */
  void quiescent (const typename Updater_t::Context_t& ctx =
                    Updater_t::defaultContext());

  
private:
  /**
   * @brief Return the begin/last pointers.
   * @param [inout] last Current value of last.
   *
   * Retrieve consistent values of the begin and last pointers.
   * The last pointer should have already been fetched, and may be updated.
   * Usage should be like this:
   *
   *@code
   *  const_iterator last = m_last;
   *  if (!last) return;  // Container empty.
   *  const_iterator begin = getBegin (last);
   *  if (!last) return;  // Container empty.
   @endcode
   */
  const_iterator getBegin (const_iterator& last) const;


  /**
   * @brief Consistently update both the begin and last pointers.
   * @param begin New begin pointer.
   * @param end New end pointer.
   */
  void updatePointers (value_type* new_begin, value_type* new_end);

  
  /// Updater object.  This maintains ownership of the current implementation
  /// class and the older versions.
  Updater_t m_updater;

  /// Comparison object.
  COMPARE m_compare;

  /// Current version of the implementation class.
  Impl* m_impl;

  /// Pointers to the first and last elements of the container.
  /// m_last is not the usual end pointer; it points at the last element
  /// in the container.
  /// If the container is empty, then m_last will be null.
  /// If these are to both be updated together, it should be done in this
  /// order.  First, m_begin should be set to null.  This marks that there's
  /// an update in progress.  Then m_last should be set, followed by m_begin.
  /// To read both pointers, we first fetch m_last.  Then fetch m_begin.
  /// Then that that both m_begin is non-null and the previous value
  /// we fetched for last matches what's now in m_last.  If either condition
  /// fails, then re-fetch both pointers.
  std::atomic<value_type*> m_begin;
  std::atomic<value_type*> m_last;

  /// Mutex protecting the container.
  typedef std::mutex mutex_t;
  typedef std::lock_guard<mutex_t> lock_t;
  mutex_t m_mutex;
};


} // namespace CxxUtils


#include "CxxUtils/ConcurrentRangeMap.icc"


#endif // not CXXUTILS_CONCURRENTRANGEMAP_H
