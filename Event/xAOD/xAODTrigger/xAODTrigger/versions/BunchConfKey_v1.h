// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: BunchConfKey_v1.h 630856 2014-11-25 11:22:49Z gwatts $
#ifndef XAODTRIGGER_VERSIONS_BUNCHCONFKEY_V1_H
#define XAODTRIGGER_VERSIONS_BUNCHCONFKEY_V1_H

// System include(s):
extern "C" {
#   include <stdint.h>
}

namespace xAOD {

   /// Class identifying the bunch configuration of a given event
   ///
   /// We put one object of this type into every event to identify which
   /// bunch configuration was used to produce it. In a way very similar
   /// to how xAOD::TrigConfKeys is used.
   ///
   /// @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
   ///
   /// $Revision: 630856 $
   /// $Date: 2014-11-25 12:22:49 +0100 (Tue, 25 Nov 2014) $
   ///
   class BunchConfKey_v1 {

   public:
      /// Default constructor
      BunchConfKey_v1( uint32_t id = 0 );

      /// Get the 32-bit identifier of the bunch configuration
      uint32_t id() const;
      /// Set the 32-bit identifier of the bunch configuration
      void setId( uint32_t id );

   private:
      /// The 32-bit identifier of the bunch configuration
      uint32_t m_id;

   }; // class BunchConfKey_v1

} // namespace xAOD

#endif // XAODTRIGGER_VERSIONS_BUNCHCONFKEY_V1_H
