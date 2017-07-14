// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CXXUTILS_PAGEACCESSCONTROL_H
#define CXXUTILS_PAGEACCESSCONTROL_H 1

/**
 * @file CxxUtils/PageAccessProtect.h
 * @author Paolo Calafiura
 * @date Jan 2009
 * @brief This class allows to protect a page containing a given address
 *       and to restore back its access protection to what was in
 *       /proc/self/maps
 * NOTE that protectPage will leak memory when asked to write-protect a page
 * 
 * $Id: PageAccessControl.h,v 1.4 2009-03-04 23:10:42 calaf Exp $
 */

#include <vector>
#include <sys/mman.h> /*PROT_NONE*/

#include "CxxUtils/procmaps.h"

class PageAccessControl {
public:
  PageAccessControl(size_t reservedSize=65535) :
    m_pmaps(),
    m_protectedIsSorted(false)
  {
    //we must reserve enough elements, or we risk vector allocating in a protected page during handle()...
    m_protected.reserve(reservedSize);
  }
  PageAccessControl(procmaps& pmaps, size_t reservedSize=65535) :
    m_pmaps(pmaps),
    m_protectedIsSorted(false)
  {
    //we must reserve enough elements, or we risk vector allocating in a protected page during handle()...
    m_protected.reserve(reservedSize);
  }

  ///protect the page containing addr, record the amount of memory we protected
  ///NOTE To avoid SEGV, if PROT_WRITE is requested the remainder of the page 
  /// containing the object at addr will be leaked with malloc before being 
  /// write-locked
  template <typename T>
  bool protectPage(T* addr, int prot) {
    return protectPage(addr, sizeof(T), prot);
  }
  
  ///forbid access to the page containing addr, setting its prot to PROT_NONE
  template <typename T>
  bool forbidPage(const T* addr) {
    return forbidPage(addr, sizeof(T));
  }

  ///forbid access to the page containing addr, setting its prot to PROT_NONE
  bool forbidPage(const void* addr, size_t objSize) {
    return protectPage(addr, objSize, PROT_NONE);
  }
    
  ///FIXME this will not work well for objects spanning across pages
  bool forbidPage(const void* addr) {
    return protectPage(addr, 4, PROT_NONE);
  }
    
  ///void* version of protectPage. Used to implement all others 
  bool protectPage(const void* addr, size_t objSize, int prot);
  
  bool restorePageProt(const void * addr);
  void sort();

  /// @class Entry describes the protection of a memory region (see mprotect(2))
  struct Entry {
    Entry(const void* a, size_t l, int p, void* pl);
    /// address of page for which protection was requested. Used as key
    const void* addr;
    ///lenght of the protected region, from page addr to end of protected obj
    size_t lenProt; 
    int prot;
    ///pointer to the heap fragment we leaked before protecting the page
    void* leak;
    /// # of times this entry protection has been restored. 0 means page was
    /// never accessed
    int restored;
  };

  ///the list of protected pages
  typedef std::vector<Entry> protected_t;
  typedef protected_t::const_iterator const_iterator;
  const protected_t& protectedPtrs() const { return m_protected; }
  const_iterator beginProtectedPtrs() const { return m_protected.begin(); }
  const_iterator endProtectedPtrs() const { return m_protected.end(); }
  void reset() { m_protected.clear(); }

  ///was the page containing address accessed?
  bool accessed(const void* address) const;

private:

  /// the content of /proc/self/maps
  procmaps m_pmaps;

  // can not preallocate map
  //  typedef std::map<void*, size_t, std::less<void*>,
  //		   boost::pool_allocator<std::map<void*, size_t>::value_type > > protected_t;
  protected_t m_protected;
  bool m_protectedIsSorted;
};
inline
bool operator <(const PageAccessControl::Entry& lhs, const PageAccessControl::Entry& rhs) {
  return lhs.addr < rhs.addr;
}
#endif
