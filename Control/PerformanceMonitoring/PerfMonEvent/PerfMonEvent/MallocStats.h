///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

// MallocStats.h 
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 
#ifndef PERFMONEVENT_PERFMONMALLOCSTATS_H 
#define PERFMONEVENT_PERFMONMALLOCSTATS_H 1

// the following code has been reaped off the STXXL library (boost licence)
// see http://stxxl.sourceforge.net/ for licence

#include <iostream>
#include "PerfMonEvent/mallinfo.h"
#include <cstdlib>
#include <string>

namespace PerfMon {

  //! \brief Access to some useful malloc statistics

  //! malloc is default C++ allocator
class MallocStats
{
  PerfMon::mallinfo_t m_infos;

public:

  /** c-tor
   */
  MallocStats();

  typedef int return_type;

  /** update cached informations about ,alloc statistics
   */
  void refresh()
  { m_infos = PerfMon::mallinfo(); }

  //! \brief Returns number of bytes allocated from system not including mmapped regions
  return_type from_system_nmmap()
  {
    refresh();
    return m_infos.arena;
  }

  //! \brief Returns number of free chunks
  return_type free_chunks()
  {
    refresh();
    return m_infos.ordblks;
  }

  //! \brief Number of bytes allocated and in use
  return_type used()
  {
    refresh();
    return m_infos.uordblks;
  }

  //! \brief Number of bytes allocated but not in use
  return_type not_used()
  {
    refresh();
    return m_infos.fordblks;
  }

  //! \brief Top-most, releasable (via malloc_trim) space (bytes)
  return_type releasable()
  {
    refresh();
    return m_infos.keepcost;
  }

  //! \brief Maximum total allocated space (bytes) (always 0 ?)
  return_type max_allocated()
  {
    refresh();
    return m_infos.usmblks;
  }

  //! \brief Number of fastbin blocks
  return_type fastbin_blocks()
  {
    refresh();
    return m_infos.smblks;
  }

  //! \brief Space available in freed fastbin blocks (bytes)
  return_type fastbin_free()
  {
    refresh();
    return m_infos.fsmblks;
  }

  //! \brief Returns number of bytes allocated from system using mmap
  return_type from_system_mmap()
  {
    refresh();
    return m_infos.hblkhd;
  }

  //! \brief Number of chunks allocated via mmap()
  return_type mmap_chunks()
  {
    refresh();
    return m_infos.hblks;
  }

  /** Returns \b total number of bytes allocated from system 
   *  including mmapped regions
   */
  return_type from_system_total()
  {
    refresh();
    return from_system_nmmap() + from_system_mmap();
  }

  /** display malloc stats
   */
  std::string repr();

  /** display malloc stats on std::cout
   */
  void dump (std::ostream& out = std::cout);
};

} //> end namespace PerfMon

#endif // !PERFMONEVENT_PERFMONMALLOCSTATS_H
