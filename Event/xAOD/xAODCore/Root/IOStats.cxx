/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: IOStats.cxx 621105 2014-10-10 12:34:11Z krasznaa $

// Local include(s):
#include "xAODCore/tools/IOStats.h"
#include "xAODCore/tools/ReadStats.h"

namespace xAOD {

   IOStats::~IOStats() {

      delete m_stats;
   }

   IOStats& IOStats::instance() {

      static IOStats obj;
      return obj;
   }

   ReadStats& IOStats::stats() {

      return *m_stats;
   }

   const ReadStats& IOStats::stats() const {

      return *m_stats;
   }

   IOStats::IOStats()
      : m_stats( new ReadStats() ) {

   }

} // namespace xAOD
