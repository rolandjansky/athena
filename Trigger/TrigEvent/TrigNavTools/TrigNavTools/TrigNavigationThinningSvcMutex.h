// -*- c++ -*-

/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGNAVTOOLS_TRIGNAVIGATIONTHINNINGSVCMUTEX_H
#define TRIGNAVTOOLS_TRIGNAVIGATIONTHINNINGSVCMUTEX_H

#include <mutex>

/**
 * @brief Low-granularity protection for Run 2 data paths. 
 * Currently shared by the TrigNavigationThinningSvc and AthenaMonNamager
 * Both of which are deprecated and can be removed following R2->R3 nav conversion project.
 */
class TrigNavigationThinningSvcMutex {
public:
  static std::mutex s_mutex;
};

#endif
