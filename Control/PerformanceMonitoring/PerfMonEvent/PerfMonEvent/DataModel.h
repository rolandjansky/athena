///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

// DataModel.h 
// Header file for class(es) of the PerfMon DataModel
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 
#ifndef PERFMONEVENT_DATAMODEL_H 
#define PERFMONEVENT_DATAMODEL_H 

// STL includes
#include <time.h>
#include "PerfMonEvent/mallinfo.h"

// PerfMonEvent includes

// Forward declaration

namespace PerfMon {

  inline
  float rt_cpu()
  {
#if !defined(__APPLE__)
    struct timespec res;
    /*int sc = */::clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &res);
    return (res.tv_sec*1e3 + res.tv_nsec*1e-6); // in milliseconds
#else
    return -999.;
#endif
  }

  struct Cpu {
    Cpu() : user(0.), sys(0.), real(0.), rt_cpu(0.) {}
    float user;
    float sys;
    float real;
    float rt_cpu;
    float cpu() const { return user + sys; }
  };

  struct Mem {
    Mem()
    {
      vmem[0] = vmem[1] = 0.;
      rss[0]  = rss[1]  = 0.;
      mall[0] = mall[1] = 0.;
      nmall[0]= nmall[1]= 0.;
      nfree[0]= nfree[1]= 0.;
   }

    float vmem[2];
    float rss[2];
    float mall[2];
    float nmall[2];
    float nfree[2];
    float dVmem() const { return vmem[1] - vmem[0]; }
    float dRss()  const { return rss [1] - rss [0]; }
    float dMall() const { return mall[1] - mall[0]; }
    float dNMall() const { return nmall[1] - nmall[0]; }
    float dNFree() const { return nfree[1] - nfree[0]; }
  };

  struct IoContainer {
    Cpu r;
    Cpu rr;
    Cpu w;
  };

  struct Component { //-> 12+32=44 bytes
    Cpu cpu;
    Mem mem;
  };

  struct MallInfo {
    static 
    inline float mallInfo()
    { 
#if !defined(__APPLE__)
      const PerfMon::mallinfo_t mi = PerfMon::mallinfo();
      return float(mi.uordblks+mi.hblkhd);
#else
      return -999.;
#endif
    }
  };

//   struct MallInfo {
//     int arena;    /* non-mmapped space allocated from system */
//     int ordblks;  /* number of free chunks */
//     int smblks;   /* number of fastbin blocks */
//     int hblks;    /* number of mmapped regions */
//     int hblkhd;   /* space in mmapped regions */
//     int usmblks;  /* maximum total allocated space */
//     int fsmblks;  /* space available in freed fastbin blocks */
//     int uordblks; /* total allocated space */
//     int fordblks; /* total free space */
//     int keepcost; /* top-most, releasable (via malloc_trim) space */
//   };

//   MallInfo mallInfo();
// PerfMon::MallInfo PerfMon::mallInfo() 
// { 
//   const MallInfo_t mi = ::mallinfo();
//   MallInfo mo;
//   mo.arena    = mi.arena;    /* non-mmapped space allocated from system */
//   mo.ordblks  = mi.ordblks;  /* number of free chunks */
//   mo.smblks   = mi.smblks;   /* number of fastbin blocks */
//   mo.hblks    = mi.hblks;    /* number of mmapped regions */
//   mo.hblkhd   = mi.hblkhd;   /* space in mmapped regions */
//   mo.usmblks  = mi.usmblks;  /* maximum total allocated space */
//   mo.fsmblks  = mi.fsmblks;  /* space available in freed fastbin blocks */
//   mo.uordblks = mi.uordblks; /* total allocated space */
//   mo.fordblks = mi.fordblks; /* total free space */
//   mo.keepcost = mi.keepcost; /* top-most, releasable (via malloc_trim) space */
//   return mo;
// }

} // end namespace PerfMon

#endif //> PERFMONEVENT_DATAMODEL_H
