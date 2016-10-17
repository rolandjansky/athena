/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: IOStats.cxx 778387 2016-10-14 00:02:04Z krasznaa $

// Local include(s):
#include "xAODCore/tools/IOStats.h"
#include "xAODCore/tools/ReadStats.h"

namespace xAOD {

   IOStats::~IOStats() {

      // The ReadStats object is not deleted. Since the object may be needed
      // during the finalisation of the application. In which case it can lead
      // to a race condition which object is deleted first. This one, or the one
      // trying to use this one in its destructor.
      //      delete m_stats;
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
