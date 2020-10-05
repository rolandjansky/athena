// This file's extension implies that it's C, but it's really -*- C++ -*-.
/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/
/*
 */
/**
 * @file AthenaServices/src/DelayedConditionsCleanerSvc.h
 * @author scott snyder <snyder@bnl.gov>
 * @date May, 2018
 * @brief Clean conditions containers after a delay.
 */


#ifndef ATHENASERVICES_DELAYEDCONDITIONSCLEANERTOOLSVC_H
#define ATHENASERVICES_DELAYEDCONDITIONSCLEANERTOOLSVC_H


#include "AthenaBaseComps/AthService.h"
#include "AthenaKernel/IConditionsCleanerSvc.h"
#include "CxxUtils/Ring.h"
#include <queue>
#include <mutex>
#include <cstdint>
#include <atomic>
#include <unordered_map>


namespace Athena {


class DelayedConditionsCleanerTask;
class DelayedConditionsCleanerSvcProps;


/**
 * @brief Clean conditions containers after a delay.
 *
 * This is an implementation of IConditionsCleanerSvc, for doing
 * garbage collection of conditions objects.
 *
 * Briefly, it works like this.
 *
 * When a conditions object is added (@c condObjAdded interface),
 * we put an entry in a priority queue, saying that we want to
 * clean this container CleanDelay events later.
 *
 * On each event (@c event interface) we put the current IOV keys
 * (run+LBN and timestamp) into ring buffers (of size RingSize).
 * If the topmost entry of the priority queue has come due, then we
 * pull off that entry and other entries due up to LookAhead events later.
 * We then do a trim operation on each of the conditions containers,
 * removing conditions objects from the oldest first that do not
 * match any of the IOV keys in the ring buffer.
 *
 * The cleaning can optionally be done as an asynchronous TBB job if
 * Async is true and allowAsync=true is passed to @c event.
 */
class ATLAS_CHECK_THREAD_SAFETY DelayedConditionsCleanerSvc
  : public extends<AthService, IConditionsCleanerSvc>
{
public:
  /// Packed key type.
  typedef CondContBase::key_type key_type;


  /**
   * @brief Standard Gaudi constructor.
   * @param name Service name.
   * @param svc Service locator.
   */
  DelayedConditionsCleanerSvc (const std::string& name, ISvcLocator* svc);

  /**
   * @brief Standard destructor. Needed to avoid problems with unique_ptr
   */
  ~DelayedConditionsCleanerSvc();


  /**
   * @brief Standard Gaudi initialize method.
   */
  virtual StatusCode initialize() override;


  /**
   * @brief Standard Gaudi finalize method.
   */
  virtual StatusCode finalize() override;


  /**
   * @brief Called at the start of each event.
   * @param ctx The current event context.
   * @param allowAsync If true, then cleaning may be run
   *                   in an asynchronous TBB task.
   */
  virtual StatusCode event (const EventContext& ctx, bool allowAsync) override;


  /**
   * @brief Called after a conditions object has been added.
   * @param ctx The current event context.
   * @param cc The container to which the object was added.
   */
  virtual StatusCode condObjAdded (const EventContext& ctx,
                                   CondContBase& cc) override;


  /**
   * @brief Print some statistics about the garbage collection.
   *        Would generally be called in finalize(), but broken out
   *        as a separate interface for testing/debugging purposes.
   */
  virtual StatusCode printStats() const override;


  /**
   * @brief Clear the internal state of the service.
   * Only for testing.  Don't call if any other thread may be touching the service.
   */
  virtual StatusCode reset() override;



private:
  friend class DelayedConditionsCleanerTask;


  /// Ring buffer holding most recent IOV keys of a given type.
  typedef CxxUtils::Ring<key_type> Ring;


  /// Run+LBN or timestamp key?
  using KeyType = CondContBase::KeyType;


  /// Information that we maintain about each conditions container.
  class CondContInfo
  {
  public:
    CondContInfo (CondContBase& cc) : m_cc (cc) {}

    /// The conditions container.  Strictly redundant with the key field
    /// of the map, but we want a non-const version.
    CondContBase& m_cc;

    /// Number of times cleaning was attempted.
    size_t m_nClean = 0;

    /// Total number of objects removed by cleaning.
    size_t m_nRemoved  = 0;

    /// Number of times exactly 0 objects were removed.
    size_t m_removed0 = 0;

    /// Number of times exactly 1 object was removed.
    size_t m_removed1 = 0;

    /// Number of times two or more objects were removed.
    size_t m_removed2plus = 0;
  };


  /**
   * @brief Do cleaning for a set of containers.
   * @param cis Set of containers to clean.
   * @param ring Ring buffer with recent IOV keys.
   * @param slotKeys Vector of current keys for all slots.
   * @param allowAsync Can this task run asynchronously?
   *
   * This will either run cleaning directly, or submit it as a TBB task.
   */
  void scheduleClean (std::vector<CondContInfo*>&& cis,
                      Ring& ring,
                      const std::vector<key_type>& slotKeys,
                      bool allowAsync);


  /**
   * @brief Clean a set of containers.
   * @param cis Set of containers to clean.
   * @param keys Set of IOV keys for recent events.
   */
  void cleanContainers (std::vector<CondContInfo*>&& cis,
                        std::vector<key_type>&& keys);


  /**
   * @brief Clean a single container.
   * @param ci The container to clean.
   * @param keys Set of IOV keys for recent events.
   */
  void cleanContainer (CondContInfo* ci,
                       const std::vector<key_type>& keys);



  /// Two ring buffers for recent IOV keys, one for run+LBN and one for
  /// timestamp.  We only access these from event(), which is called
  /// from the event loop, so no locking is needed.
  Ring m_runlbn;
  Ring m_timestamp;

  /// IOV keys currently in use for each slot.
  std::vector<key_type> m_slotLBN;
  std::vector<key_type> m_slotTimestamp;
  

  /// Map of information, indexed by the conditions container.
  typedef std::unordered_map<CondContBase*, CondContInfo> CCInfoMap_t;
  CCInfoMap_t m_ccinfo;


  /// Item in the work queue.
  /// This is a priority queue, so these should be comparable.
  struct QueueItem
  {
    QueueItem (EventContext::ContextEvt_t evt, CondContInfo& ci)
      : m_evt (evt), m_ci (&ci)
    {}
    
    EventContext::ContextEvt_t m_evt;
    CondContInfo* m_ci;

    bool operator< (const QueueItem& other) const
    {
      // Reversed, since we want the lowest to come first.
      return m_evt > other.m_evt;
    }
  };

  /// Priority queue of pending cleaning requests.
  std::priority_queue<QueueItem> m_work;

  /// Serialize access to m_ccinfo and m_work.
  typedef std::mutex mutex_t;
  typedef std::lock_guard<mutex_t> lock_t;
  mutex_t m_workMutex;


  /// Priority queue statistics.
  size_t m_nEvents = 0;     // Number of times queue was examined.
  size_t m_queueSum = 0;    // Running sum of queue size.
  size_t m_workRemoved = 0; // Count of items removed from the queue.
  size_t m_maxQueue = 0;    // Maximum queue size.


  /// Number of active asynchronous cleaning tasks.
  std::atomic<int> m_cleanTasks {0};

  /// Component properties.
  std::unique_ptr<DelayedConditionsCleanerSvcProps> m_props;
};


} // namespace Athena


#endif // not ATHENASERVICES_DELAYEDCONDITIONSCLEANERTOOLSVC_H
