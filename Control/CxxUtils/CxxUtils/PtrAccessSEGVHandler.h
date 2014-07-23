// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CXXUTILS_PTRACCESSSEGVHANDLER_H
#define CXXUTILS_PTRACCESSSEGVHANDLER_H 1

/**
 * @file CxxUtils/PtrAccessSEGVHandler.h
 * @author Paolo Calafiura
 * @date Jan 2009
 * @brief This class provides a handle function that logs the  
 *       addresses that had an access violation. It can be installed as sigaction handler using its
 *       C facade in cPtrAccessSIGVHandler.h. The purpose (at least the original one) is to protect
 *       all pages containing heap objects managed by StoreGateSvc, install the handler, and then
 *       see which objects have actually been accessed. 
 *
 * $Id: PtrAccessSEGVHandler.h,v 1.2 2009-02-04 02:02:19 calaf Exp $
 */

#include <signal.h>   /* siginfo_t */
#include <vector>

class PageAccessControl;

class PtrAccessSEGVHandler {
public:
  ///the list of accessed pointers
  typedef std::vector<void*> accessed_t;
  typedef accessed_t::const_iterator const_iterator;
  const accessed_t& accessedPtrs() const { return m_accessed; }
  const_iterator beginAccessedPtrs() const { return m_accessed.begin(); }
  const_iterator endAccessedPtrs() const { return m_accessed.end(); }
  void reset() { m_accessed.clear(); }
  
  PtrAccessSEGVHandler(PageAccessControl& pac, size_t reservedSize=65535) :
    m_pac(pac) 
  {
    //we must reserve enough elements, or we risk vector allocating in a protected page during handle()...
    m_accessed.reserve(reservedSize);
  }

  ///the actual signal handler
  void handle(int signal_number,siginfo_t *sigi,void *unused);

private:
  /// used to restore protection of the page which segfaulted
  PageAccessControl& m_pac;
  /// the addresses accessed since last reset
  accessed_t m_accessed;
};
#endif
