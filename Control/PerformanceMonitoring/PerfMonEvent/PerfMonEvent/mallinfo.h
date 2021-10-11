// This file's extension implies that it's C, but it's really -*- C++ -*-.
/*
 * Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration.
 */
/**
 * @file PerfMonEvent/mallinfo.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Jun, 2021
 * @brief Wrappers for mallinfo.
 */


#ifndef PERFMONEVENT_MALLINFO_H
#define PERFMONEVENT_MALLINFO_H


#include "CxxUtils/features.h"


#ifndef __APPLE__
#include <malloc.h>

namespace PerfMon {
#if HAVE_MALLINFO2
using mallinfo_t = struct mallinfo2;
inline mallinfo_t mallinfo() { return ::mallinfo2(); }
#else
using mallinfo_t = struct mallinfo;
inline mallinfo_t mallinfo() { return ::mallinfo(); }
#endif
}

#else /* brain dead macos: define a dummy mallinfo structure */
struct mallinfo {
    int arena;
    int ordblks;  /* number of free chunks */
    int smblks;   /* number of fastbin blocks */
    int hblks;    /* number of mmapped regions */
    int hblkhd;   /* space in mmapped regions */
    int usmblks;  /* maximum total allocated space */
    int fsmblks;  /* space available in freed fastbin blocks */
    int uordblks; /* total allocated space */
    int fordblks; /* total free space */
    int keepcost; /* top-most, releasable (via malloc_trim) space */
};
namespace PerfMon {
using mallinfo_t = struct mallinfo;
}
#endif


#endif // not PERFMONEVENT_MALLINFO_H
