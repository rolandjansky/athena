/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file   AFPSiHit_v2.cxx
 * @author Grzegorz Gach <grzegorz.gach@cern.ch>
 * @date   2017-04-26
 * 
 * @brief  Implementation file of the AFPSiHit_v2 class.
 * 
 */


// xAOD include(s):
#include "xAODCore/AuxStoreAccessorMacros.h"

// Local include(s):
#include "xAODForward/versions/AFPSiHit_v2.h"


namespace xAOD
{
  AFPSiHit_v2::AFPSiHit_v2() : SG::AuxElement()
  { }

  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER (AFPSiHit_v2, float, depositedCharge, setDepositedCharge)
  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER (AFPSiHit_v2, float, timeOverThreshold, setTimeOverThreshold)
  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER (AFPSiHit_v2, int, stationID, setStationID)
  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER (AFPSiHit_v2, int, pixelLayerID, setPixelLayerID)
  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER (AFPSiHit_v2, int, pixelRowIDChip, setPixelRowIDChip)
  AUXSTORE_PRIMITIVE_SETTER_AND_GETTER (AFPSiHit_v2, int, pixelColIDChip, setPixelColIDChip)
  
  void AFPSiHit_v2::toPersistent() {
  }
  
  int AFPSiHit_v2::pixelHorizID() const
  {
    // use pixel rows because detectors are rotated and subtract
    // from 336, because the axis is reversed
    return 336 - pixelRowIDChip();
  }

  void AFPSiHit_v2::setPixelHorizID(const int horizontalID) 
  {
    // use pixel rows because detectors are rotated and subtract
    // from 336, because the axis is reversed
    setPixelRowIDChip (336 - horizontalID);
  }


  int AFPSiHit_v2::pixelVertID() const
  {
    // use pixel columns because detectors are rotated
    return pixelColIDChip();
  }

  void AFPSiHit_v2::setPixelVertID(const int verticalID) 
  {
    // use pixel columns because detectors are rotated
    setPixelColIDChip (verticalID);
  }
} // end xAOD
