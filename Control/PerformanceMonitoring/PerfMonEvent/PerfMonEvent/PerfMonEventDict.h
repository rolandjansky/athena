// this is -*- C++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PERFMONEVENT_PERFMONEVENTDICT_H
#define PERFMONEVENT_PERFMONEVENTDICT_H

#include "PerfMonEvent/PyStore.h"
#include "PerfMonEvent/PyChrono.h"
#include "PerfMonEvent/DataModel.h"
#include "PerfMonEvent/MemStatsHooks.h"
#include "PerfMonEvent/MallocStats.h"

/* POSIX includes */
#include <time.h>

namespace PerfMon {
  inline
  struct timespec clock_gettime()
  {
    struct timespec res;
#ifndef __APPLE__
    /*int sc = */::clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &res);
#endif
    return res;
  }

  struct Clock
  {
    struct timespec start;
    struct timespec stop;

    static struct timespec resolution()
    {
      struct timespec r;
#ifndef __APPLE__
      /*sc*/clock_getres(CLOCK_PROCESS_CPUTIME_ID, &r);
#endif
      // std::cout << "::pmon:: resolution: " << r.tv_sec << ":" << r.tv_nsec 
      // 		<< std::endl;
      return r;
    }

    Clock()
      : stop()
    {
      this->start = PerfMon::clock_gettime();
    }
    struct timespec diff()
    {
      this->stop = PerfMon::clock_gettime();
      struct timespec delta;
      if ((stop.tv_nsec-start.tv_nsec)<0) {
	delta.tv_sec = stop.tv_sec-start.tv_sec-1;
	delta.tv_nsec = 1000000000+stop.tv_nsec-start.tv_nsec;
      } else {
	delta.tv_sec = stop.tv_sec-start.tv_sec;
	delta.tv_nsec = stop.tv_nsec-start.tv_nsec;
      }
      return delta;
    }
  };
}

namespace PerfMonEventDict {

  struct dict 
  {
    struct timespec m_1;
  };

  //clock_gettime

}

#endif
