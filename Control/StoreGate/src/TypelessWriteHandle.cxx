/*
 * Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration.
 */
/**
 * @file StoreGate/src/TypelessWriteHandle.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date May, 2019
 * @brief Handle allowing writes to the event store with a configurable type.
 */


#include "StoreGate/TypelessWriteHandle.h"


namespace SG {


/**
 * @brief Constructor from a TypelessWriteHandleKey and an explicit event context.
 * @param key The key object holding the clid/key.
 * @param ctx The event context.
 *
 * This will raise an exception if the StoreGate key is blank,
 * or if the event store cannot be found.
 *
 * If the default event store has been requested, then the thread-specific
 * store from the event context will be used.
 */
TypelessWriteHandle::TypelessWriteHandle (const TypelessWriteHandleKey& key,
                                          const EventContext& ctx)
  : VarHandleBase (key, &ctx)
{
}


/**
 * @brief Can the handle be successfully dereferenced?
 *
 * This will always return false.
 */
bool TypelessWriteHandle::isValid()
{
  return false;
}


/**
 * @brief Record an object in the store.
 * @param data The object to record.
 *
 * @c data will usually be a @c DataBucket object.
 * Will fail if there is already an object present with this key.
 */
StatusCode
TypelessWriteHandle::typelessPut (std::unique_ptr<DataObject> data) const
{
  IProxyDict* store = nullptr;
  const void* dataPtr = data.get();
  const void* ptr = put_impl (nullptr, // ctx
                              std::move (data),
                              dataPtr, // dataPtr
                              false,   // allowMods
                              false,   // returnExisting,
                              store);
  if (ptr != nullptr) {
    return StatusCode::SUCCESS;
  }
  return StatusCode::FAILURE;
}


} // namespace SG
