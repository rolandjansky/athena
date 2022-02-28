/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file   AFPToFTrack_v1.cxx
 * @author Leszek Adamczyk <leszek.adamczyk@agh.edu.pl>
 * @date   2021-11-11
 * 
 * @brief  Implementation file for class xAOD::AFPToFTrack_v1
 * 
 */


// xAOD include(s):
#include "xAODCore/AuxStoreAccessorMacros.h"

// Local include(s):
#include "xAODForward/versions/AFPToFHitContainer_v1.h"
#include "xAODForward/versions/AFPToFTrack_v1.h"

namespace xAOD
{
  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER (AFPToFTrack_v1, int, stationID, setStationID)
  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER (AFPToFTrack_v1, int, trainID, setTrainID)
  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER (AFPToFTrack_v1, float, trainTime, setTrainTime)
  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER (AFPToFTrack_v1, int, trainSize, setTrainSize)
  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER (AFPToFTrack_v1, int, trainNSat, setTrainNSat)
  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER (AFPToFTrack_v1, int, algID, setAlgID)

  AUXSTORE_OBJECT_SETTER_AND_GETTER (AFPToFTrack_v1, std::vector< AFPToFTrack_v1::AFPToFHitLink_t >, hits, setHits)
  static const SG::AuxElement::Accessor< std::vector<AFPToFTrack_v1::AFPToFHitLink_t> > hitsAcc( "bars" );

  void AFPToFTrack_v1::addBar( const AFPToFHitLink_t& link )
  {
    hitsAcc( *this ).push_back( link );
  }

  void AFPToFTrack_v1::toPersistent() {
    // Prepare the hits links for persistification:
    if( hitsAcc.isAvailableWritable( *this ) ) {
      std::vector<AFPToFTrack_v1::AFPToFHitLink_t>::iterator end = hitsAcc( *this ).end();
      for(std::vector<AFPToFTrack_v1::AFPToFHitLink_t>::iterator itr = hitsAcc( *this ).begin(); itr != end; ++itr )
	itr->toPersistent();

    }
  }
  
}
