/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "./square_root.h"
#include <cmath>
double square_root(double x ){
  if (x  < 0 ){return -1.0;}
  return sqrt(x);
}
