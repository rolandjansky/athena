/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// System include(s):
#include <iostream>

// Local include(s):
#include "MuonTrackSummaryAccessors_v1.h"

/// Helper macro for Accessor objects
#define DEFINE_ACCESSOR(TYPE, NAME )                               \
   case xAOD::NAME:                                                \
   {                                                               \
      static const SG::AuxElement::Accessor< TYPE > a( #NAME );          \
      return &a;                                                   \
   }                                                               \
   break;

namespace xAOD {

   const SG::AuxElement::Accessor< uint8_t >*
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

        DEFINE_ACCESSOR( uint8_t, phiLayer1RPCHits );
        DEFINE_ACCESSOR( uint8_t, phiLayer2RPCHits );
        DEFINE_ACCESSOR( uint8_t, phiLayer3RPCHits );
        DEFINE_ACCESSOR( uint8_t, phiLayer4RPCHits );

        DEFINE_ACCESSOR( uint8_t, etaLayer1RPCHits );
        DEFINE_ACCESSOR( uint8_t, etaLayer2RPCHits );
        DEFINE_ACCESSOR( uint8_t, etaLayer3RPCHits );
        DEFINE_ACCESSOR( uint8_t, etaLayer4RPCHits );

        DEFINE_ACCESSOR( uint8_t, phiLayer1RPCHoles );
        DEFINE_ACCESSOR( uint8_t, phiLayer2RPCHoles );
        DEFINE_ACCESSOR( uint8_t, phiLayer3RPCHoles );
        DEFINE_ACCESSOR( uint8_t, phiLayer4RPCHoles );

        DEFINE_ACCESSOR( uint8_t, etaLayer1RPCHoles );
        DEFINE_ACCESSOR( uint8_t, etaLayer2RPCHoles );
        DEFINE_ACCESSOR( uint8_t, etaLayer3RPCHoles );
        DEFINE_ACCESSOR( uint8_t, etaLayer4RPCHoles );

        DEFINE_ACCESSOR( uint8_t, phiLayer1TGCHits );
        DEFINE_ACCESSOR( uint8_t, phiLayer2TGCHits );
        DEFINE_ACCESSOR( uint8_t, phiLayer3TGCHits );
        DEFINE_ACCESSOR( uint8_t, phiLayer4TGCHits );

        DEFINE_ACCESSOR( uint8_t, etaLayer1TGCHits );
        DEFINE_ACCESSOR( uint8_t, etaLayer2TGCHits );
        DEFINE_ACCESSOR( uint8_t, etaLayer3TGCHits );
        DEFINE_ACCESSOR( uint8_t, etaLayer4TGCHits );

        DEFINE_ACCESSOR( uint8_t, phiLayer1TGCHoles );
        DEFINE_ACCESSOR( uint8_t, phiLayer2TGCHoles );
        DEFINE_ACCESSOR( uint8_t, phiLayer3TGCHoles );
        DEFINE_ACCESSOR( uint8_t, phiLayer4TGCHoles );

        DEFINE_ACCESSOR( uint8_t, etaLayer1TGCHoles );
        DEFINE_ACCESSOR( uint8_t, etaLayer2TGCHoles );
        DEFINE_ACCESSOR( uint8_t, etaLayer3TGCHoles );
        DEFINE_ACCESSOR( uint8_t, etaLayer4TGCHoles );

	// New Small Wheel
        // STGC
	DEFINE_ACCESSOR( uint8_t, phiLayer1STGCHits );
	DEFINE_ACCESSOR( uint8_t, phiLayer2STGCHits );

        DEFINE_ACCESSOR( uint8_t, etaLayer1STGCHits );
        DEFINE_ACCESSOR( uint8_t, etaLayer2STGCHits );

	DEFINE_ACCESSOR( uint8_t, phiLayer1STGCHoles );
	DEFINE_ACCESSOR( uint8_t, phiLayer2STGCHoles );

	DEFINE_ACCESSOR( uint8_t, etaLayer1STGCHoles );
	DEFINE_ACCESSOR( uint8_t, etaLayer2STGCHoles );

	// MM
        DEFINE_ACCESSOR( uint8_t, MMHits );
	DEFINE_ACCESSOR( uint8_t, MMHoles );
	// Note: there is currently no MuonStationIndex for the two MM layers
	// In the future it might be needed to add them in order to have
        // separate counters as follows
        // DEFINE_ACCESSOR( uint8_t, Layer1MMHits );
	// DEFINE_ACCESSOR( uint8_t, Layer2MMHits );
	// DEFINE_ACCESSOR( uint8_t, Layer1MMHoles );
	// DEFINE_ACCESSOR( uint8_t, Layer2MMHoles );

        DEFINE_ACCESSOR( uint8_t, innerClosePrecisionHits );
        DEFINE_ACCESSOR( uint8_t, middleClosePrecisionHits );
        DEFINE_ACCESSOR( uint8_t, outerClosePrecisionHits );
        DEFINE_ACCESSOR( uint8_t, extendedClosePrecisionHits );

        DEFINE_ACCESSOR( uint8_t, innerOutBoundsPrecisionHits );
        DEFINE_ACCESSOR( uint8_t, middleOutBoundsPrecisionHits );
        DEFINE_ACCESSOR( uint8_t, outerOutBoundsPrecisionHits );
        DEFINE_ACCESSOR( uint8_t, extendedOutBoundsPrecisionHits );

        DEFINE_ACCESSOR( uint8_t, combinedTrackOutBoundsPrecisionHits );

        DEFINE_ACCESSOR( uint8_t, isEndcapGoodLayers );
        DEFINE_ACCESSOR( uint8_t, isSmallGoodSectors );

        DEFINE_ACCESSOR( uint8_t, cscEtaHits );
        DEFINE_ACCESSOR( uint8_t, cscUnspoiledEtaHits );

      default:                  
         std::cerr << "xAOD::MuonTrackParticle_v1 ERROR Unknown MuonSummaryType ("
                   << type << ") requested" << std::endl;
         return 0;
      }
   }
   
} // namespace xAOD
