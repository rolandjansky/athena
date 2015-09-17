/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: MuonTrackSummaryAccessors_v1.cxx 599069 2014-05-27 14:13:41Z emoyse $

// System include(s):
#include <iostream>

// Local include(s):
#include "MuonTrackSummaryAccessors_v1.h"

/// Helper macro for Accessor objects
#define DEFINE_ACCESSOR(TYPE, NAME )                               \
   case xAOD::NAME:                                                \
   {                                                               \
      static SG::AuxElement::Accessor< TYPE > a( #NAME );          \
      return &a;                                                   \
   }                                                               \
   break;

namespace xAOD {

   SG::AuxElement::Accessor< uint8_t >*
   muonTrackSummaryAccessorV1( xAOD::MuonSummaryType type ) {

      switch( type ) {
        DEFINE_ACCESSOR( uint8_t, primarySector );
        DEFINE_ACCESSOR( uint8_t, secondarySector );
        DEFINE_ACCESSOR( uint8_t, innerSmallHits );
        DEFINE_ACCESSOR( uint8_t, innerLargeHits );
        DEFINE_ACCESSOR( uint8_t, middleSmallHits );
        DEFINE_ACCESSOR( uint8_t, middleLargeHits );
        DEFINE_ACCESSOR( uint8_t, outerSmallHits );
        DEFINE_ACCESSOR( uint8_t, outerLargeHits );
        DEFINE_ACCESSOR( uint8_t, extendedSmallHits );
        DEFINE_ACCESSOR( uint8_t, extendedLargeHits );

        DEFINE_ACCESSOR( uint8_t, innerSmallHoles );
        DEFINE_ACCESSOR( uint8_t, innerLargeHoles );
        DEFINE_ACCESSOR( uint8_t, middleSmallHoles );
        DEFINE_ACCESSOR( uint8_t, middleLargeHoles );
        DEFINE_ACCESSOR( uint8_t, outerSmallHoles );
        DEFINE_ACCESSOR( uint8_t, outerLargeHoles );
        DEFINE_ACCESSOR( uint8_t, extendedSmallHoles );
        DEFINE_ACCESSOR( uint8_t, extendedLargeHoles );

        DEFINE_ACCESSOR( uint8_t, phiLayer1Hits );
        DEFINE_ACCESSOR( uint8_t, phiLayer2Hits );
        DEFINE_ACCESSOR( uint8_t, phiLayer3Hits );
        DEFINE_ACCESSOR( uint8_t, phiLayer4Hits );

        DEFINE_ACCESSOR( uint8_t, etaLayer1Hits );
        DEFINE_ACCESSOR( uint8_t, etaLayer2Hits );
        DEFINE_ACCESSOR( uint8_t, etaLayer3Hits );
        DEFINE_ACCESSOR( uint8_t, etaLayer4Hits );

        DEFINE_ACCESSOR( uint8_t, phiLayer1Holes );
        DEFINE_ACCESSOR( uint8_t, phiLayer2Holes );
        DEFINE_ACCESSOR( uint8_t, phiLayer3Holes );
        DEFINE_ACCESSOR( uint8_t, phiLayer4Holes );

        DEFINE_ACCESSOR( uint8_t, etaLayer1Holes );
        DEFINE_ACCESSOR( uint8_t, etaLayer2Holes );
        DEFINE_ACCESSOR( uint8_t, etaLayer3Holes );
        DEFINE_ACCESSOR( uint8_t, etaLayer4Holes );

      default:                  
         std::cerr << "xAOD::MuonTrackParticle_v1 ERROR Unknown MuonSummaryType ("
                   << type << ") requested" << std::endl;
         return 0;
      }
   }
   
} // namespace xAOD
