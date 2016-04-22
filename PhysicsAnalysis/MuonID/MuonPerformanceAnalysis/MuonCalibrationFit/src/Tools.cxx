/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#define __Tools_C__
#include "MuonCalibrationFit/Tools.h"

bool Tools::RegionsSorter( RegionInfo* a, RegionInfo* b ) {
  return ( a->Order < b->Order );
}

float Tools::GetWeight( int chNum ) {
  switch( chNum ) {
    case 361107: return  1.9012 * 1. / 19942848.;
    case 361101: return  11.306 * 1. / 27951402.;
    case 361104: return  8.2826 * 1. / 19965914.;
    case 361108: return  1.9012 * 1. / 19197457.;
    case 410000: return  0.69611 * 0.54383 / 19958779.;
    default    : return  1.;
  }
}

float Tools::GetNormPhi( float phi, float eta ) {

  float LargeSector = 0.5;
  float SmallSector = 1.5;

  //::: Barrel
  if( TMath::Abs( eta ) < 1.05 ) {
    //::: Large Sectors
    if( TMath::Abs( phi ) > 2.960 ) return LargeSector;
    for( int i = -3; i <= 3; i++ ) {
      float min_bound = -0.18 + i * 0.785;
      float max_bound = 0.18 + i * 0.785;
      if( min_bound < phi && phi < max_bound ) return LargeSector;
    }
    return SmallSector;
  }

  //::: Endcap 
  else {
    //::: Large Sectors
    if( TMath::Abs( phi ) > 3.011 ) return LargeSector;
    for( int i = -3; i <= 3; i++ ) {
      float min_bound = -0.131 + i * 0.784;
      float max_bound = 0.131 + i * 0.784;
      if( min_bound < phi && phi < max_bound ) return LargeSector;
    }
    return SmallSector;
  }

}
