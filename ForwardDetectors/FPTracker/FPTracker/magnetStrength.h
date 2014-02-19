/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef FPTRACKER_MAGNETSTRENGTH_H
#define FPTRACKER_MAGNETSTRENGTH_H

namespace FPTracker{
  // calculate qpole gradient from formula g = K1L Brho /L
  //                                           K1L from CERN database
  //                                           Brho = 23349. = 3.345*7000
  //                                           L = mag length
  
  
  double  magnetStrength(int type, double length, double strength, double Brho);
}

#endif