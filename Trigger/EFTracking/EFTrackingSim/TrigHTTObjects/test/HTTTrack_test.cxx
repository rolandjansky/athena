/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

/*
 * HTTTrack_test.cxx: Unit tests for HTTTrack
 */

#include <cstdio>
#include <iostream>
#include <cassert>
#include "TrigHTTObjects/HTTTrack.h"

int main(int, char**)
{
  const double qoverpt(-0.123456), chi2(3.14159);

  HTTTrack track;
  HTTHit hits[8];
  track.setNLayers(8);

  for (unsigned i = 0; i < 8; i++) {
    hits[i].setPhiIndex(i*i);
    hits[i].setEtaIndex(i+1);
    hits[i].setLayer(i);
    hits[i].setHitType(HitType::mapped);
    track.setHTTHit(i,hits[i]);
  }
  track.setQOverPt(qoverpt);
  track.setChi2(chi2);
  std::cout << "q/pt = " << track.getQOverPt() << std::endl;
  std::cout << "chi2 = " << track.getChi2() << std::endl;

  const std::vector <HTTHit> returned_hits = track.getHTTHits();
  for (unsigned i = 0; i < 8; i++) {
    std::cout << "layer = " << returned_hits[i].getLayer() << " and phi = " << returned_hits[i].getPhiIndex() << 
      " " << " and eta = " << returned_hits[i].getEtaIndex() << std::endl;
  }

  return 0;

}

