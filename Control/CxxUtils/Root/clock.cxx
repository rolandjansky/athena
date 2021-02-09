/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file CxxUtils/src/clock.cxx
 * @author David Quarrie <David.Quarrie@cern.ch>
 * @date Jan, 2010
 * @brief Implementation of clock_gettime() function for MacOSX
 */

#include "CxxUtils/clock.h"

#if defined (__APPLE__) && __clang_major__ < 12
#include <stdint.h>
#include <mach/mach_time.h> 

long clock_gettime (unsigned int /*which_clock*/, struct timespec *tp) 
{
	uint64_t time = 0;
    mach_timebase_info_data_t info;
    kern_return_t err = mach_timebase_info( &info );
        
	//Convert the timebase into seconds
    if( err == 0  ) {
		uint64_t mach_time = mach_absolute_time( );
		time = mach_time * info.numer / info.denom; 
	}
	tp->tv_sec  = time * 1e-9;
	tp->tv_nsec = time - (tp->tv_sec * 1e9);
	return 0;
}
#endif
