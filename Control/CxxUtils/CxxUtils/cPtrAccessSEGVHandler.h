// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CXXUTILS_CPTRACCESSSEGVHANDLER_H
#define CXXUTILS_CPTRACCESSSEGVHANDLER_H 1

/**
 * @file CxxUtils/cPtrAccessSEGVHandler.h
 * @author Paolo Calafiura
 * @date Jan 2009
 * @brief a C wrapper providing access to PtrAccessSEGVHandler::handle the way sigaction wants it
 *    Example:
 *  PtrAccessSEGVHandler h(p);
 *  setPtrAccessSEGVHandler(h);
 *  struct sigaction sa;
 *  sa.sa_sigaction= cPtrAccessSEGVHandler; 
 *  sigaction(SIGSEGV,&sa, NULL);
 *
 * $Id: cPtrAccessSEGVHandler.h,v 1.1 2009-01-30 00:50:51 calaf Exp $
 */

#include "CxxUtils/checker_macros.h"
#include <signal.h>   /*siginfo_t*/
class PtrAccessSEGVHandler;

void setPtrAccessSEGVHandler ATLAS_NOT_THREAD_SAFE (PtrAccessSEGVHandler* h);
void cPtrAccessSEGVHandler ATLAS_NOT_THREAD_SAFE (int signal, siginfo_t* si, void* old);
#endif
