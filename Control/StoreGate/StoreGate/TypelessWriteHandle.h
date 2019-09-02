// This file's extension implies that it's C, but it's really -*- C++ -*-.
/*
 * Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration.
 */
/**
 * @file StoreGate/TypelessWriteHandle.h
 * @author scott snyder <snyder@bnl.gov>
 * @date May, 2019
 * @brief Handle allowing writes to the event store with a configurable type.
 */


#ifndef STOREGATE_TYPELESSWRITEHANDLE_H
#define STOREGATE_TYPELESSWRITEHANDLE_H


#include "StoreGate/TypelessWriteHandleKey.h"
#include "StoreGate/VarHandleBase.h"


namespace SG {


/**
 * @brief Handle allowing writes to the event store with a configurable type.
 *
 * This is like @c WriteHandle, except that the type to be written is
 * configurable as a job property.
 * Accordingly, most of the functionality of @c WriteHandle is not
 * supported.  One records to the store with a put-like operation
 * that is passed a @c DataObject.
 */
class TypelessWriteHandle
  : public SG::VarHandleBase
{
public:
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
  explicit TypelessWriteHandle (const TypelessWriteHandleKey& key,
                                const EventContext& ctx);

  
  /**
   * @brief Can the handle be successfully dereferenced?
   *
   * This will always return false.
   */
  virtual bool isValid() override final;


  /**
   * @brief Record an object in the store.
   * @param data The object to record.
   *
   * @c data will usually be a @c DataBucket object.
   * Will fail if there is already an object present with this key.
   */
  StatusCode typelessPut (std::unique_ptr<DataObject> data) const;
};


} // namespace SG


#endif // not STOREGATE_TYPELESSWRITEHANDLE_H
