// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: IOStats.h 621105 2014-10-10 12:34:11Z krasznaa $
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
   /// $Revision: 621105 $
   /// $Date: 2014-10-10 14:34:11 +0200 (Fri, 10 Oct 2014) $
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

      /// The object describing the file access pattern
      ReadStats* m_stats;

   }; // class IOStats

} // namespace xAOD

#endif // XAODCORE_TOOLS_IOSTATS_H
