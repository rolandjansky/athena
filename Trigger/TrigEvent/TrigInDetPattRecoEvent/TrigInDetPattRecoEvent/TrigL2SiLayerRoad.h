/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef __TRIG_L2_SI_LAYER_ROAD_H__
#define __TRIG_L2_SI_LAYER_ROAD_H__

#include <set>
#include <iostream>
#include <iterator>
#include <algorithm>

//original name: STRoad.h - re-packaged from TrigSiTrack

struct TrigL2SiLayerRoad  {
   
  TrigL2SiLayerRoad() : lay1(100), lay2(100) {}
  TrigL2SiLayerRoad(const TrigL2SiLayerRoad& r) : lay1(r.lay1), lay2(r.lay2) {
    lay3 = r.lay3;
  }

  const TrigL2SiLayerRoad& operator = (const TrigL2SiLayerRoad& r) {
    lay1 = r.lay1; lay2 = r.lay2; lay3 = r.lay3;
    return *this;
  }
  
  ~TrigL2SiLayerRoad() {}

  int lay1, lay2;  
  std::set<int> lay3;
};

std::ostream& operator << (std::ostream& o, const struct TrigL2SiLayerRoad& r) {
  o << "SEED ROAD MAKING: road " << r.lay1 << " --> " << r.lay2 << " --> ";
  std::copy(r.lay3.begin(),r.lay3.end(),std::ostream_iterator<int>(o," "));
  return o;
}

std::ostream& operator << (std::ostream& o, const struct TrigL2SiLayerRoad* pR) {
  o << "SEED ROAD MAKING: road " << pR->lay1 << " --> " << pR->lay2 << " --> ";
  std::copy(pR->lay3.begin(),pR->lay3.end(),std::ostream_iterator<int>(o," "));
  return o;
}

#endif
