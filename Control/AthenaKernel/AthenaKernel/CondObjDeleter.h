// This file's extension implies that it's C, but it's really -*- C++ -*-.
/*
 * Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration.
 */
/**
 * @file AthenaKernel/CondObjDeleter.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Jun, 2022
 * @brief Deletion object for conditions payloads.
 */


#ifndef ATHENAKERNEL_CONDOBJDELETER_H
#define ATHENAKERNEL_CONDOBJDELETER_H


#include "AthenaKernel/RCUObject.h"
#include "CxxUtils/ConcurrentRangeMap.h"
#include "GaudiKernel/EventContext.h"
#include <mutex>


namespace Athena {


/**
 * @brief Deletion object for conditions payloads.
 *
 * This class implements the @c IRangeMapPayloadDeleter interface
 * for conditions objects.
 *
 * The underlying @c ConcurrentRangeMap objects hold the payload objects
 * as void*'s, so that's the type we use in the @c IRangeMapPayloadDeleter
 * template.  The parameter @c T is the actual type of the payload objects.
 *
 * We use @c RCUObject to manage the lifetime of the deleted objects.
 */
template <class T>
class CondObjDeleter
  : public CxxUtils::IRangeMapPayloadDeleter<void, EventContext>
{
public:
  /**
   * @brief Constructor.
   * @param rcusvc Service with which to register.
   */
  CondObjDeleter (Athena::IRCUSvc& rcusvc);


  /**
   * @brief Queue an object for deletion.
   * @param p The object to delete.
   *
   * The object should not actually be deleted until @c quiescent has been
   * called for each active slot.
   */
  virtual void discard (const void* p) override;


  /**
   * @brief Mark a slot as quiescent.
   * @param ctx Event context identifying the slot.
   */
  virtual void quiescent (const EventContext& ctx) override;


private:
  /**
   * @brief Function to delete a payload object immediately.
   */
  static void delPayload (const void* p);

  /// Lifetime manager for discarded objects.
  Athena::RCUObject<T> m_obj;
};


} // namespace Athena


#include "AthenaKernel/CondObjDeleter.icc"


#endif // not ATHENAKERNEL_CONDOBJDELETER_H
