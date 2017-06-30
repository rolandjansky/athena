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

      /// Access the object belonging to the current thread
      ///
      /// Components collecting information should always use this function. It
      /// is safe to be called at any point during the process, from any thread,
      /// without any outside protection.
      ///
      /// @return A thread specific xAOD::ReadStat object that can be modified
      ///
      ReadStats& stats();

      /// Access the statistics object, merging information from all threads
      ///
      /// This function should only be used during the finalisation of a
      /// process, once all the event processing threads have already finished.
      /// That's because it's not absolutely thread-safe. If another thread
      /// modifies an xAOD::ReadStats object through the stats() function while
      /// this function is executing, that can possibly lead to a crash.
      ///
      /// In the end the function is not made thread safe, because doing so
      /// would require the stats() function to be made slower.
      ///
      /// @return The merged statistics from all executing threads
      ///
      ReadStats merged() const;

   private:
      /// The constructor of the object is made private
      IOStats();
      /// The copy constructor is deleted
      IOStats( const IOStats& ) = delete;

      /// Objects describing the file access pattern, per thread
      std::map< std::thread::id, ReadStats > m_stats;

      /// Thread specific pointer to the ReadStats object of the current thread
      AthContainers_detail::thread_specific_ptr< ReadStats > m_ptr;

      /// Mutex for accessing the read statistics
      mutable std::mutex m_mutex;

   }; // class IOStats

} // namespace xAOD

#endif // XAODCORE_TOOLS_IOSTATS_H
