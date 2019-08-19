/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonHoughPatternTools/HoughDataPerSec.h"

namespace Muon {

void HoughDataPerSec::cleanUp() {
  for(RegionHitVec::iterator it=hitVec.begin();it!=hitVec.end();++it)
    for( HitVec::iterator it2=it->begin();it2!=it->end();++it2 ) delete *it2;
  hitVec.clear();

  for(RegionPhiHitVec::iterator it=phiHitVec.begin();it!=phiHitVec.end();++it)
    for( PhiHitVec::iterator it2=it->begin();it2!=it->end();++it2 ) delete *it2;
  phiHitVec.clear();

  for(RegionMaximumVec::iterator it=maxVec.begin();it!=maxVec.end();++it)
    for( MaximumVec::iterator it2=it->begin();it2!=it->end();++it2 ) delete *it2;
  maxVec.clear();

  for(RegionPhiMaximumVec::iterator it=phiMaxVec.begin();it!=phiMaxVec.end();++it)
    for( PhiMaximumVec::iterator it2=it->begin();it2!=it->end();++it2 ) delete *it2;
  phiMaxVec.clear();
}

}