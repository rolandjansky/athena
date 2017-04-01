// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: ArenaHeader.h 470529 2011-11-24 23:54:22Z ssnyder $

/**
 * @file  AthAllocators/ArenaHeader.h
 * @author scott snyder
 * @date May 2007
 * @brief Proxy for a group of Arenas.
 *        See Arena.h for an overview of the arena-based memory allocators.
 */


#ifndef ATLALLOCATORS_ARENAHEADER_H
#define ATLALLOCATORS_ARENAHEADER_H


#include "AthAllocators/ArenaBase.h"
#include "GaudiKernel/EventContext.h"
#include <vector>
#include <cstdlib>
#include <string>
#include <iosfwd>
#include <mutex>
#include "boost/thread/tss.hpp"


class EventContext;


namespace SG {


class ArenaAllocatorBase;
class ArenaBase;


/**
 * @brief Proxy for a group of Arenas.
 *        See Arena.h for an overview of the arena-based memory allocators.
 *
 * A Header collects a group of Arenas.  One of these is the current Arena,
 * in which memory operations will take place.  We can also generate
 * a report of memory usage from all the Arenas in the group.
 *
 * We keep a thread-specific pointer to the current Arena, which is
 * used to handle the mapping from indices to Allocator instances.
 */
class ArenaHeader
{
public:
  /**
   * @brief Constructor.
   */
  ArenaHeader();


  /**
   * @brief Destructor.
   *
   * This will clean up any memory allocated in the default Arena.
   */
  ~ArenaHeader();


  /**
   * @brief Translate an integer index to an Allocator pointer.
   * @param i The index to look up.
   *
   * The default Arena will be used.
   *
   * If the index isn't valid, an assertion will be tripped.
   */
  LockedAllocator allocator (size_t i);


  /**
   * @brief Translate an integer index to an Allocator pointer.
   * @param ctx Use the Arena associated with this event context.
   * @param i The index to look up.
   *
   * If the index isn't valid, an assertion will be tripped.
   */
  LockedAllocator allocator (const EventContext& ctx, size_t i);


  /**
   * @brief Set the current Arena for the current thread.
   * @param arena New current Arena.
   * @return The previous Arena.
   */
  ArenaBase* setArena (ArenaBase* arena);


  /**
   * @brief Add a new Arena to the group.
   * @param a The Arena to add.
   */
  void addArena (ArenaBase* a);


  /**
   * @brief Record the arena associated with an event slot.
   * @param slot The slot number.
   * @param a The Arena instance.
   */
  void setArenaForSlot (int slot, ArenaBase* a);


  /**
   * @brief Remove an Arena from the group.
   * @param a The Arena to remove.
   *
   * Will trip an assertion if the Arena is not in the group.
   */
  void delArena (ArenaBase* a);


  /**
   * @brief Generate a report of all Arenas in the group.
   * @param os Stream to which to send a report.
   */
  void report (std::ostream& os) const;


  /**
   * @brief Generate a report of all Arenas in the group, and return
   *        the result as a string.
   *
   * We have this in addition to @c report() in order to make it easier
   * to call from scripting languages.
   */
  std::string reportStr () const;


  /**
   * @brief Call @c reset on all Allocators in the current Arena.
   *
   * All elements allocated are returned to the free state.
   * @c clear should be called on them if it was provided.
   * The elements may continue to be cached internally, without
   * returning to the system.
   */
  void reset();


  /**
   * @brief Return the global default Header instance.
   */
  static ArenaHeader* defaultHeader();


private:
  /// Current Arena.
  boost::thread_specific_ptr<ArenaBase> m_arena;

  /// The default Arena.
  ArenaBase m_defaultArena;

  /// List of all Arenas in our group.
  std::vector<ArenaBase*>  m_arenas;

  /// Arenas indexed by event slot.
  std::vector<ArenaBase*> m_slots;

  /// Mutex to protect access to m_defaultArena, m_arenas, and m_slots.
  mutable std::mutex m_mutex;
};


} // namespace SG


#include "AthAllocators/ArenaHeader.icc"



#endif // not ATLALLOCATORS_ARENAHEADER_H

