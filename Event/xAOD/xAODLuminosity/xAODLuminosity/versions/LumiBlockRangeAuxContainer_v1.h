// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: LumiBlockRangeAuxContainer_v1.h 647501 2015-02-17 14:22:33Z krasznaa $
#ifndef XAODLUMINOSITY_VERSIONS_LUMIBLOCKRANGEAUXCONTAINER_V1_H
#define XAODLUMINOSITY_VERSIONS_LUMIBLOCKRANGEAUXCONTAINER_V1_H

// System include(s):
#include <vector>
extern "C" {
#   include <stdint.h>
}

// xAOD include(s):
#include "xAODCore/AuxContainerBase.h"

namespace xAOD {

   /// Auxiliary container for xAOD::LumiBlockRangeContainer_v1
   ///
   /// This class holds the payload of the luminosity block range container,
   /// describing what fraction of which luminosity blocks were accessed to
   /// produce a certain output file.
   ///
   /// @author Marjorie Shapiro <mdshapiro@lbl.gov>
   /// @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
   ///
   /// $Revision: 647501 $
   /// $Date: 2015-02-17 15:22:33 +0100 (Tue, 17 Feb 2015) $
   ///
   class LumiBlockRangeAuxContainer_v1 : public AuxContainerBase {

   public:
      /// Default constructor
      LumiBlockRangeAuxContainer_v1();

   private:
      /// @name Start time of the luminosity block range
      /// @{
      /// The run number of the start time of the range
      std::vector< uint32_t > startRunNumber;
      /// The luminosity block of the start time of the range
      std::vector< uint32_t > startLumiBlockNumber;
      /// @}

      /// @name Stop time of the luminosity block range
      /// @{
      /// The run number of the stop time of the range
      std::vector< uint32_t > stopRunNumber;
      /// The luminosity block of the stop time of the range
      std::vector< uint32_t > stopLumiBlockNumber;
      /// @}

      /// @name Event numbers in the luminosity block range
      /// @{
      /// The number of expected events in this luminosity block range
      std::vector< uint32_t > eventsExpected;
      /// The number of seen/processed events in this luminosity block range
      std::vector< uint32_t > eventsSeen;
      /// @}

   }; // class LumiBlockRangeAuxContainer_v1

} // namespace xAOD

// Set up the StoreGate inheritance of the class:
#include "xAODCore/BaseInfo.h"
SG_BASE( xAOD::LumiBlockRangeAuxContainer_v1, xAOD::AuxContainerBase );

#endif // XAODLUMINOSITY_VERSIONS_LUMIBLOCKRANGEAUXCONTAINER_V1_H
