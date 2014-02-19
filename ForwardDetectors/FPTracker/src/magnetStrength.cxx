/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "FPTracker/magnetStrength.h"
#include <cassert>
namespace FPTracker{
  double magnetStrength(int type, double length, double strength, double brho)  { 
    // calculate qpole gradient from formula g = K1L brho /L
    //                                           K1L from CERN database
    //                                           brho = 23349. = 3.345*7000 - now pbeam0/speedOflight (12//11/09)
    //                                           L = mag length
    
    assert(type>=0 and type<5);

    if(type==0 || type==4) return strength;  
    strength = brho*strength/length;

    if (type == 1){ return  strength;}
    if (type == 2){ return -strength;}
    //itype == 3 only remains....
    return strength >=0. ? strength:-strength;
    
    // strength = bend angle for dipoles, -> gradient for quadrupoles
     // type = 0 for dipole,
     //        1 for hor focussing qpoles,
     //        2 for vert focussing qpoles.
  }
}
