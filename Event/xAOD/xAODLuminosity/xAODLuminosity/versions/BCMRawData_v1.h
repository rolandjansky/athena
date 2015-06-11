// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: BCMRawData_v1.h 652111 2015-03-06 10:13:22Z krasznaa $
#ifndef XAODLUMINOSITY_VERSIONS_BCMRAWDATA_V1_H
#define XAODLUMINOSITY_VERSIONS_BCMRAWDATA_V1_H

// System include(s):
extern "C" {
#   include "stdint.h"
}

// Core EDM include(s):
#include "AthContainers/AuxElement.h"

namespace xAOD {

   /// Class describing the BCM raw data
   ///
   /// This is pretty much a literal translation of the old BCM_RawData class
   /// for the xAOD EDM.
   ///
   /// @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
   ///
   /// $Revision: 652111 $
   /// $Date: 2015-03-06 11:13:22 +0100 (Fri, 06 Mar 2015) $
   ///
   class BCMRawData_v1 : public SG::AuxElement {

   public:
      /// Default constructor
      BCMRawData_v1();

      /// @name The actual raw data payload
      /// @{

      /// The first raw word
      uint32_t bcmWord1() const;
      /// Set the first raw word
      void setBcmWord1( uint32_t value );

      /// The second raw word
      uint32_t bcmWord2() const;
      /// Set the second raw word
      void setBcmWord2( uint32_t value );

      /// @}

      /// @name Conveninence accessors to certain bits of the raw data
      /// @{

      /// Channel identifier [0-15]
      int channel() const;

      /// Position of first pulse [0-63]
      int pulse1Position() const;
      /// Width of first pulse [0-31]
      int pulse1Width() const;

      /// Position of second pulse [0-63]
      int pulse2Position() const;
      /// Width of second pulse [0-31]
      int pulse2Width() const;

      /// Level-1 Accept [0-63]
      int lvl1a() const;
      /// Bunch crossing ID [0-4095]
      int bcid() const;
      /// Level-1 ID [0-65535]
      int lvl1id() const;

      /// Error bits [0-15]
      int error() const;

      /// @}

   }; // class BCMRawData_v1

} // namespace xAOD

#endif // XAODLUMINOSITY_VERSIONS_BCMRAWDATA_V1_H
