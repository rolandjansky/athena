// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: LumiBlockRange_v1.h 647501 2015-02-17 14:22:33Z krasznaa $
#ifndef XAODLUMINOSITY_VERSIONS_LUMIBLOCKRANGE_V1_H
#define XAODLUMINOSITY_VERSIONS_LUMIBLOCKRANGE_V1_H

// System include(s):
extern "C" {
#   include <stdint.h>
}

// Core EDM include(s):
#include "AthContainers/AuxElement.h"

namespace xAOD {

   /// Class describing a luminosity block range
   ///
   /// This class is used in the metadata of ATLAS analysis files to describe
   /// which lumiblocks were fully or partly processed to produce a certain
   /// file.
   ///
   /// @author Marjorie Shapiro <mdshapiro@lbl.gov>
   /// @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
   ///
   /// $Revision: 647501 $
   /// $Date: 2015-02-17 15:22:33 +0100 (Tue, 17 Feb 2015) $
   ///
   class LumiBlockRange_v1 : public SG::AuxElement {

   public:
      /// Default constructor
      LumiBlockRange_v1();
      /// Copy constructor
      LumiBlockRange_v1( const LumiBlockRange_v1& parent );

      /// Assignment operator
      LumiBlockRange_v1& operator= ( const LumiBlockRange_v1& rhs );

      /// @name Start time of the luminosity block range
      /// @{

      /// Get the run number of the start time of the range
      uint32_t startRunNumber() const;
      /// Set the run number of the start time of the range
      void setStartRunNumber( uint32_t value );

      /// Get the luminosity block of the start time of the range
      uint32_t startLumiBlockNumber() const;
      /// Set the luminosity block of the start time of the range
      void setStartLumiBlockNumber( uint32_t value );

      /// @}

      /// @name Stop time of the luminosity block range
      /// @{

      /// Get the run number of the stop time of the range
      uint32_t stopRunNumber() const;
      /// Set the run number of the stop time of the range
      void setStopRunNumber( uint32_t value );

      /// Get the luminosity block of the stop time of the range
      uint32_t stopLumiBlockNumber() const;
      /// Set the luminosity block of the stop time of the range
      void setStopLumiBlockNumber( uint32_t value );

      /// @}

      /// @name Event numbers in the luminosity block range
      /// @{

      /// Get the number of expected events in this luminosity block range
      uint32_t eventsExpected() const;
      /// Set the number of expected events in this luminosity block range
      void setEventsExpected( uint32_t value );

      /// Get the number of seen/processed events in this luminosity block range
      uint32_t eventsSeen() const;
      /// Set the number of seen/processed events in this luminosity block range
      void setEventsSeen( uint32_t value );

      /// @}

   }; // class LumiBlockRange_v1

} // namespace xAOD

#endif // XAODLUMINOSITY_VERSIONS_LUMIBLOCKRANGE_V1_H
