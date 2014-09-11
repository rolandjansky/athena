///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// MallocStats.cxx 
// Implementation file for class MallocStats
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 

// PerfMonEvent includes
#include "PerfMonEvent/MallocStats.h"

// STL includes
#include <sstream>

namespace PerfMon {

/////////////////////////////////////////////////////////////////// 
// Public methods: 
/////////////////////////////////////////////////////////////////// 

// Constructors
////////////////
MallocStats::MallocStats() :
  m_infos (mallinfo())
{}

// Destructor
///////////////

/////////////////////////////////////////////////////////////////// 
// Const methods: 
///////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////// 
// Non-const methods: 
/////////////////////////////////////////////////////////////////// 

/////////////////////////////////////////////////////////////////// 
// Protected methods: 
/////////////////////////////////////////////////////////////////// 

/** display malloc stats
 */
std::string MallocStats::repr()
{
  std::ostringstream s;
  s << "MALLOC statistics\n"
    << "=================================================================\n"
    << "Space allocated from system not using mmap: " 
    << from_system_nmmap() << " bytes\n"
    << "       number of free chunks                       : " << free_chunks() 
    << "\n"
    << "       space allocated and in use                  : " << used() 
    << " bytes\n"
    << "       space allocated but not in use              : " << not_used()
    << " bytes\n"
    << "       top-most, releasable (via malloc_trim) space: " << releasable()
    << " bytes\n"
    << "       maximum total allocated space (?)           : " << max_allocated() 
    << " bytes\n"
    << "   FASTBIN blocks \n"
    << "       number of fastbin blocks: " << fastbin_blocks() << "\n"
    << "       space available in freed fastbin blocks: " << fastbin_free()
    << " bytes\n"
    << "Space allocated from system using mmap: " << from_system_mmap() 
    << " bytes\n"
    << "       number of chunks allocated via mmap(): " << mmap_chunks() 
    << "\n"
    << "Total space allocated from system (mmap and not mmap): " 
    << from_system_total() << " bytes\n"
    << "=================================================================\n"
    << std::flush;
  return s.str();
}

void
MallocStats::dump (std::ostream& out)
{
  out << repr();
}


} //> end namespace PerfMon

