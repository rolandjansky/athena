// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: IOStats.h 573109 2013-11-29 16:40:02Z krasznaa $
#ifndef XAODCORE_TOOLS_IOSTATS_H
#define XAODCORE_TOOLS_IOSTATS_H

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
   /// $Revision: 573109 $
   /// $Date: 2013-11-29 17:40:02 +0100 (Fri, 29 Nov 2013) $
   ///
   class IOStats {

   public:
      /// Destructor
      ~IOStats();

      /// Singleton object accessor
      static IOStats& instance();

      /// Access the object describing the file access pattern
      ReadStats& stats();
      /// Access the object describing the file access pattern (const version)
      const ReadStats& stats() const;

   private:
      /// The constructor of the object is made private
      IOStats();

      /// Pointer to the only IOStats object in memory
      static IOStats* m_instance;

      /// The object describing the file access pattern
      ReadStats* m_stats;

   }; // class IOStats

} // namespace xAOD

#endif // XAODCORE_TOOLS_IOSTATS_H
