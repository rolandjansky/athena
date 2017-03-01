// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file AthenaKernel/RCUObject.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Aug, 2016
 * @brief read-copy-update (RCU) style synchronization for Athena.
 *
 * Read-copy-update (RCU) is a style of synchronization that is appropriate
 * to a read-often, update-seldom case.  More precisely:
 *
 *  - Reads are frequent and low overhead is important.
 *  - Updates are infrequent, and extra overhead doesn't matter much.
 *  - Exact time ordering between reads and updates is not important.
 *    That is, if a read happens at about the same time as an update,
 *    it doesn't matter if the read sees the data before or after
 *    the update (as long as the reader sees something that is consistent).
 *
 * For a more detailed introduction see, for example:
 *
 *    <https://lwn.net/Articles/262464>
 *    <https://lwn.net/Articles/573424>
 *
 * There are many different styles of RCU implemenation; the one we implement
 * here is close to the `quiescent-state-based RCU' (QSBR) variant discussed
 * in the second article above.  Here, readers take out no locks whatsoever.
 * It follows then that the updater cannot directly change the object
 * that is being read.  Instead, it makes an updated copy of the object
 * and then publishes it by atomically changing a pointer to the object.
 * The remaining question is then when can the old object be deleted.
 * It is necessary to delay this until until no thread can be reading
 * the old version of the object.
 *
 * In this implementation, the updater does not wait for this to happen.
 * Instead, after the update, each event slot (other than the updater)
 * is marked as being in a `grace' period (via an array of flags
 * in RCUObject).  A grace period ends for a given event slot when
 * the object is declared `quiescent'; that is, it is at that time
 * not referencing the object.  Once the grace periods for all event
 * slots have finished, then old versions of the object can be deleted.
 *
 * It is possible to explicitly declare an object as quiescent, either
 * by calling the quiescent() method or by using RCUReadQuiesce.
 * Objects can also be registered with the Athena RCUSvc, which will
 * declare objects quiescent at the end of an event.
 *
 * The current implementation is very simple, and is suitable for a small
 * number of objects that change infrequently.
 *
 *   - If objects are being updated ~ every event and one is relying
 *     on RCUSvc to mark them as quiescent, then it's possible that we'll
 *     never be out of the grace period for all event slots simultaneously,
 *     preventing objects from being cleaned up.
 *
 *   - If there are a large number of infrequently-changing objects,
 *     then RCUSvc may waste time marking as quiescent objects that
 *     have not changed.
 *
 *  If either of these becomes an issue, they can be addressed with a more
 *  elaborate implementation.
 *
 * Usage:
 *
 *  The usual usage is to create an RCUObject instance via the RCUSvc.
 *  RCUObject is templated on the type of the controlled object.
 *
 *@code
 *  ServiceHandle<Athena::RCUSvc> rcusvc ("Athean::RCUSvc", "test");
 *  std::unique_ptr<Athena::RCUObject<Payload> > =
 *    rcusvc->newrcu<Payload>();
 @endcode
 *
 * @c newrcu may also take additional arguments that will be passed
 * to the constructor for @c Payload.  This will register the object
 * with the service.  One can alternately create a @c RCUObject directly,
 * passing it the number of event slots.
 *
 * To read the controlled object, create a @c RCURead object:
 *
 *@code
 *  const Athena::RCUObject<Payload>& rcuobj = ...;
 *  {
 *    Athena::RCURead<Payload> r (rcuobj);
 *    ret = r->someMethodOnPayload();
 *  }
 @endcode
 *
 * and to update use @c RCUUpdate:
 *
 *@code
 *  Athena::RCUObject<Payload>& rcuobj = ...;
 *  {
 *    Athena::RCUUpdate<Payload> u (rcuobj);
 *    auto newPayload = std::make_unique<Payload> (*u);
 *    newPayload->updateSomeStuff();
 *    u.update (std::move (newPayload));
 *  }
 @endcode
 *
 * To explicitly declare an object as quiescent, call the @c quiescent method.
 * This takes an @c EventContext object, or it can be defaulted, in which
 * case the current global context object will be used.  You can also
 * use @c RCUReadQuiesce instead of @c RCURead to automatically call
 * @c quiescent when the read is complete (when the @c RCUReadQuiesce
 * object is destroyed).
 */


#ifndef ATHENAKERNEL_RCUOBJECT_H
#define ATHENAKERNEL_RCUOBJECT_H


#include "CxxUtils/make_unique.h"
#include "GaudiKernel/ThreadLocalContext.h"
#include "GaudiKernel/EventContext.h"
#include "boost/dynamic_bitset.hpp"
#include <atomic>
#include <deque>
#include <vector>
#include <memory>
#include <mutex>


namespace Athena {


class IRCUSvc;
template <class T> class RCURead;
template <class T> class RCUReadQuiesce;
template <class T> class RCUUpdate;



/**
 * @brief Base object class for RCU-style synchronization for Athena.
 */
class IRCUObject
{
public:
  /**
   * @brief Constructor, with RCUSvc.
   * @param svc Service with which to register.
   *
   * The service will call @c quiescent at the end of each event.
   */
  IRCUObject (IRCUSvc& svc);


  /**
   * @brief Constructor, with event slot count.
   * @param nslots Number of active event slots.
   *
   * This version does not register with a service.
   */
  IRCUObject (size_t nslots);


  /**
   * @brief Destructor.
   *
   * Remove this object from the service if it has been registered.
   */
  virtual ~IRCUObject();


  /**
   * @brief Declare the current event slot of this object to be quiescent.
   *
   * This will take out a lock and possibly trigger object cleanup.
   */
  void quiescent();


  /**
   * @brief Declare the event slot @c ctx of this object to be quiescent.
   * @param ctx The event context.
   *
   * This will take out a lock and possibly trigger object cleanup.
   */
  void quiescent (const EventContext& ctx);


protected:
  typedef std::mutex mutex_t;
  typedef std::lock_guard<mutex_t> lock_t;


  /**
   * @brief Declare that the grace period for a slot is ending.
   * @param Lock object (external locking).
   * @param ctx Event context for the slot.
   * @returns true if any slot is still in a grace period.
   *          false if no slots are in a grace period.
   *
   * The caller must be holding the mutex for this object.
   */
  bool endGrace (lock_t& /*lock*/, const EventContext& ctx);


  /**
   * @brief Declare that all slots are in a grace period.
   * @param Lock object (external locking).
   * @param ctx Event context for the slot.
   *
   * The caller must be holding the mutex for this object.
   */
  void setGrace (lock_t& /*lock*/);


  /**
   * @brief Return the mutex for this object.
   */
  mutex_t& mutex();


  /**
   * @brief Delete all old objects.
   * @param Lock object (external locking).
   *
   * The caller must be holding the mutex for this object.
   */
  virtual void clearOld (lock_t& /*lock*/) = 0;


private:
  /// The mutex for this object.
  std::mutex m_mutex;

  /// The service with which we're registered, or null.
  IRCUSvc* m_svc;

  /// Bit[i] set means that slot i is in a grace period.
  boost::dynamic_bitset<> m_grace;
};


/**
 * @brief Wrapper object controlled by RCU synchonization.
 *
 * See the comments at the top of the file for complete comments.
 */
template <class T>
class RCUObject
  : public IRCUObject
{
public:
  typedef RCURead<T> Read_t;
  typedef RCUReadQuiesce<T> ReadQuiesce_t;
  typedef RCUUpdate<T> Update_t;


  /**
   * @brief Constructor, with RCUSvc.
   * @param svc Service with which to register.
   * @param args... Additional arguments to pass to the @c T constructor.
   *
   * The service will call @c quiescent at the end of each event.
   */
  template <typename... Args>
  RCUObject (IRCUSvc& svc, Args&&... args);


  /**
   * @brief Constructor, with number of slots.
   * @param nslots Number of event slots.
   * @param args... Additional arguments to pass to the @c T constructor.
   */
  template <typename... Args>
  RCUObject (size_t nslots, Args&&... args);


  /**
   * @brief Return a reader for this @c RCUObject.
   */
  Read_t reader() const;


  /**
   * @brief Return a reader for this @c RCUObject.
   *        When destroyed, this reader will declare
   *        the @c RCUObject as quiescent
   *
   * This version will read the global event context.
   */
  ReadQuiesce_t readerQuiesce();


  /**
   * @brief Return a reader for this @c RCUObject.
   *        When destroyed, this reader will declare
   *        the @c RCUObject as quiescent
   * @param ctx The event context.
   */
  ReadQuiesce_t readerQuiesce (const EventContext& ctx);


  /**
   * @brief Return an updater for this @c RCUObject.
   *
   * This version will read the global event context.
   */
  Update_t updater();


  /**
   * @brief Return an updater for this @c RCUObject.
   * @param ctx The event context.
   */
  Update_t updater (const EventContext& ctx);


private:
  /**
   * @brief Delete all old objects.
   * @param Lock object (external locking).
   *
   * The caller must be holding the mutex for this object.
   */
  virtual void clearOld (lock_t& /*lock*/) override;


private:
  template <class U> friend class RCUReadQuiesce;
  template <class U> friend class RCURead;
  template <class U> friend class RCUUpdate;

  /// Pointer to the current object.
  std::atomic<const T*> m_obj;

  /// List of all allocated objects.
  /// The current object is the one at the front;
  /// the others are pending cleanup.
  std::deque<std::unique_ptr<T> > m_objs;
};


/**
 * @brief Helper to read data from a @c RCUObject.
 * 
 * See the header comments for details.
 */
template <class T>
class RCURead
{
public:
  /**
   * @brief Constructor.
   * @param rcuobj The @c RCUObject we're reading.
   */
  RCURead (const RCUObject<T>& rcuobj);


  /**
   * @brief Access data.
   */
  const T& operator*() const;


  /**
   * @brief Access data.
   */
  const T* operator->() const;

  
private:
  /// The data object we're reading.
  const T& m_obj;
};


/**
 * @brief Helper to read data from a @c RCUObject.
 *        When this object is deleted, the @c RCUObject is declared
 *        quiescent for this event slot.
 * 
 * See the header comments for details.
 */
template <class T>
class RCUReadQuiesce
  : public RCURead<T>
{
public:
  /**
   * @brief Constructor.
   * @param rcuobj The @c RCUObject we're reading.
   *
   * This version will read the global event context.
   */
  RCUReadQuiesce (RCUObject<T>& rcuobj);


  /**
   * @brief Constructor.
   * @param rcuobj The @c RCUObject we're reading.
   * @param ctx The event context.
   */
  RCUReadQuiesce (RCUObject<T>& rcuobj, const EventContext& ctx);


  /**
   * @brief Destructor.
   *
   * Mark this event slot quiescent.
   */
  ~RCUReadQuiesce();


private:
  /// The RCUObject we're referencing.
  RCUObject<T>& m_rcuobj;

  /// The current event context.
  const EventContext& m_ctx;
};


/**
 * @brief Helper to update data in a @c RCUObject.
 * 
 * See the header comments for details.
 */
template <class T>
class RCUUpdate
{
public:
  /**
   * @brief Constructor.
   * @param rcuobj The @c RCUObject we're reading.
   *
   * This version will read the global event context.
   */
  RCUUpdate (RCUObject<T>& rcuobj);

  
  /**
   * @brief Constructor.
   * @param rcuobj The @c RCUObject we're reading.
   * @param ctx The event context.
   */
  RCUUpdate (RCUObject<T>& rcuobj, const EventContext& ctx);

  
  /**
   * @brief Access data.
   */
  const T& operator*() const;


  /**
   * @brief Access data.
   */
  const T* operator->() const;


  /**
   * @brief Publish a new version of the data object.
   * @param ptr The new data object.
   */
  void update (std::unique_ptr<T> ptr);

private:
  /// The object we're referencing.
  RCUObject<T>& m_rcuobj;

  /// The event context.
  const EventContext& m_ctx;

  /// Lock for synchonization.
  std::lock_guard<std::mutex> m_g;
};


} // namespace Athena


#include "AthenaKernel/RCUObject.icc"


#endif // not ATHENAKERNEL_RCUOBJECT_H
