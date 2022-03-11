/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigHTTObjects/HTTOfflineHit.h"
#include <iostream>

ClassImp(HTTOfflineHit)


std::ostream& operator<<(std::ostream& s, const HTTOfflineHit& h) {

  s << "locX " << h.getLocX()
    << " \t locY " << h.getLocY()
    << " \t isPixel " << h.isPixel()
    << " \t isBarrel " << h.isBarrel()
    << " \t layer " << h.getLayer()
    << " \t clusterID " << h.getClusterID()
    << " \t track number " << h.getTrackNumber()
    << std::endl;


  return s;
}
