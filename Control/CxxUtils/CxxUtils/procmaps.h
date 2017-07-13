// dear emacs, this is -*- C++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PROCMAPS_H 
#define PROCMAPS_H 1
#include <vector>
/** @class procmaps 
 * @brief A simple API to access /proc/self/maps info
 *
 * @author Paolo Calafiura - LBNL, ATLAS Collaboration
 * $Id: procmaps.h,v 1.3 2009-04-01 08:23:32 binet Exp $
 **/
class procmaps {
public:
  struct Entry {
    Entry(const char* line);
    unsigned long begAddress;
    unsigned long endAddress;
    bool readable;
    bool writable;
    bool executable;
    bool isPrivate; ///=true page is private(COW), =false page is shared
    unsigned int offset;
    unsigned int dev[2]; /// dev[0] major, dev[1] minor
    unsigned int inode;
    char pathname[32];  ///truncated if needed
  };

  procmaps(size_t entries=1024);
  //not yet  procmaps(int pid);

  typedef std::vector<Entry> procmaps_t;
  typedef procmaps_t::const_iterator const_iterator;
  
  ///main entry point: get info for the page range containing address
  /// by default tries to reload /proc/self/maps when address is not found
  const Entry* getEntry(const void* address, bool tryReloadMaps=true);

  ///access underlying entries
  const procmaps_t& pmaps() const { return m_pmaps; }
  const_iterator begin() const { return pmaps().begin(); }
  const_iterator end() const { return pmaps().end(); }

  /// load/refresh info from /proc/self/map
  void loadMaps(bool dump=false); 

private:
  procmaps_t m_pmaps;
};

inline
bool operator<(procmaps::Entry& lhs, procmaps::Entry& rhs){
  return lhs.begAddress < rhs.begAddress;
}
#endif
