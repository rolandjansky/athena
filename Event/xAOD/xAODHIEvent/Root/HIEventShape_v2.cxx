/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: HIEventShape_v2.cxx 693494 2015-09-07 14:59:45Z krasznaa $

// xAOD include(s):
#include "xAODCore/AuxStoreAccessorMacros.h"

// Local include(s):
#include "xAODHIEvent/versions/HIEventShape_v2.h"

namespace xAOD {

   HIEventShape_v2::HIEventShape_v2()
      : SG::AuxElement() {

   }

   AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( HIEventShape_v2, float, et, setEt )
   AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( HIEventShape_v2, float, area, setArea )
   AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( HIEventShape_v2, float, rho, setRho )
   AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( HIEventShape_v2, int, nCells,
                                         setNCells )

   AUXSTORE_OBJECT_SETTER_AND_GETTER( HIEventShape_v2, std::vector< float >,
                                      etCos, setEtCos )

   std::vector< float >& HIEventShape_v2::etCos() {

      static Accessor< std::vector< float > > acc( "etCos" );
      return acc( *this );
   }

   AUXSTORE_OBJECT_SETTER_AND_GETTER( HIEventShape_v2, std::vector< float >,
                                      etSin, setEtSin )

   std::vector< float >& HIEventShape_v2::etSin() {

      static Accessor< std::vector< float > > acc( "etSin" );
      return acc( *this );
   }

   AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( HIEventShape_v2, float, etaMin,
                                         setEtaMin )
   AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( HIEventShape_v2, float, etaMax,
                                         setEtaMax )
   AUXSTORE_PRIMITIVE_SETTER_AND_GETTER( HIEventShape_v2, int, layer, setLayer )

} // namespace xAOD
