// This file's extension implies that it's C, but it's really -*- C++ -*-.
/*
 * Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration.
 */
// $Id$
/**
 * @file AthenaKernel/SlotSpecificObj.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Jul, 2017
 * @brief Maintain a set of objects, one per slot.
 */


#ifndef ATHENAKERNEL_SLOTSPECIFICOBJ_H
#define ATHENAKERNEL_SLOTSPECIFICOBJ_H


#include "CxxUtils/checker_macros.h"
#include "GaudiKernel/ThreadLocalContext.h"
#include "GaudiKernel/EventContext.h"
#include <vector>
#include <string>


namespace SG {


/**
 * @brief Return the number of event slots.
 */
size_t getNSlots();


/**
 * @brief Allow setting the name of the whiteboard service.
 *
 * For testing.
 */
void setNSlotsHiveMgrName ATLAS_NOT_THREAD_SAFE (const std::string& s);


/**
 * @brief Maintain a set of objects, one per slot.
 *
 * The payload @c T must be default-constructable.  One such instance of @c T
 * will be made for each event slot.
 *
 * The usual pointer operators may be used to access the object, as well as @c get().
 * @c get() also takes an optional event context argument, to allow specifying
 * the slot explicitly, rather than looking it up in a thread-local global.
 *
 * This class does not do anything itself to protect the contents
 * of the payload objects against simultaneous access from different threads.
 * Especially if the owner is a service or a reentrant algorithm,
 * multiple threads may be executing  simultaneously for the same slot.
 * In such cases, @c T must be safe against simultaneous access from
 * multiple threads, possibly with an internal lock or using std::atomic.
 *
 * This is similiar to @c ContextSpecificPtr from Gaudi, but with some
 * important differences:
 *
 *   - The payload objects are managed and owned by this class.
 *   - Rather than using an unordered_map with locking, we just use a vector,
 *     indexed by slot.  We thus don't need any locking.  The price of this
 *     is that we need to know the number of slots when we're constructed.
 *   - We allow requesting a given slot explicitly, via an @c EventContext argument.
 */
template <class T>
class SlotSpecificObj
{
public:
  /**
   * @brief Constructor.
   *
   * The number of slots will be found by calling @c getNSlots().
   */
  SlotSpecificObj();

  
  /**
   * @brief Constructor, with number of slots specified explicitly.
   * @param nslots The number of event slots.
   */
  SlotSpecificObj (size_t nslots);


  /**
   * @brief Return pointer to the object for slot given by @c ctx.
   * @param ctx Event context giving the desired slot.
   */
  T* get (const EventContext& ctx);


  /**
   * @brief Return pointer to the object for the current slot.
   *
   * The slot number is found by retrieving the global current context.
   */
  T* get();


  /**
   * @brief Return pointer to the object for slot given by @c ctx.
   * @param ctx Event context giving the desired slot.
   */
  const T* get (const EventContext& ctx) const;


  /**
   * @brief Return pointer to the object for the current slot.
   *
   * The slot number is found by retrieving the global current context.
   */
  const T* get() const;


  /**
   * @brief Dereference the pointer.
   *
   * The slot number is found by retrieving the global current context.
   */
  T& operator* ();


  /**
   * @brief Dereference the pointer.
   *
   * The slot number is found by retrieving the global current context.
   */
  const T& operator* () const;


  /**
   * @brief Dereference the pointer.
   *
   * The slot number is found by retrieving the global current context.
   */
  T* operator-> ();


  /**
   * @brief Dereference the pointer.
   *
   * The slot number is found by retrieving the global current context.
   */
  const T* operator-> () const;


  //**********************************************************************
  // Allow iterating over slots.
  // Remember that any needed locking of the payload objects
  // is the responsibility of the caller.

  typedef typename std::vector<T>::iterator iterator;
  typedef typename std::vector<T>::const_iterator const_iterator;
  typedef typename std::vector<T>::value_type value_type;


  /**
   * @brief Begin iterator.
   */
  iterator begin();


  /**
   * @brief Const begin iterator.
   */
  const_iterator begin() const;


  /**
   * @brief End iterator.
   */
  iterator end();


  /**
   * @brief Const end iterator.
   */
  const_iterator end() const;

  
private:
  /// Set of per-slot objects.
  std::vector<T> m_slots;
};


} // namespace SG


#include "AthenaKernel/SlotSpecificObj.icc"


#endif // not ATHENAKERNEL_SLOTSPECIFICOBJ_H
