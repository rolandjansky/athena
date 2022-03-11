/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

/*
 * HTTLogicalEventOutputHeader_test.cxx: Unit tests for HTTLogicalEventOutputHeader
 */

#include <cstdio>
#include <iostream>
#include <cassert>
#include "TrigHTTObjects/HTTLogicalEventOutputHeader.h"

int main(int, char**)
{
  // some random values for checking things, just put them all up here
  const int roadid(8675309);
  const double qoverpt(-0.123456), chi2(3.14159);

  HTTLogicalEventOutputHeader header;

  HTTTrack track;
  track.setQOverPt(qoverpt);
  track.setChi2(chi2);
  std::vector<HTTTrack> trackvec;
  trackvec.push_back(track);
  header.addHTTTracks_1st(trackvec);

  HTTRoad road;
  road.setRoadID(roadid);
  std::vector<HTTRoad*> roadvec;
  roadvec.push_back(&road);

  header.addHTTTracks_1st(trackvec);
  header.addHTTRoads_1st(roadvec);
  std::cout << "q/pt = " << header.getHTTTracks_1st()[0].getQOverPt()  << " and chi2 = " << 
    header.getHTTTracks_1st()[0].getChi2() << std::endl;

  std::vector<HTTRoad*> roads_1st;
  header.getHTTRoads_1st(roads_1st);
  std::cout << "Road ID = " << roads_1st[0]->getRoadID() << std::endl;

  return 0;

}

