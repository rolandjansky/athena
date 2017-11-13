/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file   AFPTrack_v1.cxx
 * @author Grzegorz Gach <grzegorz.gach@cern.ch>
 * @date   2016-07-14
 * 
 * @brief  Implementation file for class xAOD::AFPTrack_v1
 * 
 */


// xAOD include(s):
#include "xAODCore/AuxStoreAccessorMacros.h"

// Local include(s):
#include "xAODForward/versions/AFPSiHitContainer_v1.h"
#include "xAODForward/versions/AFPTrack_v1.h"

namespace xAOD
{
  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER (AFPTrack_v1, int, stationID, setStationID)
  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER (AFPTrack_v1, float, xLocal, setXLocal)
  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER (AFPTrack_v1, float, yLocal, setYLocal)
  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER (AFPTrack_v1, float, zLocal, setZLocal)
  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER (AFPTrack_v1, float, xSlope, setXSlope)
  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER (AFPTrack_v1, float, ySlope, setYSlope)
  // AUXSTORE_PRIMITIVE_SETTER_AND_GETTER (AFPTrack_v1,  float, zSlope, setZSlope)
  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER (AFPTrack_v1, int, nHoles, setNHoles)
  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER (AFPTrack_v1, int, nHits, setNHits)
  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER (AFPTrack_v1, float, chi2, setChi2)
  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER (AFPTrack_v1, int, algID, setAlgID)

  AUXSTORE_OBJECT_SETTER_AND_GETTER (AFPTrack_v1, std::vector< AFPTrack_v1::AFPHitLink_t >, hits, setHits)
  static SG::AuxElement::Accessor< std::vector<AFPTrack_v1::AFPHitLink_t> > hitsAcc( "hits" );

  void AFPTrack_v1::addHit( const AFPHitLink_t& link )
  {
    hitsAcc( *this ).push_back( link );
  }

  void AFPTrack_v1::toPersistent() {
    // Prepare the hits links for persistification:
    if( hitsAcc.isAvailableWritable( *this ) ) {
      std::vector<AFPHitLink_t>::iterator end = hitsAcc( *this ).end();
      for(std::vector<AFPHitLink_t>::iterator itr = hitsAcc( *this ).begin(); itr != end; ++itr )
	itr->toPersistent();

    }
    
      return;
  }

  
}
