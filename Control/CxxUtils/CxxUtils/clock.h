// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file CxxUtils/clock.h
 * @author David Quarrie <David.Quarrie@cern.ch>
 * @date Jan 2010, from earlier code.
 * @brief Provide simplified clock_gettime() function for MacOSX.
 */


#ifndef CXXUTILS_CLOCK_H
#define CXXUTILS_CLOCK_H

#ifdef __APPLE__

#include <time.h>

#ifndef CLOCK_REALTIME
#   define CLOCK_REALTIME           0
#endif // not CLOCK_REALTIME
#ifndef CLOCK_MONOTONIC
#   define CLOCK_MONOTONIC          1
#endif // not CLOCK_MONOTONIC
#ifndef CLOCK_PROCESS_CPUTIME_ID
#   define CLOCK_PROCESS_CPUTIME_ID 2
#endif // not CLOCK_PROCESS_CPUTIME_ID
#ifndef CLOCK_THREAD_CPUTIME_ID
#   define CLOCK_THREAD_CPUTIME_ID  3
#endif // not CLOCK_THREAD_CPUTIME_ID

long clock_gettime (unsigned int which_clock, struct timespec *tp);
#endif

#endif // not CXXUTILS_CLOCK_H
