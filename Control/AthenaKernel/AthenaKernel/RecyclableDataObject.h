// This file's extension implies that it's C, but it's really -*- C++ -*-.
/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/
/*
 */
/**
 * @file AthenaKernel/RecyclableDataObject.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Oct, 2018
 * @brief Helper for recycling objects from event to event.
 */


#ifndef ATHENAKERNEL_RECYCLABLEDATAOBJECT_H
#define ATHENAKERNEL_RECYCLABLEDATAOBJECT_H


// Work around a warning in tbb, found by gcc8.
// Fixed in TBB 2018 U5.
#if defined(__GNUC__) && __GNUC__ >= 8
# pragma GCC diagnostic push
# pragma GCC diagnostic ignored "-Wclass-memaccess"
#endif
#include "tbb/concurrent_queue.h"
#if defined(__GNUC__) && __GNUC__ >= 8
# pragma GCC diagnostic pop
#endif
#include <memory>


namespace Athena {


/**
 * @brief Helper for recycling objects from event to event.
 *
 * In some cases, one wants to reuse already-allocated objects
 * from event to event, rather than deleting and recreating them each time.
 * This has typically been done for complicated objects,
 * such as classes deriving from IdentfiableContainer.
 *
 * Some care is obviously needed for this to work in a MT job.
 * Rather than holding a pointer to a specific object from a class member,
 * we instead maintain a concurrent_queue of pointers to objects.
 * When we want an object, we get one from the queue; if the queue is empty,
 * then we allocate a new one.
 *
 * The object used should derive from @c DataObject.  We bump the reference
 * count by 1 when the object is allocated so that it won't get deleted
 * by StoreGate.  We also override @c release() so that when the reference
 * count goes back to 1 it is `recycled'.  When this happens,
 * we call the @c recycle() method on the object (which should probably
 * be protected) and put the object back on the queue to be allocated again.
 *
 * Example use:
 *@code
 *  class MyDataObj : public DataObject { ...
 *  public:
 *    MyDataObj (bool createColl); ...
 *  protected:
 *    void recycle();
 *  ... };
 *
 *  class MyConverter { ...
 *  private:
 *    RecyclableDataQueue<MyDataObj> m_queue;
 *    ... };
 *
 *  StatusCode MyConverter::createObj (...) { ...
 *    MyDataObj* obj = m_queue.get (true);
 @endcode
 * 
 * @c DOBJ should derive from @c DataObject and should provide
 * a member @c recycle().
 *
 * Implementation note: one needs to be careful about the order in which
 * objects are deleted during finalization.  In particular, it is possible
 * for a Gaudi component that holds a @c RecyclableDataQueue as a member
 * to be deleted while there are still associated data objects registered
 * in the event store.  To be robust against this, we allocate the
 * actual queue object separately from @c RecyclableDataQueue and
 * manage it with @c shared_ptr; @c RecyclableDataObject then references
 * the queue with a @c shared_ptr.
 */
template <class DOBJ>
class RecyclableDataObject : public DOBJ
{
public:
  /// Underlying queue type holding these objects.
  typedef tbb::concurrent_queue<DOBJ*> queue_t;


  /**
   * @brief Constructor.
   * @param queue Queue on which this object will be entered when it is released.
   * @param args... Additional arguments to pass to the @c DOBJ constructor.
   */
  template <typename... ARGS>
  RecyclableDataObject (std::shared_ptr<queue_t> queue, ARGS&&... args);


  /**
   * @brief DataObject release method.
   *
   * This overrides the @c release() method of @c DataObject.
   * Once the reference count falls back to 1 (meaning that the object is no longer
   * referenced from StoreGate), we recycle the object back to the queue.
   */
  virtual unsigned long release() override;


private:
  /// The queue on which this object will be placed when it is recycled.
  std::shared_ptr<queue_t> m_queue;
};


/**
 * @brief Holder for recyclable objects.
 *
 * See @c RecyclableDataObject.
 */
template <class DOBJ>
class RecyclableDataQueue
{
public:
  /// Underlying queue type holding these objects.
  typedef typename RecyclableDataObject<DOBJ>::queue_t queue_t;


  /** 
   * @brief Constructor.
   */
  RecyclableDataQueue();


  /**
   * @brief Get an object, either a new one or one recycled from a previous event.
   * @param args... Arguments to pass to the @c DOBJ constructor if we make a new one.
   */
  template <typename... ARGS>
  DOBJ* get (ARGS&&... args);


  /**
   * @brief Destructor.
   *
   * Free all objects in the queue.
   */
  ~RecyclableDataQueue();


private:
  /// Shared reference to the actual queue.
  /// Each data object also has a reference to the queue.
  std::shared_ptr<queue_t> m_queue;
};


} // namespace Athena


#include "AthenaKernel/RecyclableDataObject.icc"


#endif // not ATHENAKERNEL_RECYCLABLEDATAOBJECT_H
