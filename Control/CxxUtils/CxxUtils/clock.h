// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file CxxUtils/clock.h
 * @author David Quarrie <David.Quarrie@cern.ch>
 * @date Jan 2010, from earlier code.
 * @brief Provide simplified clock_gettime() function for MacOSX.
 */


#ifndef CXXUTILS_CLOCK_H
#define CXXUTILS_CLOCK_H

#if defined (__APPLE__) && __clang_major__ < 12
#include <time.h>
#define CLOCK_REALTIME           0
#define CLOCK_MONOTONIC          1
#define CLOCK_PROCESS_CPUTIME_ID 2
#define CLOCK_THREAD_CPUTIME_ID	 3

long clock_gettime (unsigned int which_clock, struct timespec *tp);
#endif

#endif // not CXXUTILS_CLOCK_H
