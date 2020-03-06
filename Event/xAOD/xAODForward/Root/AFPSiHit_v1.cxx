/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file   AFPSiHit_v1.cxx
 * @author Grzegorz Gach <grzegorz.gach@cern.ch>
 * @date   2016-07-14
 * 
 * @brief  Implementation file of the AFPSiHit_v1 class.
 * 
 */


// xAOD include(s):
#include "xAODCore/AuxStoreAccessorMacros.h"

// Local include(s):
#include "xAODForward/versions/AFPTrackContainer_v1.h"
#include "xAODForward/versions/AFPSiHit_v1.h"


namespace xAOD
{
  AFPSiHit_v1::AFPSiHit_v1() : SG::AuxElement()
  { }

  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER (AFPSiHit_v1, float, depositedCharge, setDepositedCharge)
  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER (AFPSiHit_v1, float, timeOverThreshold, setTimeOverThreshold)
  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER (AFPSiHit_v1, int, stationID, setStationID)
  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER (AFPSiHit_v1, int, pixelLayerID, setPixelLayerID)
  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER (AFPSiHit_v1, int, pixelRowIDChip, setPixelRowIDChip)
  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER (AFPSiHit_v1, int, pixelColIDChip, setPixelColIDChip)
  
  AUXSTORE_OBJECT_SETTER_AND_GETTER (AFPSiHit_v1, std::vector< AFPSiHit_v1::AFPTrackLink_t >, tracksLinks, setTracksLinks)
  static SG::AuxElement::Accessor< std::vector<AFPSiHit_v1::AFPTrackLink_t> > tracksLinksAcc( "tracksLinks" );
  
  void AFPSiHit_v1::addTrackLink( const AFPTrackLink_t& link )
  {
    tracksLinksAcc( *this ).push_back( link );
  }

  void AFPSiHit_v1::toPersistent() {
    // Prepare the tracks links for persistification:
    if ( tracksLinksAcc.isAvailableWritable( *this ) )
      for (AFPTrackLink_t trackLink : tracksLinksAcc( *this ) )
	trackLink.toPersistent();
  }

  
  int AFPSiHit_v1::pixelHorizID() const
  {
    // use pixel rows because detectors are rotated and subtract
    // from 336, because the axis is reversed
    return 336 - pixelRowIDChip();
  }

  void AFPSiHit_v1::setPixelHorizID(const int horizontalID) 
  {
    // use pixel rows because detectors are rotated and subtract
    // from 336, because the axis is reversed
    setPixelRowIDChip (336 - horizontalID);
  }


  int AFPSiHit_v1::pixelVertID() const
  {
    // use pixel columns because detectors are rotated
    return pixelColIDChip();
  }

  void AFPSiHit_v1::setPixelVertID(const int verticalID) 
  {
    // use pixel columns because detectors are rotated
    setPixelColIDChip (verticalID);
  }



  // Type::ObjectType AFPSiHit_v1::type() const
  // {
  //   return Type::AFPSiHit;
  // }
  
}
