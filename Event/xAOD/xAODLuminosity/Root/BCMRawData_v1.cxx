/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: BCMRawData_v1.cxx 652111 2015-03-06 10:13:22Z krasznaa $

// xAOD include(s):
#include "xAODCore/AuxStoreAccessorMacros.h"

// Local include(s):
#include "xAODLuminosity/versions/BCMRawData_v1.h"

namespace xAOD {

   BCMRawData_v1::BCMRawData_v1()
   : SG::AuxElement() {

   }

   /////////////////////////////////////////////////////////////////////////////
   //
   //          Implementation of the raw data accessor functions
   //

   AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( BCMRawData_v1, uint32_t, bcmWord1,
                                         setBcmWord1 )
   AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( BCMRawData_v1, uint32_t, bcmWord2,
                                         setBcmWord2 )

   //
   /////////////////////////////////////////////////////////////////////////////

   /////////////////////////////////////////////////////////////////////////////
   //
   //              Implementation of the convenience functions
   //

   int BCMRawData_v1::channel() const {

      return ( bcmWord1() & 0xf );
   }

   int BCMRawData_v1::pulse1Position() const {

      return ( ( bcmWord1() >> 4 ) & 0x3f );
   }

   int BCMRawData_v1::pulse1Width() const {

      return ( ( bcmWord1() >> 10 ) & 0x1f );
   }

   int BCMRawData_v1::pulse2Position() const {

      return ( ( bcmWord1() >> 15 ) & 0x3f );
   }

   int BCMRawData_v1::pulse2Width() const {

      return ( ( bcmWord1() >> 21 ) & 0x1f );
   }

   int BCMRawData_v1::lvl1a() const {

      return ( ( bcmWord1() >> 26 ) & 0x3f );
   }

   int BCMRawData_v1::bcid() const {

      return ( bcmWord2() & 0xfff );
   }

   int BCMRawData_v1::lvl1id() const {

      return ( ( bcmWord2() >> 12 ) & 0xffff );
   }

   int BCMRawData_v1::error() const {

      return ( ( bcmWord2() >> 28 ) & 0xf );
   }

   //
   /////////////////////////////////////////////////////////////////////////////

} // namespace xAOD
