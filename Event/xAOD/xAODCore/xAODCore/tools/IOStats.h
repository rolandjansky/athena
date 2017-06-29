// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAODCORE_TOOLS_IOSTATS_H
#define XAODCORE_TOOLS_IOSTATS_H

// System include(s):
#include <map>
#include <thread>
#include <mutex>

// EDM include(s):
#include "AthContainers/tools/threading.h"

// Local include(s):
#include "xAODCore/tools/ReadStats.h"

namespace xAOD {

   // Forward declaration(s):
   class ReadStats;

   /// Singleton object holding on to the process's I/O statistics
   ///
   /// This singleton class holds the xAOD file access statistics object
   /// describing the auxiliary variable access pattern of the current
   /// process.
   ///
   /// @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
   ///
   class IOStats {

   public:
      /// Singleton object accessor
      static IOStats& instance();

      /// Access the object describing the file access pattern in the current
      /// thread
      ReadStats& stats();

      /// Access the statistics object merging information from all threads
      ReadStats merged() const;

   private:
      /// The constructor of the object is made private
      IOStats();
      /// The copy constructor is deleted
      IOStats( const IOStats& ) = delete;

      /// Objects describing the file access pattern, per thread
      std::map< std::thread::id, ReadStats > m_stats;

      /// Helper structure holding a pointer to the ReadStats object in the
      /// current thread.
      class ReadStatsPtr {
      public:
         /// Constructor making sure that we start with a null pointer
         ReadStatsPtr() : m_ptr( nullptr ) {}
         /// Pointer to the ReadStats object of a given thread
         ReadStats* m_ptr;
      }; // class ReadStatsPtr

      /// Thread specific pointer to the ReadStats object of the current thread
      AthContainers_detail::thread_specific_ptr< ReadStatsPtr > m_ptr;

      /// Mutex for accessing the read statistics
      mutable std::mutex m_mutex;

   }; // class IOStats

} // namespace xAOD

#endif // XAODCORE_TOOLS_IOSTATS_H
