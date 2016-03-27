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
#include <algorithm>
#include <ostream>
#include <sstream>
#include <cassert>


namespace SG {


void null_allocvec_deleter (ArenaHeader::ArenaAllocVec_t*) {}


/**
 * @brief Constructor.
 */
ArenaHeader::ArenaHeader()
    // m_allocvec doesn't own the vectors to which it points.
    // Need to pass in a dummy deleter to prevent them from being deleted.
  : m_allocvec (null_allocvec_deleter),
    m_ownedAllocvec (0)
{
}


/**
 * @brief Destructor.
 *
 * This will clean up any memory allocated in the default Arena.
 */
ArenaHeader::~ArenaHeader()
{
  if (m_ownedAllocvec) {
    for (size_t i = 0 ; i < m_ownedAllocvec->size(); i++)
      delete (*m_ownedAllocvec)[i];
    delete m_ownedAllocvec;
  }

  // We don't own this.
  m_allocvec.release();
}


/**
 * @brief Set the current Arena.
 * @param allocvec New vector of Allocator instances.
 * @return The previous vector.
 *
 * This sets the notion of the current Arena.
 */
ArenaHeader::ArenaAllocVec_t*
ArenaHeader::setAllocVec (ArenaAllocVec_t* allocvec)
{
  ArenaAllocVec_t* ret = m_allocvec.get();
  m_allocvec.release();
  m_allocvec.reset (allocvec);
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
  for (size_t i = 0; i < m_arenas.size(); i++) {
    os << "=== " << m_arenas[i]->name() << " ===" << std::endl;
    m_arenas[i]->report (os);
  }

  // The default Arena.
  if (m_ownedAllocvec) {
    os << "=== default ===" << std::endl;
    ArenaAllocatorBase::Stats::header (os);
    os << std::endl;
    for (size_t i = 0; i < m_ownedAllocvec->size(); i++) {
      if ((*m_ownedAllocvec)[i])
        (*m_ownedAllocvec)[i]->report (os);
    }
  }
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
  if (m_allocvec.get()) {
    for (size_t i = 0; i < m_allocvec->size(); i++) {
      if ((*m_allocvec)[i])
        (*m_allocvec)[i]->reset();
    }
  }
}


/**
 * @brief Return the global default Header instance.
 */
ArenaHeader* ArenaHeader::defaultHeader()
{
  static ArenaHeader head;
  return &head;
}


/**
 * @brief Make a new Allocator for index i.
 * @param i The index of the Allocator.
 *
 * The Allocator vector was empty for index @c i.  Make an appropriate
 * new Allocator, store it in the vector, and return it.  Will trip
 * an assertion if the index is not valid.
 */
ArenaAllocatorBase* ArenaHeader::makeAllocator (size_t i)
{
  // If we don't have an Arena set, use the default one.
  if (!m_allocvec.get()) {
    std::lock_guard<std::mutex> lock (m_mutex);

    // Create the default Arena if needed.
    if (!m_ownedAllocvec)
      m_ownedAllocvec = new ArenaAllocVec_t;

    // Install the default Arena.
    m_allocvec.reset (m_ownedAllocvec);

    // See if the index is now in the default Arena.
    if (i < m_allocvec->size()) {
      ArenaAllocatorBase* allocbase = (*m_allocvec)[i];
      if (allocbase) return allocbase;
    }
  }

  // We have to create a new Allocator.
  // Make sure there's room in the vector.
  if (m_allocvec->size() <= i)
    m_allocvec->resize (i+1);

  // Create the Allocator, using the Registry.
  ArenaAllocatorBase* alloc =
    ArenaAllocatorRegistry::instance()->create (i);

  // Install it in the vector.
  (*m_allocvec)[i] = alloc;

  return alloc;
}


} // namespace SG
