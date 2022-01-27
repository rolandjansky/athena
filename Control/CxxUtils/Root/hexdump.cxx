/*
 * Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration.
 */
/**
 * @file CxxUtils/src/hexdump.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Apr, 2021
 * @brief Helpers to make a nice dump of a region of memory.
 */


#include "CxxUtils/hexdump.h"
#include "CxxUtils/procmaps.h"
#include "boost/io/ios_state.hpp"
#include <iomanip>
#include <unistd.h>


namespace {
// Number of bytes to dump on one line.
constexpr unsigned int width = 16;
}


namespace CxxUtils {


/**
 * @brief Make a hex dump of memory.
 * @param s Stream to which to write the output.
 * @param addr Address at which to start dumping.
 * @param n Number of byte to dump.
 * @param offset Offset by which to shift the printed address
 *               (mostly for testing).
 */
void hexdump (std::ostream& s, const void* addr, size_t n, size_t offset /*= 0*/)
{
  const char* ptr = reinterpret_cast<const char*> (addr);
  size_t ipos = 0;

  boost::io::ios_all_saver saver (s);
  std::hex (s);
  s.fill ('0');

  char cbuf[width + 1] = {0};
  union {
    uint32_t u32;
    unsigned char uc[4];
  } bbuf;

  while (n-- > 0) {
    if ((ipos % width) == 0) {
      s << std::setw(16) << reinterpret_cast<uintptr_t>(ptr + ipos) - offset << " ";
    }
    if ((ipos % 4) == 0) {
      s << " ";
    }
    bbuf.uc[ipos % 4] = ptr[ipos];
    cbuf[ipos % width] = std::isgraph (ptr[ipos]) ? ptr[ipos] : '.';

    ++ipos;
    if ((ipos % 4) == 0) {
      s << std::setw(8) << static_cast<unsigned int>(bbuf.u32);
    }
    if ((ipos % width) == 0) {
      s << "  " << cbuf << "\n";
    }
  }

  if ((ipos % width) > 0) {
    unsigned ntrail = (ipos % 4);
    if (ntrail > 0) {
      for (unsigned i = ntrail; i < 4; i++) {
        bbuf.uc[i] = 0;
      }
      s << std::setw(2*ntrail) << static_cast<unsigned int>(bbuf.u32);
    }
    while ((ipos % width) != 0) {
      if ((ipos % 4) == 0) {
        s << " ";
      }
      s << "  ";
      cbuf[ipos % width] = ' ';
      ++ipos;
    }
    s << "  " << cbuf << "\n";
  }
}


/**
 * @brief Make a hex dump of memory, protected against bad reads.
 * @param s Stream to which to write the output.
 * @param addr Address at which to start dumping.
 * @param n Number of byte to dump.
 * @param offset Offset by which to shift the printed address
 *               (mostly for testing).
 *
 * This function will skip dumping memory pages that are not readable.
 * It may also start dumping slightly before the requested address,
 * in order to start with an alignment of 16.
 */
void safeHexdump (std::ostream& s, const void* addr, size_t n, size_t offset /*= 0*/)
{
  const char* ptr = reinterpret_cast<const char*> (addr);

  // Adjust to start at width-byte boundary.
  size_t nadj = reinterpret_cast<uintptr_t>(ptr) % width;
  if (nadj > 0) {
    ptr -= nadj;
    n += nadj;
  }

  size_t pagesize = sysconf (_SC_PAGESIZE);

  procmaps m;

  // Print page by page.
  while (n > 0) {
    uintptr_t iptr = reinterpret_cast<uintptr_t>(ptr);
    size_t thispage = ((iptr + pagesize) & ~(pagesize-1)) - iptr;
    if (thispage > n) {
      thispage = n;
    }
    const procmaps::Entry* ent = m.getEntry (ptr);
    if (ent && ent->readable) {
      hexdump (s, ptr, thispage, offset);
    }
    else {
      boost::io::ios_all_saver saver (s);
      std::hex (s);
      s.fill ('0');
      s << std::setw(16) << reinterpret_cast<uintptr_t>(ptr) - offset
        << "  --- is not readable\n";
      if (ent) {
        thispage = std::max (ent->endAddress - iptr, thispage);
      }
    }
    ptr += thispage;
    n -= thispage;
  }
}


} // namespace CxxUtils
