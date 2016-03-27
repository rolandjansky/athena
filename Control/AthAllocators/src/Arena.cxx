/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: Arena.cxx 470529 2011-11-24 23:54:22Z ssnyder $
/**
 * @file  AthAllocators/src/Arena.cxx
 * @author scott snyder
 * @date May 2007
 * @brief Collection of memory allocators with a common lifetime.
 *        Out-of-line implementations.
 */

#include "AthAllocators/Arena.h"
#include "AthAllocators/ArenaAllocatorBase.h"

namespace SG {


/**
 * @brief Constructor.
 * @param name The name of this @c Arena; to use in reports.
 * @param header The header with which this @c Arena is associated.
 *        If defaulted, the global default @c ArenaHeader will be used.
 */
Arena::Arena (const std::string& name, ArenaHeader* header /*= 0*/)
  : m_header (header),
    m_name (name)
{
  if (!m_header)
    m_header = SG::ArenaHeader::defaultHeader();
  m_header->addArena (this);
}


/**
 * @brief Destructor.
 */
Arena::~Arena()
{
  m_header->delArena (this);
  for (size_t i = 0; i < m_allocs.size(); i++)
    delete m_allocs[i];
}


/**
 * @brief reset all contained allocators.  All elements will be freed.
 */
void Arena::reset()
{
  for (size_t i = 0; i < m_allocs.size(); i++) {
    if (m_allocs[i])
      m_allocs[i]->reset();
  }
}


/**
 * @brief erase all contained allocators.  All elements will be freed,
 *        and the memory returned to the system.
 */
void Arena::erase()
{
  for (size_t i = 0; i < m_allocs.size(); i++) {
    if (m_allocs[i])
      m_allocs[i]->erase();
  }
}


/**
 * @brief Generate a report of the memory in use by this @c Arena.
 * @param os The stream to which to write the report.
 */
void Arena::report (std::ostream& os) const
{
  bool first = true;
  for (size_t i = 0; i < m_allocs.size(); i++) {
    if (m_allocs[i]) {
      if (first) {
        ArenaAllocatorBase::Stats::header (os);
        os << std::endl;
        first = false;
      }
      m_allocs[i]->report (os);
    }
  }
}


/**
 * @brief Return statistics summed over all allocators in this @c Arena.
 */
const ArenaAllocatorBase::Stats& Arena::stats () const
{
  m_stats = ArenaAllocatorBase::Stats();
  for (size_t i = 0; i < m_allocs.size(); i++) {
    if (m_allocs[i]) {
      m_stats += m_allocs[i]->stats();
    }
  }
  return m_stats;
}


/**
 * @brief Return the @c ArenaHeader with which this @c Arena is associated.
 */
ArenaHeader* Arena::header() const
{
  return m_header;
}


/**
 * @brief Return this @c Arena's name.
 */
const std::string& Arena::name() const
{
  return m_name;
}


/**
 * @brief Make this @c Arena the current one for its @c ArenaHeader.
 * @returns The previously current allocator vector.
 */
ArenaHeader::ArenaAllocVec_t* Arena::makeCurrent()
{
  return m_header->setAllocVec (&m_allocs);
}


/**
 * @brief Constructor.  Make @c a current.
 * @param a The @c Arena to make current.
 */
Arena::Push::Push (Arena& a)
  : m_header (a.header()),
    m_allocs (a.makeCurrent())
{
}


/**
 * @brief Destructor.  Undoes the effect of the constructor.
 */
Arena::Push::~Push()
{
  m_header->setAllocVec (m_allocs);
}


} // namespace SG
