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
  : ArenaBase (name),
    m_header (header)
{
  if (!m_header) {
    m_header = SG::ArenaHeader::defaultHeader();
  }
  m_header->addArena (this);
}


/**
 * @brief Destructor.
 */
Arena::~Arena()
{
  m_header->delArena (this);
}


/**
 * @brief Return the @c ArenaHeader with which this @c Arena is associated.
 */
ArenaHeader* Arena::header()
{
  return m_header;
}


/**
 * @brief Make this @c Arena the current one for its @c ArenaHeader.
 * @returns The previously current Arena.
 */
ArenaBase* Arena::makeCurrent()
{
  return m_header->setArena (this);
}


/**
 * @brief Constructor.  Make @c a current.
 * @param a The @c Arena to make current.
 */
Arena::Push::Push (Arena& a)
  : m_header (a.header()),
    m_prev (a.makeCurrent())
{
}


/**
 * @brief Destructor.  Undoes the effect of the constructor.
 */
Arena::Push::~Push()
{
  m_header->setArena (m_prev);
}


} // namespace SG
