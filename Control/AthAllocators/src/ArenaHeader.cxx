/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: ArenaHeader.cxx 470529 2011-11-24 23:54:22Z ssnyder $
/**
 * @file  AthAllocators/src/ArenaHeader.cxx
 * @author scott snyder
 * @date May 2007
 * @brief Proxy for a group of Arenas.
 *        Out-of-line implementations.
 */

#include "AthAllocators/ArenaHeader.h"
#include "AthAllocators/ArenaAllocatorRegistry.h"
#include "AthAllocators/ArenaAllocatorBase.h"
#include "AthAllocators/ArenaBase.h"
#include "CxxUtils/checker_macros.h"
#include <algorithm>
#include <ostream>
#include <sstream>
#include <cassert>


namespace SG {


void null_arena_deleter (ArenaBase*) {}


/**
 * @brief Constructor.
 */
ArenaHeader::ArenaHeader()
    // m_arena doesn't own the objects to which it points.
    // Need to pass in a dummy deleter to prevent them from being deleted.
  : m_arena (null_arena_deleter)
{
}


/**
 * @brief Destructor.
 *
 * This will clean up any memory allocated in the default Arena.
 */
ArenaHeader::~ArenaHeader()
{
  // We don't own this.
  m_arena.release();
}


/**
 * @brief Set the current Arena for the current thread.
 * @param arena New current Arena.
 * @return The previous Arena.
 */
ArenaBase* ArenaHeader::setArena (ArenaBase* arena)
{
  ArenaBase* ret = m_arena.get();
  m_arena.release();
  m_arena.reset (arena);
  return ret;
}


/**
 * @brief Add a new Arena to the group.
 * @param a The Arena to add.
 */
void ArenaHeader::addArena (ArenaBase* a)
{
  std::lock_guard<std::mutex> lock (m_mutex);
  m_arenas.push_back (a);
}


/**
 * @brief Record the arena associated with an event slot.
 * @param slot The slot number.
 * @param a The Arena instance.
 */
void ArenaHeader::setArenaForSlot (int slot, ArenaBase* a)
{
  if (slot < 0) return;
  std::lock_guard<std::mutex> lock (m_mutex);
  if (slot >= static_cast<int> (m_slots.size())) {
    m_slots.resize (slot+1);
  }
  m_slots[slot] = a;
}


/**
 * @brief Remove an Arena from the group.
 * @param a The Arena to remove.
 *
 * Will trip an assertion if the Arena is not in the group.
 */
void ArenaHeader::delArena (ArenaBase* a)
{
  std::lock_guard<std::mutex> lock (m_mutex);
  std::vector<ArenaBase*>::iterator it =
    std::find (m_arenas.begin(), m_arenas.end(), a);
  assert (it != m_arenas.end());
  m_arenas.erase (it);
}


/**
 * @brief Generate a report of all Arenas in the group.
 * @param os Stream to which to send a report.
 */
void ArenaHeader::report (std::ostream& os) const
{
  std::lock_guard<std::mutex> lock (m_mutex);
  // All Allocators in the group.
  for (ArenaBase* arena : m_arenas) {
    os << "=== " << arena->name() << " ===" << std::endl;
    arena->report (os);
  }

  // The default Arena.
  os << "=== default ===" << std::endl;
  m_defaultArena.report (os);
}


/**
 * @brief Generate a report of all Arenas in the group, and return
 *        the result as a string.
 *
 * We have this in addition to @c report() in order to make it easier
 * to call from scripting languages.
 */
std::string ArenaHeader::reportStr() const
{
  std::ostringstream s;
  report (s);
  return s.str();
}


/**
 * @brief Call @c reset on all Allocators in the current Arena.
 *
 * All elements allocated are returned to the free state.
 * @c clear should be called on them if it was provided.
 * The elements may continue to be cached internally, without
 * returning to the system.
 */
void ArenaHeader::reset()
{
  if (m_arena.get()) {
    m_arena->reset();
  }
}


/**
 * @brief Return the global default Header instance.
 */
ArenaHeader* ArenaHeader::defaultHeader()
{
  static ArenaHeader head ATLAS_THREAD_SAFE;
  return &head;
}


} // namespace SG
