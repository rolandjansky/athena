// This file's extension implies that it's C, but it's really -*- C++ -*-.
/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/
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
#include <functional>


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
 * The only thing we need to do with the contained pointers is to delete them.
 * Rather than doing that directly, we take a deletion function as an argument
 * to the constructor; this should be a void function that takes a pointer argument
 * and deletes it.  This allows one to instantiate this template with @c void as @c T,
 * to reduce the amount of generated code.  The @c emplace method takes
 * a @c unique_ptr as an argument.  We define @c payload_unique_ptr which
 * is a @c unique_ptr to @c T that does deletion by calling an arbitrary function.
 * @c payload_unique_ptr may be initialized from a @c unique_ptr; to construct
 * one directly, the deletion function should be passed as a second argument
 * to the @c payload_unique_ptr constructor.
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
 *  bool inRange (const KEY& k, const RANGE& r) const;
 *
 *  // Test if two ranges overlap, and adjust if needed.
 *  // OLDRANGE is an existing range in the container, and NEWRANGE
 *  // is a new one being added.  Return one of:
 *  //   0 -- no overlap between the ranges; NEWRANGE is unmodified.
 *  //   1 -- ranges overlap.  NEWRANGE has been adjusted to avoid the overlap.
 *  //        If the start of NEWRANGE is changed, it must
 *  //        only be moved forward (increased), never backwards.
 *  //  -1 -- duplicate: NEWRANGE is entirely inside OLDRANGE.
 *  //        Delete the new range.         
 *  bool overlap (const Context_t& ctx,
 *                const RANGE& oldRange, RANGE& newRange) const;
 *
 *  // Possibly extend an existing range at the end.
 *  // RANGE is the existing range, and NEWRANGE is the range
 *  // being added.  Returns one of:
 *  // Required only for extendLastRange --- which see.
 *  //   0 -- no change was made to RANGE.
 *  //   1 -- RANGE was extended. 
 *  //  -1 -- newRange is a duplicate.
 *  int extendRange (Range& range, const Range& newRange) const;
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
 *   - void discard (std::unique_ptr<T> p);
 *     Explicitly discard an object, deferring until no thread
 *     can be referencing it.
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


  /// Function to delete a @c T*
  typedef void delete_function (const T*);


  /**
   * @brief @c unique_ptr deletion class for a payload object.
   *
   * We can't use the unique_ptr default because we want to allow
   * instantiating with a @c void.
   */
  struct DeletePayload
  {
    /// Initialize with an explicit deletion function.
    // cppcheck-suppress uninitMemberVar  // false positive
    DeletePayload (delete_function* delfcn)
      : m_delete (delfcn)
    {
    }

    /// Allow initializing a @c payload_unique_ptr from a @c std::unique_ptr<U>.
    template <class U>
    static void delfcn (const T* p)
    {
      delete reinterpret_cast<const U*>(p);
    }
    template <class U>
    // cppcheck-suppress uninitMemberVar  // false positive
    DeletePayload (const std::default_delete<U>&)
    {
      m_delete = delfcn<U>;
    }

    /// Delete a pointer.
    void operator() (const T* p) const
    {
      m_delete (p);
    }

    /// The deletion function.
    delete_function* m_delete;
  };


  /**
   * @brief @c unique_ptr holding a payload object.
   *
   * One may initialize an instance of this in one of two ways.
   * First, from another @c std::unique_ptr:
   *
   *@code
   *   payload_unique_ptr p = std::unique_ptr<U> (...);
   @endcode
   *
   * where U* must be convertible to T*.  In this case, the pointer
   * will be deleted as a U*.
   * Second, one can supply an explicit deletion function:
   *
   *@code
   *   T* tp = ...;
   *   payload_unique_ptr p (tp, delfcn);
   @endcode
   */
  typedef std::unique_ptr<T, DeletePayload> payload_unique_ptr;

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
     * @param delfcn Deletion function.
     * @param capacity Size of the data vector to allocate.
     */
    Impl (delete_function* delfcn,
          size_t capacity = 10);


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
     * @brief Queue a payload object for deletion.
     */
    void discard (const T* p);


  private:
    /// Deletion function.
    delete_function* m_delete;

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
   * @param delfcn Deletion function.
   * @param capacity Initial capacity of the map.
   * @param compare Comparison object.
   */
  ConcurrentRangeMap (Updater_t&& updater,
                      delete_function* delfcn,
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


  /// Results returned from emplace().
  enum class EmplaceResult
  {
    /// All ok.
    SUCCESS,

    /// New object was added, but overlaps with an existing range.
    OVERLAP,

    /// New object duplicates an existing range, and was not added.
    DUPLICATE,

    // Existing range was extended to match the new range; new object
    // was deleted.
    EXTENDED
  };


  /**
   * @brief Add a new element to the map.
   * @param range Validity range for this element.
   * @param ptr Payload for this element.
   * @param tryExtend If true, then allow an existing range to be extended
   *                  (see below).
   * @param ctx Execution context.
   *
   * Returns SUCCESS if the new element was successfully inserted.
   * Returns DUPLICATE if the range compared equal to an existing one.
   *         In that case, no new element is inserted (and @c ptr gets deleted).
   * Returns EXTEND if the range of the last element was extended to @c range.
   *         This happens if @c tryExtend is true, @c range is equal
   *         to the range of the last element (according to @c m_compare),
   *         and the range can be extended according to @c extendRange.
   *         (This will generally mean that the start time of @c range
   *         matches the last range, and end time of @c range is after
   *         the end time of the last range.)  In this case, again no
   *         new element is inserted and @c ptr is deleted.
   * Returns OVERLAP if the range of the new element overlaps
   *         an existing element (the new element is still inserted).
   */
  EmplaceResult emplace (const RANGE& range,
                         payload_unique_ptr ptr,
                         bool tryExtend = false,
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
   * @brief Extend the range of the last entry of the map.
   * @param newRange New range to use for the last entry.
   * @param ctx Execution context.
   *
   * The range of the last entry in the map is updated to @c newRange.
   * Does nothing if the map is empty.
   * This will make a new version of implementation data.
   *
   * The semantics of what it means to extend a range are given by the
   * @c extendRange method of the @c COMPARE object (see above).
   *
   * Returns -1 if there was an error; 1 if the last range was extended;
   * and 0 if nothing was changed.
   */
  int extendLastRange (const RANGE& newRange,
                       const typename Updater_t::Context_t& ctx =
                         Updater_t::defaultContext());


  /**
   * @brief Update all range objects.
   * @param rangeUpater Functional to call on each range object.
   * @param ctx Execution context.
   *
   * This will iterate through the list of entries and call @c rangeUpdater
   * on the @c RANGE part of each.  Be careful: rangeUpdater must not
   * change any part of the range which might affect the sorting
   * of entries.
   */
  void updateRanges (std::function<void (RANGE&)> rangeUpdater,
                     const typename Updater_t::Context_t& ctx =
                       Updater_t::defaultContext());


  /**
   * @brief Remove unused entries from the front of the list.
   * @param keys List of keys that may still be in use.
   *             (Must be sorted.)
   *
   * We examine the objects in the container, starting with the earliest one.
   * If none of the keys in @c keys match the range for this object, then
   * it is removed from the container.  We stop when we either find
   * an object with a range matching a key in @c keys or when there
   * is only one object left.
   *
   * The list @c keys MUST be sorted.
   *
   * Removed objects are queued for deletion once all slots have been
   * marked as quiescent.
   *
   * Returns the number of objects that were removed.
   */
  size_t trim (const std::vector<key_query_type>& keys);


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
   * @brief Return the number times an item was inserted into the map.
   */
  size_t nInserts() const;


  /**
   * @brief Return the maximum size of the map.
   */
  size_t maxSize() const;


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


  /**
   * @brief Return the deletion function for this container.
   */
  delete_function* delfcn() const;


private:
  /// Type of the mutex for this container.
  typedef std::mutex mutex_t;
  typedef std::lock_guard<mutex_t> lock_t;


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

  
  /**
   * @brief Test to see if any keys within @c keys match @c r.
   * @brief r Range to test.
   * @break keys List of keys to test.  MUST be sorted.
   */
  bool anyInRange (const key_type& r,
                   const std::vector<key_query_type>& keys) const;


  /**
   * @brief Install a new implementation instance and make it visible.
   * @param new_impl The new instance.
   * @param new_begin Begin pointer for the new instance.
   * @param new_end End pointer for the new instance.
   *                (Usual STL meaning of end.  If the instance is empty,
   *                then new_end should match new_begin.)
   * @param ctx Execution context.
   */
  void installImpl (std::unique_ptr<Impl> new_impl,
                    value_type* new_begin,
                    value_type* new_end,
                    const typename Updater_t::Context_t& ctx);

  
  /**
   * @brief Extend the range of the last entry of the map.
   * @param Lock object.
   * @param extendedRange New range to use for the last entry.
   * @param ctx Execution context.
   *
   * Implementation of @c extendLastRange; see there for details.
   * Must be called with the lock held.
   */
  int extendImpl (lock_t& lock,
                  const RANGE& extendedRange,
                    const typename Updater_t::Context_t& ctx);



  /// Updater object.  This maintains ownership of the current implementation
  /// class and the older versions.
  Updater_t m_updater;

  /// Comparison object.
  COMPARE m_compare;

  /// Deletion function.
  delete_function* m_delete;

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
  /// Then check that both m_begin is non-null and the previous value
  /// we fetched for last matches what's now in m_last.  If either condition
  /// fails, then re-fetch both pointers.
  std::atomic<value_type*> m_begin;
  std::atomic<value_type*> m_last;

  /// Some basic statistics.
  size_t m_nInserts;
  size_t m_maxSize;

  /// Mutex protecting the container.
  mutex_t m_mutex;
};


} // namespace CxxUtils


#include "CxxUtils/ConcurrentRangeMap.icc"


#endif // not CXXUTILS_CONCURRENTRANGEMAP_H
