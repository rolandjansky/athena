/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: IOStats.cxx 579516 2014-01-22 10:57:33Z krasznaa $

// Local include(s):
#include "xAODCore/tools/IOStats.h"
#include "xAODCore/tools/ReadStats.h"

namespace xAOD {

   // Initialise the static variable(s):
   IOStats* IOStats::m_instance = 0;

   IOStats::~IOStats() {

      delete m_stats;
   }

   IOStats& IOStats::instance() {

      if( ! m_instance ) {
         m_instance = new IOStats();
      }

      return *m_instance;
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
