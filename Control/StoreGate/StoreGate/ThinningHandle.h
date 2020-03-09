// This file's extension implies that it's C, but it's really -*- C++ -*-.
/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/
/**
 * @file StoreGate/ThinningHandle.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Sep, 2019
 * @brief Handle for requesting thinning for a data object.
 */


#ifndef STOREGATE_THINNINGHANDLE_H
#define STOREGATE_THINNINGHANDLE_H


#include "StoreGate/ThinningHandleKey.h"
#include "StoreGate/ThinningHandleBase.h"
#include "StoreGate/ReadHandle.h"
#include "GaudiKernel/ThreadLocalContext.h"


namespace SG {


/**
 * @brief Handle for requesting thinning for a data object.
 * 
 * A @c ThinningHandle + @c ThinningHandleKey is used to request thinning
 * for a data object for a particular stream.
 *
 * To use it, you should first create a @c ThinningHandleKey referencing the
 * object you want to thin, in the same way as for a @c ReadHandle:
 *
 *@code
 *  SG::ThinningHandleKey<MyObjectContainer> m_objkey
 *  { this, "ObjKey", "obj", "Object to be thinned." };
 @endcode
 *
 * When you initialize the key, you should pass it the the name of the stream
 * for which thinning is being done:
 *
 *@code
 *  StringProperty m_stream{ this, "Stream", "OutStream", "Stream name for thinning" };
 *  ...
 *  ATH_CHECK( m_objkey.initialize (m_streamName);
 @endcode
 *
 * Then you can create a @c ThinningHandle from the key in the way in which
 * you would create a @c ReadHandle:
 *
 *@code
 *  SG::ThinningHandle<MyObjectContainer> h (m_objkey, ctx);
 @endcode
 *
 * This handle can then be used to access the object in the same way in which
 * you would use a @c ReadHandle.  In addition, you can call methods
 * on the handle to specify objects to thin:
 *
 *@code
 *  size_t iobj = 0;
 *  for (const MyObject* obj : *h) {
 *    if (obj->pt() < m_thresh) {
 *      h.thin (iobj);
 *    }
 *    ++iobj;
 *  }
 @endcode
 *
 * See @c ThinningHandleBase for a full list of the methods that may be used
 * to specify the thinning.
 *
 * The thinning request will be committed when the @c ThinningHandle is destroyed.
 *
 * If the same object is thinned for the same stream in multiple algorithms,
 * then the result will be the AND of all of them; that is, an element will be
 * thinned on the output only if it has been requested to be thinned by all algorithms.
 *
 * For a complete example, see DataModelTestDataCommon/xAODTestThinCVec.
 */
template <class T>
class ThinningHandle
  : public ReadHandle<T>, public ThinningHandleBase
{
public:
  using Op = ThinningDecision::Op;


  /** 
   * @brief Constructor from a ThinningHandleKey.
   * @param key The key object holding the clid/key/store.
   *
   * This will raise an exception if the StoreGate key is blank,
   * or if the event store cannot be found.
   */
  explicit ThinningHandle (const ThinningHandleKey<T>& key)
    : ThinningHandle (key, Gaudi::Hive::currentContext())
  {
  }


  /** 
   * @brief Constructor from a ThinningHandleKey and an explicit event context.
   * @param key The key object holding the clid/key/store.
   * @param ctx The event context.
   *
   * This will raise an exception if the StoreGate key is blank,
   * or if the event store cannot be found.
   *
   * If the default event store has been requested, then the thread-specific
   * store from the event context will be used.
   */
  explicit ThinningHandle (const ThinningHandleKey<T>& key,
                           const EventContext& ctx)
    : ReadHandle<T> (key, ctx),
      ThinningHandleBase (key.decisionHandleKey(), key.key(), ctx)
  {
  }
};


/**
 * @brief Return a @c ThinningHandle referencing @c key.
 * @param key The key object holding the clid/key/store.
 *
 * This will raise an exception if the StoreGate key is blank,
 * or if the event store cannot be found.
 */
template <class T>
ThinningHandle<T> makeHandle (const ThinningHandleKey<T>& key)
{
  return ThinningHandle<T> (key);
}


/**
 * @brief Return a @c ThinningHandle referencing @c key for an explicit context.
 * @param key The key object holding the clid/key/store.
 * @param ctx The event context.
 *
 * This will raise an exception if the StoreGate key is blank,
 * or if the event store cannot be found.
 *
 * If the default event store has been requested, then the thread-specific
 * store from the event context will be used.
 */
template <class T>
ThinningHandle<T> makeHandle (const ThinningHandleKey<T>& key,
                              const EventContext& ctx)
{
  return ThinningHandle<T> (key, ctx);
}


} // namespace SG


#endif // not STOREGATE_THINNINGHANDLE_H
